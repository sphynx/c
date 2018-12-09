    set %0 hello_str
    call print_string
    halt

#include "io.s"

hello_str:
    ;; dw "hello, world"
    dw 'h'
    dw 'e'
    dw 'l'
    dw 'l'
    dw 'o'
    dw '!'
    dw '\n'
    dw 0
