;;; Prints factorial of a single digit number.
;;; NB. It prints digits of result in inverted order, e.g. 5! will be
;;; printed as '021'.

    set %0 1
    in %1                       ; stores ascii code of digit
    add %1 %1 32720             ; -'0' (32768-48) to convert to number
loop:
    eq %2 %1 1
    jt %2 exit_1
    mult %0 %0 %1
    add %1 %1 32767
    jmp loop
exit_1:
    set %3 %0
loop_2:
    mod %4 %3 10
    add %4 %4 48                ; adds '0'
    out %4
    call div_by_10
    jf %3 exit_2
    jmp loop_2
exit_2:
    halt

div_by_10:                      ; div by subtraction ;)
    set %5 %3
    set %6 0
loop_3:
    add %3 %3 32758             ; -10
    gt %7 %3 32757              ; if < 0
    jt %7 exit_3
    add %6 %6 1
    jmp loop_3
exit_3:
    set %3 %6
    ret
