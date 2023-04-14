
#include <fstream>
#include <parse/opcode.hpp>
#include <iostream>
#include "machine.hpp"
#include <mappers/int_mappers.hpp>
#include <mappers/cond_mappers.hpp>
#include <mappers/call.hpp>
#include <mappers/memory.hpp>
#include <file/header.hpp>
#include <utils/files.hpp>

void Interpreter::run( const std::string& filename) {
    
    size_t fileSize = getFileSize( filename);
    memoryManager_.loadFile( filename, fileSize, context_);

    while ( !context_.errorCode.has_value() ) {
        Instr curInstr = context_.getNextInstr();
        processInstr( curInstr);
    }

}


void Interpreter::processInstr( const Instr& instr) {
    InstrDescr descr = getInstrDescr( instr);
    uint32_t oper = descr.oper;
    switch ( oper ) {
        case OP::ADDI:
            AddMapper( instr, descr, context_);
            break;
        case OP::SUB:
            SubMapper( instr, descr, context_);
            break;
        case OP::BEQ:
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left == right; });
            break;
        case OP::BNE:
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left != right; });
            break;
        case OP::BLT:
            BranchMapper( instr, descr, context_, []( int32_t left, int32_t right) { return left < right; });
            break;
        case OP::BGE:
            BranchMapper( instr, descr, context_, []( int32_t left, int32_t right) { return left >= right; });
            break;
        case OP::BLTU:
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left < right; });
            break;
        case OP::BGEU:
            BranchMapper( instr, descr, context_, []( uint32_t left, uint32_t right) { return left >= right; });
            break;
        case OP::LB:
            LoadMapper<int8_t>( instr, descr, context_);
            break;
        case OP::LH:
            LoadMapper<int16_t>( instr, descr, context_);
            break;
        case OP::LW:
            LoadMapper<int32_t>( instr, descr, context_);
            break;
        case OP::LD:
            LoadMapper<int64_t>( instr, descr, context_);
            break;
        case OP::LBU:
            LoadMapper<uint8_t>( instr, descr, context_);
            break;
        case OP::LHU:
            LoadMapper<uint16_t>( instr, descr, context_);
            break;
        case OP::LWU:
            LoadMapper<uint32_t>( instr, descr, context_);
            break;
        case OP::SB:
            StoreMapper<uint8_t>( instr, descr, context_);
            break;
        case OP::SH:
            StoreMapper<uint16_t>( instr, descr, context_);
            break;
        case OP::SW:
            StoreMapper<uint32_t>( instr, descr, context_);
            break;
        case OP::SD:
            StoreMapper<uint64_t>( instr, descr, context_);
            break;
        case OP::ECALL:
            syscallMapper( context_, memoryManager_);
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
    if ( header->entryPointOffset == -1 ) {
        throw std::runtime_error( "No entry point. Set \"_start\" label");
    }
    ctx.pc = reinterpret_cast<uint64_t>(data_.data() + header->entryPointOffset);
    sectionTextPtr = data_.data() + sizeof(header);
    sectionData = data_.data() + header->sectionDataEntryOffset;
    stackPtr = data_.data() + fileSize + STACK_SIZE;
    stackTopPtr = stackPtr;
    ctx.registers[Reg::SP] = reinterpret_cast<IntReg>( stackPtr);
}

Instr VirtualMachineState::getNextInstr() {
    Instr cur_instr = *reinterpret_cast<Instr*>(pc);
    pc += sizeof(Instr);
    return cur_instr;
}