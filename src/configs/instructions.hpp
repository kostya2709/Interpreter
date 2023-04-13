
OPER( 0x00000013, ADDI, "addi", IType, 0)

OPER( 0x00000033, ADD, "add", RType, 0)
OPER( 0x80000033, SUB, "sub", RType, 0)

OPER( 0x00000063, BEQ, "beq", BType, 0)
OPER( 0x00001063, BNE, "bne", BType, 0)
OPER( 0x00004063, BLT, "blt", BType, 0)
OPER( 0x00005063, BGE, "bge", BType, 0)
OPER( 0x00006063, BLTU, "bltu", BType, 0)
OPER( 0x00007063, BGEU, "bgeu", BType, 0)

OPER( 0x00000003, LB,  "lb", IType, 0)
OPER( 0x00001003, LH,  "lh", IType, 0)
OPER( 0x00002003, LW,  "lw", IType, 0)
OPER( 0x00003003, LD,  "ld", IType, 0)
OPER( 0x00004003, LBU, "lbu", IType, 0)
OPER( 0x00005003, LHU, "lhu", IType, 0)
OPER( 0x00006003, LWU, "lwu", IType, 0)

OPER( 0x00000023, SB, "sb", SType, 0)
OPER( 0x00001023, SH, "sh", SType, 0)
OPER( 0x00002023, SW, "sw", SType, 0)
OPER( 0x00003023, SD, "sd", SType, 0)