#include "compiler.hpp"
#include <utils/files.hpp>
#include <fstream>
#include <iostream>
#include <parse/opcode.hpp>
#include <file/header.hpp>
#include <unordered_map>

typedef std::string::iterator StrIter;
typedef std::string::const_iterator ConstStrIter;
typedef std::vector<std::string>::const_iterator TokenIter;

void Compiler::run( const std::string& fileName) {
    
    size_t fileSize = getFileSize( fileName);
    tokenizer_.loadFile( fileName, fileSize);

    for ( const auto& line : tokenizer_.tokens_ ) {
        encode( line, *this);
    }
    writeToFile( "test.bc");
}

void Compiler::addInstr( Instr instr) {
    text_.push_back( instr);
}

void Compiler::addLiteral( const std::string& str) {
    for ( const auto& sym : str ) {
        data_.push_back( sym);
    }
    data_.push_back( 0);    // Add terminate symbol
}

bool Compiler::getInText() {
    return isInText;
}

void Compiler::setInText( bool flag) {
    isInText = flag;
}

bool isEmpty( const std::string& str) {
    for ( const char& sym : str ) {
        if ( sym == '#' ) {
            return true;
        }
        if ( std::isalpha( sym) ) {
            return false;
        }
    }
    return true;
}

void skipSpaces( StrIter& iter, const StrIter& end) {
    while ( ( iter != end ) && (( *iter == ' ' ) || ( *iter == '\v' ) || ( *iter == '\t' ) || ( *iter == '#' ) ) ) {
        if ( *iter == '#' ) {
            iter = end;
            return;
        }
        ++iter;
    }
}

std::string getToken( StrIter& iter, const StrIter& end) {
    if ( *iter == ',' ) {
        char tmp = *iter;
        ++iter;
        return std::string(1, tmp);
    }
    std::string token;
    while ( ( iter != end ) && ( *iter != ' ' ) && ( *iter != '\v' ) && ( *iter != '\t' ) && ( *iter != ',' )) {
        if ( *iter == '#' ) {
            iter = end;
            return token;
        }
        token.push_back( *iter);
        ++iter;
    }
    return token;
}

void Tokenizer::loadFile( const std::string& filename, size_t size) {
    std::ifstream file( filename, std::ios::in);
    if ( !file.is_open() ) {
        throw std::invalid_argument("No such file");
    }

    std::string line;
    while ( std::getline(file, line) )
    {
        if ( isEmpty( line) ) {
            continue;
        }
        std::cout << line << "\n";
        std::vector<std::string> parsed;

        auto iter = line.begin();
        auto iter_end = line.end();
        while ( iter != iter_end ) {
            skipSpaces( iter, iter_end);
            if ( iter == iter_end ) {
                break;
            }
            std::string token = getToken( iter, iter_end);
            std::cout << token << "\n";
            parsed.push_back( std::move( token));
        }
        std::cout << "PARSED LINE\n";
        tokens_.push_back( std::move( parsed));
    }
}

void validateTokensEnd( const TokenIter& token, const TokenIter& token_end) {
    if ( token != token_end ) {
        throw std::runtime_error( "Expected end of line, encountered " + *token);
    }
}

void parseDirective( const std::string& directive, Compiler& ctx) {
    if ( directive == ".data" ) {
        ctx.setInText( false);
    } else if ( directive == ".data" ) {
        ctx.setInText( true);
    } else
    {
        throw std::runtime_error( "Unexpected directive " + directive);
    }
}

void addLabel() {

}

uint32_t getRegNum( const std::string& str) {
    uint32_t reg_num = 0;
    if ( 0 ) {}
#define REG( reg_name, pseudo, idx)                         \
    else if ( (str == reg_name) || (str == pseudo) ) {      \
        reg_num = idx;                                      \
    }
#include <configs/regs.hpp>
    else {
        throw std::runtime_error( "Undefined register " + str);
    }
#undef REG
    return reg_num;
}

uint32_t getRegNum( TokenIter& token, const TokenIter& token_end) {

    if ( token == token_end ) {
        std::runtime_error( "Expected a register, encountered end of line");
    }

    uint32_t reg_num = getRegNum( *token);
    ++token;
    return reg_num;
}

