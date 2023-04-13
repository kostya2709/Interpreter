cmake_minimum_required(VERSION 3.16)

project(VirtualMachine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -Wall)

set(CURRENT_EXECUTABLE "${PROJECT_NAME}")

add_executable(
    VirtualMachine
    main.cpp
    ${VirtualMachine_SOURCE_DIR}/src/runtime/machine.cpp
    ${VirtualMachine_SOURCE_DIR}/src/parse/opcode.cpp
    ${VirtualMachine_SOURCE_DIR}/src/file/header.cpp
    ${VirtualMachine_SOURCE_DIR}/src/mappers/int_mappers.cpp
    ${VirtualMachine_SOURCE_DIR}/src/compiler/compiler.cpp
    ${VirtualMachine_SOURCE_DIR}/src/utils/files.cpp
)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

target_include_directories(
    VirtualMachine PUBLIC
    ${VirtualMachine_SOURCE_DIR}/src
)