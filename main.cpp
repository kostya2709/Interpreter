#include <iostream>
#include <runtime/machine.hpp>
#include <string>
#include <parse/opcode.hpp>
#include <compiler/compiler.hpp>

int main() {


  Compiler comp;
  comp.run( "../asm.s");
  // Interpreter interp;
  // interp.run("../input.txt");

  return 0;
}