void skipComma( TokenIter& token, const TokenIter& token_end) {
    
    if ( token == token_end ) {
        std::runtime_error( "Expected a comma, encountered end of line");
    }

    if ( *token !=  ",") {
        std::runtime_error( "Expected a comma separator!");
    }
    ++token;
}

int64_t getImm( TokenIter& token, const TokenIter& token_end) {

    if ( token == token_end ) {
        std::runtime_error( "Expected a comma, encountered end of line");
    }

    int64_t res = 0;
    try {
        res = std::stoll( *token);
    } catch (...) {
        throw std::runtime_error( "Expected an immediate value, encountered " + *token);
    }
    ++token;
    return res;
}

void validateRegNum( uint32_t reg_num) {
    if ( reg_num >= REG_NUM ) {
        throw std::runtime_error( "Invalid register number " + std::to_string( reg_num));
    }
}

void unexpectedTokenEnd( TokenIter& token, const TokenIter& token_end, const std::string& expected) {
    if ( token == token_end ) {
        std::runtime_error( "Expected " + expected + ", encountered end of line");
    }
}

std::unordered_map<std::string, int64_t> labels;

void getLabelOffset( TokenIter& token, const TokenIter& token_end) {
    unexpectedTokenEnd( token, token_end, "a label name");
    
    auto found = labels.find( *token + ":");
    if ( found != labels.end() ) {
        // TODO:
    } 
}

std::pair<int64_t, std::string> addressTokenize( TokenIter& token, const TokenIter& token_end) {
    
    unexpectedTokenEnd( token, token_end, "a offset(base) addres");

    ConstStrIter iter = token->begin();
    int64_t offset = 0;
    bool isNegative = false;
    if ( *iter == '-' ) {
        isNegative = true;
        ++iter;
    }

    unexpectedTokenEnd( token, token_end, "an offset digit");
    while ( std::isdigit( *iter) ) {
        offset *= 10;
        offset += *iter - '0';
        ++iter;
        unexpectedTokenEnd( token, token_end, "an offset digit");
    }

    if ( isNegative ) {
        offset *= -1;
    }

    if ( *iter != '(' ) {
        throw std::runtime_error( "Expected an opening bracket in address, encountered " + std::string(1, *iter));
    }
    ++iter;
    unexpectedTokenEnd( token, token_end, "register");

    std::string reg_name;
    while ( std::isalpha( *iter) ) {
        reg_name.push_back( *iter);
        ++iter;
        unexpectedTokenEnd( token, token_end, "register");
    }
    if ( *iter != ')' ) {
        throw std::runtime_error( "Expected a closing bracket in address, encountered " + std::string(1, *iter));
    }
    ++iter;
    ++token;

    return std::make_pair( offset, std::move( reg_name));
}

void setRd( Instr& instr, uint32_t reg_num) {
    validateRegNum( reg_num);
    instr |= (reg_num << 7);
}

void setRs1( Instr& instr, uint32_t reg_num) {
    validateRegNum( reg_num);
    instr |= (reg_num << 15);
}

void setRs2( Instr& instr, uint32_t reg_num) {
    validateRegNum( reg_num);
    instr |= (reg_num << 20);
}

Instr encodeRType( TokenIter& token, const TokenIter& token_end, uint32_t opcode) {
    std::cout << "Parsing RType\n";
    Instr instr = opcode;
    IntReg rd = getRegNum( token, token_end);
    skipComma( token, token_end);
    IntReg rs1 = getRegNum( token, token_end);
    skipComma( token, token_end);
    IntReg rs2 = getRegNum( token, token_end);

    validateTokensEnd( token, token_end);

    setRd(  instr, rd);
    setRs1( instr, rs1);
    setRs2( instr, rs2);

    printf( "INSTR 0x%x\n", instr);

    return instr;
}

Instr encodeIType( TokenIter& token, const TokenIter& token_end, uint32_t opcode) {
    Instr instr = opcode;
    IntReg rd = getRegNum( token, token_end);
    skipComma( token, token_end);
    IntReg rs1 = getRegNum( token, token_end);
    skipComma( token, token_end);
    int64_t imm = getImm( token, token_end);

    validateTokensEnd( token, token_end);

    setRd( instr, rd);
    setRs1( instr, rs1);
    instr |= (imm & 0xfff) << 20;

    printf( "INSTR 0x%x\n", instr);
    return instr;
}

