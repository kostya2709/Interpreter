#pragma once

#include <parse/opcode.hpp>
#include <iostream>

template <typename Condition>
void BranchMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx, Condition&& cond) {
    
    ArgDescr arg1 = getArg1( instr, descr);
    ArgDescr arg2 = getArg2( instr, descr);
    ArgDescr arg3 = getArg3( instr, descr);

    if ( cond( arg1.getValue(ctx), arg2.getValue(ctx)) ) {
        ctx.pc += arg3.getValue(ctx);
    }
}
