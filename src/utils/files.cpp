#include "files.hpp"
#include <iostream>
#include <fstream>

size_t getFileSize( const std::string& filename) {
    size_t size = 0;
    std::ifstream file( filename, std::ios::in);
    if ( !file.is_open() ) {
        throw std::invalid_argument("No such file.");
    }
    file.seekg( 0, std::ios::end);
    size = file.tellg();
    file.close();
    return size;  
}