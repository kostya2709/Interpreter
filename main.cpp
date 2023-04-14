#include <iostream>
#include <runtime/machine.hpp>
#include <string>
#include <parse/opcode.hpp>
#include <compiler/compiler.hpp>

int main() {

  Compiler comp;
  comp.run( "../examples/Fibonacci/Fibonacci");
  // Interpreter interp;
  // interp.run("../examples/Fibonacci/Fibonacci.bc");
  return 0;
}