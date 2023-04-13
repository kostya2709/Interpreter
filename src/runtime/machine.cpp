
#include <fstream>
#include <parse/opcode.hpp>
#include <iostream>
#include "machine.hpp"
#include <mappers/int_mappers.hpp>
#include <mappers/cond_mappers.hpp>
#include <mappers/memory.hpp>
#include <file/header.hpp>
#include <utils/files.hpp>

void Interpreter::run( const std::string& filename) {
    
    size_t fileSize = getFileSize( filename);
    memoryManager_.loadFile( filename, fileSize, context_);

    while ( context_.pc != (uint64_t)memoryManager_.sectionData ) {
        Instr curInstr = context_.getNextInstr();
        processInstr( curInstr);
    }

}


void Interpreter::processInstr( const Instr& instr) {
    InstrDescr descr = getInstrDescr( instr);
    uint32_t oper = descr.oper;
    switch ( oper ) {
        case OP::ADDI:
            std::cout << "addi\n";
            AddMapper( instr, descr, context_);
            break;
        case OP::SUB:
            std::cout << "sub\n";
            SubMapper( instr, descr, context_);
            break;
        case OP::BEQ:
            std::cout << "beq\n";
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left == right; });
            break;
        case OP::BNE:
            std::cout << "bne\n";
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left != right; });
            break;
        case OP::BLT:
            std::cout << "blt\n";
            BranchMapper( instr, descr, context_, []( int32_t left, int32_t right) { return left < right; });
            break;
        case OP::BGE:
            std::cout << "bge\n";
            BranchMapper( instr, descr, context_, []( int32_t left, int32_t right) { return left >= right; });
            break;
        case OP::BLTU:
            std::cout << "bltu\n";
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left < right; });
            break;
        case OP::BGEU:
            std::cout << "bgeu\n";
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left >= right; });
            break;
        case OP::LB:
            std::cout << "lb\n";
            LoadMapper<int8_t>( instr, descr, context_);
            break;
        case OP::LH:
            std::cout << "lh\n";
            LoadMapper<int16_t>( instr, descr, context_);
            break;
        case OP::LW:
            std::cout << "lw\n";
            LoadMapper<int32_t>( instr, descr, context_);
            break;
        case OP::LD:
            std::cout << "ld\n";
            LoadMapper<int64_t>( instr, descr, context_);
            break;
        case OP::LBU:
            std::cout << "lbu\n";
            LoadMapper<uint8_t>( instr, descr, context_);
            break;
        case OP::LHU:
            std::cout << "lhu\n";
            LoadMapper<uint16_t>( instr, descr, context_);
            break;
        case OP::LWU:
            std::cout << "lwu\n";
            LoadMapper<uint32_t>( instr, descr, context_);
            break;
        case OP::SB:
            std::cout << "sb\n";
            StoreMapper<uint8_t>( instr, descr, context_);
            break;
        case OP::SH:
            std::cout << "sh\n";
            StoreMapper<uint16_t>( instr, descr, context_);
            break;
        case OP::SW:
            std::cout << "sw\n";
            StoreMapper<uint32_t>( instr, descr, context_);
            break;
        case OP::SD:
            std::cout << "sd\n";
            StoreMapper<uint64_t>( instr, descr, context_);
            break;

        default:
            throw std::runtime_error("Unexpected instruction");
    }
}

void MemoryManager::loadFile( const std::string& filename, size_t fileSize, VirtualMachineState& ctx) {
    std::ifstream file( filename, std::ios::binary);
    if ( !file.is_open() ) {
        throw std::invalid_argument("No such file");
    }
    data_.resize( fileSize + STACK_SIZE);
    file.read( reinterpret_cast<char*>( data_.data()), fileSize);
    file.close();

    Header* header = (Header*)data_.data();
    ctx.pc = reinterpret_cast<uint64_t>(data_.data() + header->entryPointOffset);
    sectionData = data_.data() + header->sectionDataEntryOffset;
    stackPtr = data_.data() + fileSize;
    stackTopPtr = stackPtr;
}

Instr VirtualMachineState::getNextInstr() {
    Instr cur_instr = *reinterpret_cast<Instr*>(pc);
    pc += sizeof(Instr);
    return cur_instr;
}