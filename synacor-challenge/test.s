;; test program

noop ;; test comment
second:
start: push 10
push 20
jmp start
pop %0
pop %1
ss: set %2 10
out %0
print: out 'c'
jt %0 ss
out '\n'
exit:
    halt

