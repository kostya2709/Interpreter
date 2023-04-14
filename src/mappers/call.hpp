#pragma once

#include <parse/opcode.hpp>


enum Syscall {
    READ  = 63,
    WRITE = 64,
    EXIT  = 93
};

void syscallMapper( VirtualMachineState& ctx, MemoryManager& memManager);