;;; prints 0-terminated string pointed by address in %0
;;; returns number of printed characters in %0
print_string:
    push %1
    push %2
    set %1 0

ps_loop:
    rmem %2 %0
    jf %2 ps_exit
    out %2
    add %1 %1 1
    add %0 %0 1
    jmp ps_loop

ps_exit:
    set %0 %1
    pop %2
    pop %1
    ret
