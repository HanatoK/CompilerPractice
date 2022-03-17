#include "Compiler.h"

#include <chrono>

void CodeGenerator::process(std::shared_ptr<const ICodeImplBase> iCode,
    std::shared_ptr<SymbolTableStackImplBase>
        symbol_table_stack) {
  const auto start_time = std::chrono::high_resolution_clock::now();
  // do something
  const auto end_time = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_time = end_time - start_time;
  const int instructionCount = 0;
  summary(instructionCount, elapsed_time.count());
}
