#include <cstdlib>
#include <iostream>
#include <memory>

#include "ast_structures.hpp"
#include "code_generator.hpp"
#include "driver.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

// #include "lld/Common/LLVM.h"
// #include "lld/Common/Driver.h"

extern std::shared_ptr<block> program_block;

int main(int argc, char* argv[]) {
    using namespace llvm;

    // hide all the preset command line options that llvm takes
    llvm::cl::OptionCategory CompilerCategory(
        "Compiler Options", "Options for controlling the compilation process.");
    llvm::cl::HideUnrelatedOptions(CompilerCategory);

    cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<input file>"),
                                       cl::value_desc("filename"),
                                       cl::init("-"));

    // output file name of the executable and the object file
    cl::opt<std::string> OutputFilename(
        "o", cl::desc("Specify output filename"), cl::value_desc("filename"));

    // parse the command line options
    llvm::cl::ParseCommandLineOptions(
        argc, argv,
        "Barium Compiler\nUses STDIN if <input file> is not specified\n");

    // initiate the driver
    driver drv;
    if (InputFilename.empty()) {
        // parse stdin
        drv.parse("-");
    } else {
        drv.parse(InputFilename);
    }

    std::string output_file =
        OutputFilename.empty() ? "output" : OutputFilename.getValue();

    codegen_context ctx;

    if (program_block == nullptr) {
        std::cerr << "error converting ast to ir!\nprogram_block was NULL!" << '\n';
        exit(0);
    }

    // generate the LLVM IR
    ctx.generate_code(program_block);

    // run the code
    ctx.run_code();

    return 0;
}
