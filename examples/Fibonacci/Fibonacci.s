_start:     # entry point

    addi  a0, x0, 1      # 1 = StdOut
    addi  a1, a1, helloworld # Pointer to string
    addi  a2, x0, 13     # length of our string
    addi  a7, x0, 64     # linux write system call
    ecall                # Call linux to output the string

    addi t0, x0, 5      # Loop counter
    addi t1, x0, 0      # Cur Fib
    addi t2, x0, 1      # Prev Fib
    addi t4, x0, 1      # Const 1

Fib_loop:
    beq t0, x0, Fib_end_loop
    addi t3, x0, t2
    addi t2, x0, t1
    addi t1, t1, t3
    sub  t0, t0, t4
    beq t0, t0, Fib_loop

Fib_end_loop:
    

exit:
    addi a0, x0, 0      # Use 0 return code
    addi a7, x0, 93     # Service command code 93 terminates
    ecall               # Call linux to terminate the program

.data
   helloworld: "Calculating Fib(5)"