Instr encodeSType( TokenIter& token, const TokenIter& token_end, uint32_t opcode) {
    std::cout << "Parsing SType\n";
    Instr instr = opcode;
    IntReg rs2 = getRegNum( token, token_end);
    skipComma( token, token_end);

    int64_t offset = 0;
    std::string reg_name;
    std::tie(offset, reg_name) = addressTokenize( token, token_end);
    IntReg rs1 = getRegNum( reg_name);

    validateTokensEnd( token, token_end);

    setRs1( instr, rs1);
    setRs2( instr, rs2);

    offset &= 0xfff;
    instr |= (offset & 0x1f) << 7;
    instr |= (offset & 0xfe0) << (25 - 5);

    printf( "INSTR 0x%x\n", instr);
    return instr;
}

Instr encodeBType( TokenIter& token, const TokenIter& token_end, uint32_t opcode) {
    std::cout << "Parsing BType\n";

    Instr instr = opcode;
    IntReg rs1 = getRegNum( token, token_end);
    skipComma( token, token_end);
    IntReg rs2 = getRegNum( token, token_end);
    skipComma( token, token_end);

    int64_t offset = 0;
    std::string reg_name;
    std::tie(offset, reg_name) = addressTokenize( token, token_end);

    validateTokensEnd( token, token_end);

    setRs1( instr, rs1);
    setRs2( instr, rs2);

    offset &= 0xfff;
    instr |= (offset & 0x1e) << 7;
    instr |= (offset & (1 << 11)) >> (11 - 7);
    instr |= (offset & 0x3e0) << (25 - 5);
    instr |= (offset & (1 << 12)) >> (31 - 12);

    printf( "INSTR 0x%x\n", instr);
    return instr;
}

Instr encodeUType( TokenIter& token, const TokenIter& token_end, uint32_t opcode) {
    // TODO:
    // std::cout << "Parsing UType\n";
    // Instr instr = opcode;
    // IntReg rd = getRegNum( token, token_end);
    // skipComma( token, token_end);
    // int64_t imm = getImm( token, token_end);

    // validateTokensEnd( token, token_end);

    // setRd( instr, rd);
    // instr |= (imm & 0xfffff000) << 12;

    // printf( "INSTR 0x%x\n", instr);
}

Instr encodeJType( TokenIter& token, const TokenIter& token_end, uint32_t opcode) {
    std::cout << "Parsing JType\n";
}

void encode( const std::vector<std::string>& tokens, Compiler& ctx) {
    if ( tokens.empty() ) {
        return;
    }

    TokenIter token = tokens.begin();
    TokenIter tokens_end = tokens.end();

    if ( token->empty() ) {
        throw std::runtime_error( "Broken argument. Empty token");
    }

    if ( token->front() == '.' ) {
        if ( tokens.size() != 1 ) {
            throw std::runtime_error( "Expected only one token in a directive");
        }
        parseDirective( *token, ctx);
        return;
    }

    if ( token->back() == ':' ) {
        addLabel();
        ++token;
    }

    const std::string& token_oper = *token;
    Instr result;

    if ( 0 ) {}
#define OPER( opcode, oper_name, name, type, idx)           \
    else if ( token_oper == name ) {                        \
        ++token;                                            \
        result = encode##type( token, tokens_end, opcode);  \
    }
#include <configs/instructions.hpp>
#undef OPER
    else {
        throw std::runtime_error( "Unexpected instruction " + *token);
    }

    if ( ctx.getInText() ) {
        ctx.addInstr( result);
    } else {
        // TODO
    }
}

void Compiler::writeToFile( const std::string& fileName) {
    
    // TODO: label
    std::ofstream outFile( fileName, std::ios::binary);
    Header header;
    header.entryPointOffset = 0; // TODO: _start
    header.sectionDataEntryOffset = sizeof(Header) + text_.size() * sizeof(Instr);
    outFile << header;

    for ( const auto& instr : text_ ) {
        outFile << instr;
    }

    for ( const auto& byte : data_ ) {
        outFile << byte;
    }

    outFile.close();
}
