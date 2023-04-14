#pragma once

#include <configs/defs.hpp>
#include <runtime/machine.hpp>

enum InstrFmt {
    RType,
    IType,
    SType,
    BType,
    UType,
    JType
};

enum OP {

#define OPER( opcode, oper_name, name, type, idx) oper_name = opcode,
#include <configs/instructions.hpp>
#undef OPER

};


enum Reg {
#define REG( reg_name, pseudo, enum_name, idx) \
        enum_name = idx,                                      
#include <configs/regs.hpp>
#undef REG
        END
};


struct RTypeInstr {
    uint32_t opcode : 7;
    uint32_t rd     : 5;
    uint32_t funct3 : 3;
    uint32_t rs1    : 5;
    uint32_t rs2    : 5;
    uint32_t funct7 : 7;
};

struct ITypeInstr {
    uint32_t opcode : 7;
    uint32_t rd     : 5;
    uint32_t funct3 : 3;
    uint32_t rs1    : 5;
    uint32_t imm    : 12;
};

typedef
struct STypeInstr {
    uint32_t opcode : 7;
    uint32_t imm5   : 5;
    uint32_t funct3 : 3;
    uint32_t rs1    : 5;
    uint32_t rs2    : 5;
    uint32_t imm7   : 7;
} STypeInstr;

uint32_t getOpcode( Instr instr);

InstrFmt getInstrFmt( Instr instr);

struct InstrDescr {
    InstrFmt fmt;
    OP oper;
};

InstrDescr getInstrDescr( Instr instr);

enum ArgType {
    None,
    Reg,
    Imm
};

struct ArgDescr {
    ArgType type = ArgType::None;
    vmsize_t value = 0;

    vmsize_t getValue( VirtualMachineState& ctx);
    void setValue( VirtualMachineState& ctx, vmsize_t value);
};

ArgDescr getArg1( Instr instr, InstrDescr descr);
ArgDescr getArg2( Instr instr, InstrDescr descr);
ArgDescr getArg3( Instr instr, InstrDescr descr);
ArgDescr getRes(  Instr instr, InstrDescr descr);