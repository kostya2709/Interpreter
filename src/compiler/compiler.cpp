#include "compiler.hpp"
#include <utils/files.hpp>
#include <fstream>
#include <iostream>
#include <parse/opcode.hpp>
#include <file/header.hpp>
#include <unordered_map>


typedef std::hash<std::string> StrHasher;
typedef std::string::iterator StrIter;
typedef std::string::const_iterator ConstStrIter;
typedef std::vector<std::string>::const_iterator TokenIter;

void Compiler::run( const std::string& fileName) {
    
    std::string asmFile = fileName + ".s";
    std::string byteCodeFile = fileName + ".bc";
    size_t fileSize = getFileSize( asmFile);
    tokenizer_.loadFile( asmFile, fileSize);

    for ( const auto& line : tokenizer_.tokens_ ) {
        encode( line, *this);
    }
    replyLabelRequests();
    writeToFile( byteCodeFile);
}


void Compiler::replyLabelRequests() {
    for ( const auto& [pos, labelReq] : requests_ ) {
        auto foundLabel = labels_.find( labelReq.label);
        if ( foundLabel == labels_.end() ) {
            throw std::runtime_error( "Reference to an unexpected label");
        }
        LabelInfo* info = &foundLabel->second;
        size_t offset = info->offset;
        if ( labelReq.fmt == InstrFmt::BType ) {
            offset -= pos;
        }
        if ( !info->isInText ) {
            offset += text_.size() * sizeof(Instr);
        }
        text_.at(pos / sizeof(Instr)) = setImm( labelReq.instr, offset, labelReq.fmt);
    }
}

void Compiler::addInstr( Instr instr) {
    text_.push_back( instr);
}

void Compiler::addLiteral( const std::string& str) {
    if ( str.size() < 2 ) {
        throw std::runtime_error( "Too short literal");
    }

    for ( uint32_t i = 1; i < str.size() - 1; ++i ) { // skip initial and final quotation marks
        data_.push_back( str[i]);
    }
    data_.push_back( 0);    // Add terminating symbol
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
    
    std::string token;
    if ( *iter == ',' ) {
        char tmp = *iter;
        ++iter;
        return std::string(1, tmp);
    }

    if ( *iter == '\"' ) {
        token.push_back( '\"');
        ++iter;
        while ( ( iter != end ) && ( *iter != '\"' ) ) {
            token.push_back( *iter);
            ++iter;
        }
        if ( iter == end ) {
            throw std::runtime_error( "Failed to parse string, expected closing quotation marks, encountered end of line!");
        }
        token.push_back( '\"');
        ++iter;
    }

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
        std::vector<std::string> parsed;

        auto iter = line.begin();
        auto iter_end = line.end();
        while ( iter != iter_end ) {
            skipSpaces( iter, iter_end);
            if ( iter == iter_end ) {
                break;
            }
            std::string token = getToken( iter, iter_end);
            parsed.push_back( std::move( token));
        }
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
    } else if ( directive == ".text" ) {
        ctx.setInText( true);
    } else
    {
        throw std::runtime_error( "Unexpected directive " + directive);
    }
}

void Compiler::addLabel( const std::string& label) {
    std::string str = std::move(label);
    str.pop_back();             // drop ':'
    size_t label_hash = StrHasher{}( str);
    if ( labels_.contains( label_hash) ) {
        throw std::runtime_error( "Redeclaration of label \'" + str + "\'" );
    }

    LabelInfo info;
    
    if ( isInText ) {
        info.isInText = true;
        info.offset = text_.size() * sizeof( Instr);
    } else {
        info.isInText = false;
        info.offset = data_.size();
    }

    labels_[label_hash] = info;
}

LabelInfo Compiler::getLabel( const std::string& label) {
    auto label_iter = labels_.find( StrHasher{}(label));
    if ( label_iter == labels_.end() ) {
        throw std::runtime_error( "Reference to an undefined label \'" + label + "\'");
    }
    return label_iter->second;
}

void Compiler::addLabelRequest( const LabelRequest request) {
    const size_t pos = text_.size() * sizeof(Instr);
    requests_[pos] = request;
}


