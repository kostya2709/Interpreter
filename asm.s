
_start:     # entry point

    addi  a0, x0, 1      # 1 = StdOut
    addi  a1, a1, helloworld # Pointer to string
    addi  a2, x0, 13     # length of our string
    addi  a7, x0, 64     # linux write system call
    ecall                # Call linux to output the string

    addi a0, x0, 0      # Use 0 return code
    addi a7, x0, 93     # Service command code 93 terminates
    ecall               # Call linux to terminate the program

.data
   helloworld: "Hello, World"
