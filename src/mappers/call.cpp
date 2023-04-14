#include "call.hpp"

#include <string>

void syscallMapper( VirtualMachineState& ctx, MemoryManager& memManager) {

    IntReg syscallNum = ctx.registers[Reg::A7];
    switch ( syscallNum ) {

        case Syscall::WRITE:
            {
                IntReg stream  = ctx.registers[Reg::A0];
                if ( stream != 1 ) {
                    throw std::runtime_error( "Only Outstream is supported in syscall");
                }
                IntReg address = ctx.registers[Reg::A1] + reinterpret_cast<IntReg>(memManager.sectionTextPtr);
                IntReg length  = ctx.registers[Reg::A2];
                fprintf( stdout, reinterpret_cast<const char*>(address), length);
            }
            break;
        case Syscall::READ:
            {
                IntReg stream  = ctx.registers[Reg::A0];
                if ( stream != 0 ) {
                    throw std::runtime_error( "Only stdin is supported in syscall");
                }
                IntReg address = ctx.registers[Reg::A1] + reinterpret_cast<IntReg>(memManager.sectionTextPtr);
                IntReg length  = ctx.registers[Reg::A2];
                fgets( reinterpret_cast<char*>(address), length, stdin);
            }
            break;

        case Syscall::EXIT:
            printf( "t1: %llu\n", ctx.registers[Reg::T1]);
            ctx.errorCode = ctx.registers[Reg::A0];
            break;

        default:
            throw std::runtime_error( "Unsupported syscall number " + std::to_string( syscallNum));
    };
}