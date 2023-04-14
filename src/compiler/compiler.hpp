#pragma once

#include <string>
#include <vector>
#include <parse/opcode.hpp>
#include <unordered_map>

class Tokenizer {
public:
    void loadFile( const std::string& filename, size_t size);
    std::vector<std::vector<std::string>> tokens_;
private:
};

struct LabelInfo {
    size_t offset = 0;
    bool isInText = false;
};

struct LabelRequest {
    Instr instr;
    InstrFmt fmt;
    size_t label;
};

class Compiler {
public:
    void run( const std::string& filename);
    void setInText( bool flag);
    bool getInText();

    void addInstr( Instr instr);
    void addLiteral( const std::string& str);
    void addLabel( const std::string& label);
    LabelInfo getLabel( const std::string& label);
    void addLabelRequest( LabelRequest request);

    void replyLabelRequests();
    void writeToFile( const std::string& fileName);
private:
    Tokenizer tokenizer_;
    std::vector<Instr> text_;
    std::vector<uint8_t> data_;
    std::unordered_map<size_t, LabelInfo> labels_;
    std::unordered_map<size_t, LabelRequest> requests_;
    bool isInText = true;
};

void encode( const std::vector<std::string>& tokens, Compiler& compiler);
Instr setImm( Instr instr, int64_t imm, InstrFmt fmt);
