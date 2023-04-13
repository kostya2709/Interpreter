#pragma once

#include <parse/opcode.hpp>

void AddMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx);
void SubMapper( Instr instr, const InstrDescr& descr, VirtualMachineState& ctx);
