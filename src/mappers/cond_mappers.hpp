#pragma once

#include <parse/opcode.hpp>
#include <iostream>

template <typename Condition>
void BranchMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx, Condition&& cond) {
    
    ArgDescr arg1 = getArg1( instr, descr);
    ArgDescr arg2 = getArg2( instr, descr);
    ArgDescr arg3 = getArg3( instr, descr);
    ArgDescr res = getRes( instr, descr);

    std::cout << arg1.type << " " << arg1.value << "\n";
    std::cout << arg2.type << " " << arg2.value << "\n";
    std::cout << arg3.type << " " << arg3.value << "\n";
    std::cout << res.type << " " << res.value << "\n";

    if ( cond( arg1.getValue(ctx), arg2.getValue(ctx)) ) {
        ctx.pc += arg3.getValue(ctx);
    }
}