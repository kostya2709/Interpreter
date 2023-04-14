#include "int_mappers.hpp"
#include <iostream>

void AddMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx) {
    
    ArgDescr arg1 = getArg1( instr, descr);
    ArgDescr arg2 = getArg2( instr, descr);
    ArgDescr res = getRes( instr, descr);

    res.setValue( ctx, arg1.getValue(ctx) + arg2.getValue(ctx));
}

void SubMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx) {
    
    ArgDescr arg1 = getArg1( instr, descr);
    ArgDescr arg2 = getArg2( instr, descr);
    ArgDescr res = getRes( instr, descr);

    res.setValue( ctx, arg1.getValue(ctx) - arg2.getValue(ctx));
}