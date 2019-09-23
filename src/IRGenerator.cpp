#include "AST.h"

#include <iostream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

llvm::Value *LogError(const char *Str) {
    std::cout << Str << std::endl;
    return nullptr;
}

llvm::Value *SequenceNode::generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) {
    const char *functionName = "MyFunction";
    llvm::Function *function = module.getFunction(functionName);
    if (function == nullptr) {
        llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getFloatTy(context), false);

        function = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, functionName, module);
    }

    if (!function) {
        return LogError("Could not create function");
    }

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(BB);

    llvm::Value *returnValue = nullptr;
    for (auto &child : children) {
        returnValue = child->generateIR(context, builder, module);
    }
    builder.CreateRet(returnValue);

    llvm::verifyFunction(*function);
    return function;
}

llvm::Value *IntegerNode::generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) {
    return llvm::ConstantInt::get(context, llvm::APInt(32, value));
}
llvm::Value *FloatNode::generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) {
    return llvm::ConstantFP::get(context, llvm::APFloat(value));
}
llvm::Value *BoolNode::generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) {
    return llvm::ConstantInt::get(context, llvm::APInt(1, value));
}

llvm::Value *UnaryOperationNode::generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder,
                                            llvm::Module &module) {
    auto c = child->generateIR(context, builder, module);
    if (c == nullptr) {
        return LogError("Generating the child failed.");
    }
    switch (type) {
    case NOT:
        return builder.CreateNot(c, "nottmp");
    default:
        return LogError("Invalid unary operation.");
    }
}

llvm::Value *BinaryOperationNode::generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder,
                                             llvm::Module &module) {
    auto l = left->generateIR(context, builder, module);
    auto r = right->generateIR(context, builder, module);
    if (l == nullptr || r == nullptr) {
        return LogError("Generating left or right side failed.");
    }

    switch (type) {
    case ADDITION:
        return builder.CreateFAdd(l, r, "addtmp");
    case MULTIPLICATION:
        return builder.CreateFMul(l, r, "multmp");
    case SUBTRACTION:
        return builder.CreateFSub(l, r, "subtmp");
    case DIVISION:
        return builder.CreateFDiv(l, r, "divtmp");
    default:
        return LogError("Invalid binary operation.");
    }
}

void generateIR(AstNode *root) {
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);

    llvm::Module TheModule = llvm::Module("MyModuleName", context);

    root->generateIR(context, builder, TheModule);

    TheModule.print(llvm::errs(), nullptr);
}
