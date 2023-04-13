#pragma once

#include <array>
#include <vector>
#include <configs/defs.hpp>

class VirtualMachineState {

public:
    Instr getNextInstr();

    std::array<IntReg, REG_NUM> registers;
    IntReg pc;
};

class MemoryManager {
public:
    void loadFile( const std::string& filename, size_t size, VirtualMachineState& ctx);
// private:
public:
    std::vector<uint8_t> data_;
    uint8_t* sectionTextPtr;
    uint8_t* sectionData;
    uint8_t* stackPtr;
    uint8_t* stackTopPtr;
};

class Interpreter {
public:
    void run( const std::string& filename);
    void processInstr( const Instr& instr);
private:
    MemoryManager memoryManager_;
    VirtualMachineState context_;
};
