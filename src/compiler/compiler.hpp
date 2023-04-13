#pragma once

#include <string>
#include <vector>
#include <parse/opcode.hpp>

class Tokenizer {
public:
    void loadFile( const std::string& filename, size_t size);
    std::vector<std::vector<std::string>> tokens_;
private:
};


class Compiler {
public:
    void run( const std::string& filename);
    void setInText( bool flag);
    bool getInText();
    void addInstr( Instr instr);
    void addLiteral( const std::string& str);
    void writeToFile( const std::string& fileName);
private:
    Tokenizer tokenizer_;
    std::vector<Instr> text_;
    std::vector<uint8_t> data_;
    bool isInText = true;
};

void encode( const std::vector<std::string>& tokens, Compiler& compiler);