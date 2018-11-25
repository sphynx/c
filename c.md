# Notes on C and related topics.

## String literals.

String literals can be either mutable or immutable. If you want to
make sure they are mutable, you have to define them like this:

```c
char str[] = "test";
str[0] = 'q';
```

Then they are allocated on stack, `str` can be used as a pointer, but
returning a pointer to this string is not a good idea, since it will
be deallocated on function exit.

If we define a string like this:

```c
char* str = "test";
```

Then this function will most likely (compiler dependent?) be stored in
read-only data-segment and therefore will be accessible to all
functions even after the function containing that declaration will
return, so it is more or less safe to return a pointer `str`.

Details:
https://stackoverflow.com/questions/2589949/string-literals-where-do-they-go

However, if you want to make sure it's safe to return, we can use
something like

```c
char* str = strdup("test");
```

which will definitely allocated "test" in heap.

## Disassembling the code with `objdump`.

It looks like it's possible to see what binary code has been generated
by the compiler using `objdump` (I use LLVM version of it):

```
> cat test.c
```

```c
#include <stdio.h>

int main() {
    char *s = "abc";
    printf("%s\n", s);
    return 0;
}
```

```
> gcc -g -c test.c
> objdump -source test.o  # prints source code along disassembled lines
```

```
test.o:	file format Mach-O 64-bit x86-64

Disassembly of section __TEXT,__text:
_main:
; int main() {
       0:	55 	pushq	%rbp
       1:	48 89 e5 	movq	%rsp, %rbp
       4:	48 83 ec 20 	subq	$32, %rsp
       8:	48 8d 3d 2e 00 00 00 	leaq	46(%rip), %rdi
       f:	48 8d 05 23 00 00 00 	leaq	35(%rip), %rax
      16:	c7 45 fc 00 00 00 00 	movl	$0, -4(%rbp)
; char *s = "abc";
      1d:	48 89 45 f0 	movq	%rax, -16(%rbp)
; printf("%s\n", s);
      21:	48 8b 75 f0 	movq	-16(%rbp), %rsi
      25:	b0 00 	movb	$0, %al
      27:	e8 00 00 00 00 	callq	0 <_main+0x2c>
      2c:	31 c9 	xorl	%ecx, %ecx
; return 0;
      2e:	89 45 ec 	movl	%eax, -20(%rbp)
      31:	89 c8 	movl	%ecx, %eax
      33:	48 83 c4 20 	addq	$32, %rsp
      37:	5d 	popq	%rbp
      38:	c3 	retq
```

## Reading C declarators:

To understand what this means:
```c
char *(*(**foo [][8])())[];
```

Or to understand LDDB/GDB pretty-printing trick:
```
p *(int(*)[32]) x
```

You can look at some of those articles and sites:

[http://www.unixwiz.net/techtips/reading-cdecl.html](Stephen Friedl)
[https://eli.thegreenplace.net/2008/07/18/reading-c-type-declarations](Eli Benderski)
[https://cdecl.org/](cdecl.org)

Also, reading K&R book, chapter 5.12 "Complicated declarations" helps
a lot. They also provide an example program which transforms
declarations to human readable string (similar to cdecl, but working
under simplified grammar).

Another interesting point is illustrated in section 5.7
"Multi-dimensional arrays" which demonstrates that

int *daytab[13]

and

int (*daytab)[13]

are very different, since [] bind tighter than *. Therefore, the first
one is 13-element array of pointers to int and the second one, is a
pointer to 13-element array of int.

This is directly related to "p *(int(*)[32]) x" gdb trick, showing why
parenthesis are needed there.

Having something like "p *(int*[32]) x" would mean that we ask to
treat that x as an array of pointers, not a pointer to array.


## To show which macros are predefined in C compilers:

gcc -x c /dev/null -dM -E
clang -x c /dev/null -dM -E

## To check for OS:

Linux and Linux-derived           __linux__
Darwin (Mac OS X and iOS)         __APPLE__
FreeBSD                           __FreeBSD__
Windows                           _WIN32
Windows 64 bit                    _WIN64 (implies _WIN32)

(from https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html )
