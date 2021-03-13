#include "IrGenerator.h"

void IrGenerator::visitStatementNode(StatementNode *node) {
    LOG("Enter Statement")

    if (node->getChild() == nullptr) {
        return;
    }

    node->getChild()->accept(this);
    auto value = nodesToValues[node->getChild()];
    if (node->isReturnStatement()) {
        builder.CreateRet(value);
    }
    nodesToValues[node] = value;

    LOG("Exit Statement")
}

bool hasReturnStatement(AstNode *node) {
    if (node == nullptr) {
        return false;
    }
    switch (node->getAstNodeType()) {
    case ast::SEQUENCE: {
        auto children = (dynamic_cast<SequenceNode *>(node))->getChildren();
        if (!children.empty()) {
            return hasReturnStatement(children[children.size() - 1]);
        }
        return false;
    }
    case ast::STATEMENT: {
        return (dynamic_cast<StatementNode *>(node))->isReturnStatement();
    }
    case ast::LITERAL:
    case ast::UNARY_OPERATION:
    case ast::BINARY_OPERATION:
    case ast::FUNCTION:
    case ast::CALL:
    case ast::VARIABLE_DEFINITION:
    case ast::VARIABLE:
    case ast::ASSIGNMENT:
    case ast::IF_STATEMENT:
    default:
        return false;
    }
}

void IrGenerator::visitIfStatementNode(IfStatementNode *node) {
    LOG("Enter IfStatement")

    node->getCondition()->accept(this);
    auto condition = nodesToValues[node->getCondition()];

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", function);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "if_merge");

    builder.CreateCondBr(condition, thenBB, elseBB);

    builder.SetInsertPoint(thenBB);
    if (node->getIfBody() != nullptr) {
        withScope([this, &node]() { node->getIfBody()->accept(this); });
    }
    if (!hasReturnStatement(node->getIfBody())) {
        // create branch instruction to jump to the merge block
        builder.CreateBr(mergeBB);
    }

    function->getBasicBlockList().push_back(elseBB);
    builder.SetInsertPoint(elseBB);

    if (node->getElseBody() != nullptr) {
        withScope([this, &node]() { node->getElseBody()->accept(this); });
    }
    if (!hasReturnStatement(node->getElseBody())) {
        // create branch instruction to jump to the merge block
        builder.CreateBr(mergeBB);
    }

    function->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);

    LOG("Exit IfStatement")
}

void IrGenerator::visitForStatementNode(ForStatementNode *node) {
    LOG("Enter ForStatement");
    pushScope();

    node->getInit()->accept(this);

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *loopHeaderBB = llvm::BasicBlock::Create(context, "loop-header", function);
    llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(context, "loop-body", function);
    llvm::BasicBlock *loopExitBB = llvm::BasicBlock::Create(context, "loop-exit", function);
    builder.CreateBr(loopHeaderBB);
    builder.SetInsertPoint(loopHeaderBB);

    node->getCondition()->accept(this);
    auto condition = nodesToValues[node->getCondition()];

    builder.CreateCondBr(condition, loopBodyBB, loopExitBB);

    builder.SetInsertPoint(loopBodyBB);

    if (node->getBody() != nullptr) {
        node->getBody()->accept(this);
    }

    node->getUpdate()->accept(this);

    popScope();

    builder.CreateBr(loopHeaderBB);

    builder.SetInsertPoint(loopExitBB);

    LOG("Exit ForStatement");
}

void IrGenerator::visitAssertNode(AssertNode *node) {
    LOG("Enter Assert");

    node->getCondition()->accept(this);
    auto condition = nodesToValues[node->getCondition()];

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", function);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "if_merge");

    builder.CreateCondBr(condition, thenBB, elseBB);

    builder.SetInsertPoint(thenBB);
    // create branch instruction to jump to the merge block
    builder.CreateBr(mergeBB);

    function->getBasicBlockList().push_back(elseBB);
    builder.SetInsertPoint(elseBB);

    auto exitCode = llvm::ConstantInt::get(llvm::IntegerType::getInt32Ty(context), 1);
    std::vector<llvm::Value *> args = {exitCode};
    createStdLibCall("exit", args);

    // create branch instruction to jump to the merge block
    builder.CreateBr(mergeBB);

    function->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);

    LOG("Exit Assert");
}