uint32_t getRegNum( const std::string& str) {
    uint32_t reg_num = 0;
    if ( 0 ) {}
#define REG( reg_name, pseudo, enum_name, idx)              \
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
        throw std::runtime_error( "Expected a register, encountered end of line");
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

Instr encodeRType( TokenIter& token, const TokenIter& token_end, uint32_t opcode, Compiler& ctx) {
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

    return instr;
}

Instr encodeSType( TokenIter& token, const TokenIter& token_end, uint32_t opcode, Compiler& ctx) {
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

    return instr;
}

Instr setImmBType( Instr instr, int64_t imm) {
    imm &= 0xfff;
    instr |= (imm & 0x1e) << 7;
    instr |= (imm & (1 << 11)) >> (11 - 7);
    instr |= (imm & 0x3e0) << (25 - 5);
    instr |= (imm & (1 << 12)) >> (31 - 12);
    return instr;
}

Instr setImmIType( Instr instr, int64_t imm) {
    instr |= (imm & 0xfff) << 20;
    return instr;
}

Instr setImm( Instr instr, int64_t imm, InstrFmt fmt) {
    Instr res;
    switch ( fmt ) {
        case InstrFmt::BType:
            res = setImmBType( instr, imm);
            break;
        case InstrFmt::IType:
            res = setImmIType( instr, imm);
            break;
        default:
            throw std::runtime_error( "Unexpected fmt for labels: " + std::to_string( fmt));
    };
    return res;
}

void requestLabel( size_t labelHash, Instr instr, InstrFmt fmt, Compiler& ctx) {

    LabelRequest request;
    request.instr = instr;
    request.fmt = fmt;
    request.label = labelHash;
    ctx.addLabelRequest( request);
}

bool isTokenImm( const TokenIter& token) {
    return std::isdigit( token->front());
}

int64_t getImmOrLabel( TokenIter& token, const TokenIter& token_end, Instr instr, InstrFmt fmt, Compiler& ctx) {
    int64_t offset = 0;
    if ( isTokenImm( token) ) {
        offset = getImm( token, token_end);
    } else {
        size_t labelHash = StrHasher{}( *token);
        requestLabel( labelHash, instr, fmt, ctx);
        ++token;
    }
    return offset;
}

Instr encodeBType( TokenIter& token, const TokenIter& token_end, uint32_t opcode, Compiler& ctx) {

    Instr instr = opcode;
    IntReg rs1 = getRegNum( token, token_end);
    setRs1( instr, rs1);
    skipComma( token, token_end);

    IntReg rs2 = getRegNum( token, token_end);
    setRs2( instr, rs2);
    skipComma( token, token_end);

    int64_t offset = getImmOrLabel( token, token_end, instr, InstrFmt::BType, ctx);

    validateTokensEnd( token, token_end);

    instr = setImmBType( instr, offset);

    return instr;
}

Instr encodeIType( TokenIter& token, const TokenIter& token_end, uint32_t opcode, Compiler& ctx) {
    Instr instr = opcode;

    if ( instr == 0x00000073 ) { // ecall
        return instr;
    }

    IntReg rd = getRegNum( token, token_end);
    setRd( instr, rd);
    skipComma( token, token_end);

    IntReg rs1 = getRegNum( token, token_end);
    setRs1( instr, rs1);
    skipComma( token, token_end);

    int64_t imm = getImmOrLabel( token, token_end, instr, InstrFmt::IType, ctx);

    validateTokensEnd( token, token_end);

    instr = setImmIType( instr, imm);

    return instr;
}

Instr encodeUType( TokenIter& token, const TokenIter& token_end, uint32_t opcode, Compiler& ctx) {
    // TODO: Implement
    assert( false);
    return Instr();
}

Instr encodeJType( TokenIter& token, const TokenIter& token_end, uint32_t opcode, Compiler& ctx) {
    // TODO: IMPLEMENT
    assert( false);
    return Instr();
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
        ctx.addLabel( *token);
        ++token;
    }

    if ( token == tokens_end ) {
        return;
    }

    if ( token->front() == '\"' ) {
        if ( ctx.getInText() == true ) {
            throw std::runtime_error( "String literals are supported only in section .data");
        }
        if ( token->back() != '\"' ) {
            throw std::runtime_error( "Expected closing quotation marks at the end of the token " + *token);
        }
        ctx.addLiteral( *token);
        ++token;

        // TDOD: several strings together
    }

    if ( token == tokens_end ) {
        return;
    }

    if ( ctx.getInText() == false ) {
        throw std::runtime_error( "Invalid instruction in section .data \'" + *token + '\'');
    }

    const std::string& token_oper = *token;
    Instr result;

    if ( 0 ) {}
#define OPER( opcode, oper_name, name, type, idx)               \
    else if ( token_oper == name ) {                            \
        ++token;                                                \
        result = encode##type( token, tokens_end, opcode, ctx); \
    }
#include <configs/instructions.hpp>
#undef OPER
    else {
        throw std::runtime_error( "Unexpected instruction " + *token);
    }

    ctx.addInstr( result);
}

void Compiler::writeToFile( const std::string& fileName) {
    
    std::ofstream outFile( fileName, std::ios::binary | std::ios::out);

    Header header;
    auto foundLabel = labels_.find( StrHasher{}("_start"));
    if ( foundLabel != labels_.end() ) {
        header.entryPointOffset = foundLabel->second.offset + sizeof(Header);
    } else {
        header.entryPointOffset = -1;
    }

    header.sectionDataEntryOffset = sizeof(Header) + text_.size() * sizeof(Instr);
    
    outFile.write( reinterpret_cast<const char*>(&header), sizeof(header));
    outFile.write( reinterpret_cast<const char*>(text_.data()), text_.size() * sizeof(Instr));
    outFile.write( reinterpret_cast<const char*>(data_.data()), data_.size());

    outFile.close();
}
