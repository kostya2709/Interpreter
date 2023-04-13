#include "opcode.hpp"
#include <stdexcept>
#include <string>

uint32_t getOpcode( Instr instr) {
    return 0x7f & instr;
}

uint32_t getOpcodeFunct( Instr instr) {
    InstrFmt fmt = getInstrFmt( instr);
    uint32_t mask = 0x7f;
    switch ( fmt ) {
        case InstrFmt::RType:
            mask |= 0x7000;
            mask |= 0xfe000000;
            break;
        case InstrFmt::IType:
        case InstrFmt::SType:
        case InstrFmt::BType:
            mask |= 0x7000;
            break;
        case InstrFmt::UType:
        case InstrFmt::JType:
            break;
        default:
            std::invalid_argument( "Unexpected format " + std::to_string( fmt));
    }
    return instr & mask;
}

InstrFmt getInstrFmt( Instr instr) {
    uint32_t opc = getOpcode( instr);
    uint32_t opc_mask = 0x7f;
    InstrFmt fmt;
    printf("instrrr 0x%x\n", opc);
    if (0) {}
#define OPER(opcode, oper_name, name, type, idx) \
    else if ( (opc_mask & opcode) == opc ) { \
        fmt = type; \
    }
    #include <configs/instructions.hpp>
#undef OPER
    else {
        throw std::invalid_argument( "Invalid instruction ");
    }

    return fmt;
}

InstrDescr getInstrDescr( Instr instr) {
    InstrDescr descr;
    uint32_t opcode_fnct = getOpcodeFunct( instr);
    printf("opcode_funct: 0x%x\n", opcode_fnct);
    switch ( opcode_fnct ) {
#define OPER( opcode, oper_name, name, type, idx)   \
    case opcode:                                    \
        descr.fmt = type;                           \
        descr.oper = OP::oper_name;                 \
        break;
#include <configs/instructions.hpp>
#undef OPER
    };
    printf( "oper: %x\n", descr.oper);
    return descr;
}

static int32_t signExtend( uint32_t value) {
    if ( value & (1 << 11) ) {
        value |= 0xfffff000;
    }
    return value;
}

ArgDescr getArg1( Instr instr, InstrDescr descr) {
    ArgDescr arg;
    switch ( descr.fmt ) {
        case InstrFmt::RType:
        case InstrFmt::IType:
        case InstrFmt::SType:
        case InstrFmt::BType:
            arg.type = ArgType::Reg;
            arg.value = (instr & 0xf8000) >> 15;
            break;
        default:
            std::invalid_argument(" Unexpected format" + std::to_string(descr.fmt));
    }
    return arg;
}

ArgDescr getArg2( Instr instr, InstrDescr descr) {
    ArgDescr arg;
    int32_t val;
    switch ( descr.fmt ) {
        case InstrFmt::RType:
        case InstrFmt::SType:
        case InstrFmt::BType:
            arg.type = ArgType::Reg;
            arg.value = (instr & 0x1f00000) >> 20;
            break;
        case InstrFmt::IType:
            arg.type = ArgType::Imm;
            val = (instr & 0xfff00000) >> 20;
            arg.value = signExtend( val);
        default:
            std::invalid_argument(" Unexpected format" + std::to_string(descr.fmt));
    }
    return arg;
}

ArgDescr getArg3( Instr instr, InstrDescr descr) {
    ArgDescr arg;
    arg.type = ArgType::Imm;
    uint32_t val = 0;
    switch ( descr.fmt ) {
        case InstrFmt::BType:
            val |= (instr & 0xf00) >> 7;
            val |= (instr & 0xee000000) >> (25 - 5);
            val |= (instr & (1 << 7)) << 4;
            val |= (instr & (1 << 31)) >> 9;
            break;
        case InstrFmt::SType:
            val |= (instr & 0xf80) >> 7;
            val |= (instr & 0xfe000000) >> (25 - 5);
            break;
        default:
            std::invalid_argument(" Unexpected format" + std::to_string(descr.fmt));
    }
    arg.value = signExtend( val);
    return arg;
}

ArgDescr getRes( Instr instr, InstrDescr descr) {
    ArgDescr arg;
    switch ( descr.fmt ) {
        case InstrFmt::RType:
        case InstrFmt::IType:
            arg.type = ArgType::Reg;
            arg.value = (instr & 0xf80) >> 7;
            break;
        case InstrFmt::SType:
        case InstrFmt::UType:

        default:
            std::invalid_argument(" Unexpected format" + std::to_string(descr.fmt));
    }
    return arg;
}

vmsize_t ArgDescr::getValue( VirtualMachineState& ctx) {
    if ( type == ArgType::None ) {
        throw std::runtime_error( "Attempt to get value of an uninitialized arg");
    } else if ( type == ArgType::Reg ) {
        if ( value >= ctx.registers.size() ) {
            throw std::runtime_error( "Invalid register number " + std::to_string( value));
        } else {
            return ctx.registers[value];
        }
    } else { // type == ArgType::Imm
        return value;
    }
}

ArgDescr& ArgDescr::setValue( VirtualMachineState& ctx, vmsize_t new_val) {
    if ( type == ArgType::None ) {
        throw std::runtime_error( "Attempt to set value of an uninitialized arg");
    } else if ( type == ArgType::Reg ) {
        if ( value >= ctx.registers.size() ) {
            throw std::runtime_error( "Invalid register number " + std::to_string( value));
        } else {
            ctx.registers[value] = new_val;
            return *this;
        }
    } else if ( type == ArgType::Imm ) {
        throw std::runtime_error( "Attempt to assign to immediate");
    }
}