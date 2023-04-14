#pragma once

#include <parse/opcode.hpp>

template <typename size>
void LoadMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx) {
    
    ArgDescr arg1 = getArg1( instr, descr);
    ArgDescr arg2 = getArg2( instr, descr);
    ArgDescr res = getRes( instr, descr);

    res.setValue( ctx, *(reinterpret_cast<size*>( arg1.getValue( ctx)) + arg2.getValue( ctx)));
}

template <typename size>
void StoreMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx) {
    
    ArgDescr arg1 = getArg1( instr, descr);
    ArgDescr arg2 = getArg2( instr, descr);
    ArgDescr arg3 = getArg3( instr, descr);

    size* ptr = reinterpret_cast<size*>( arg1.getValue( ctx)) + arg3.getValue( ctx);
    *ptr = arg2.getValue(ctx);
}