#include "IrGenerator.h"

void IrGenerator::visitStatementNode(StatementNode *node) {
    log.debug("Enter Statement");

    if (node->child == nullptr) {
        return;
    }

    visitNode(node->child);
    auto *value = nodesToValues[node->child];
    if (node->returnStatement) {
        builder.CreateRet(value);
    }
    nodesToValues[AST_NODE(node)] = value;

    log.debug("Exit Statement");
}

bool hasReturnStatement(AstNode *node) {
    if (node == nullptr) {
        return false;
    }
    switch (node->type) {
    case ast::NodeType::SEQUENCE: {
        auto children = node->sequence.children;
        if (!children.empty()) {
            return hasReturnStatement(children[children.size() - 1]);
        }
        return false;
    }
    case ast::NodeType::STATEMENT: {
        return node->statement.returnStatement;
    }
    case ast::NodeType::LITERAL:
    case ast::NodeType::UNARY_OPERATION:
    case ast::NodeType::BINARY_OPERATION:
    case ast::NodeType::FUNCTION:
    case ast::NodeType::CALL:
    case ast::NodeType::VARIABLE_DEFINITION:
    case ast::NodeType::VARIABLE:
    case ast::NodeType::ASSIGNMENT:
    case ast::NodeType::IF_STATEMENT:
    default:
        return false;
    }
}

void IrGenerator::visitIfStatementNode(IfStatementNode *node) {
    log.debug("Enter IfStatement");

    visitNode(node->condition);
    auto *condition = nodesToValues[node->condition];

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", function);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "if_merge");

    builder.CreateCondBr(condition, thenBB, elseBB);

    builder.SetInsertPoint(thenBB);
    if (node->ifBody != nullptr) {
        withScope([this, &node]() { visitNode(node->ifBody); });
    }
    if (!hasReturnStatement(node->ifBody)) {
        // create branch instruction to jump to the merge block
        builder.CreateBr(mergeBB);
    }

    function->getBasicBlockList().push_back(elseBB);
    builder.SetInsertPoint(elseBB);

    if (node->elseBody != nullptr) {
        withScope([this, &node]() { visitNode(node->elseBody); });
    }
    if (!hasReturnStatement(node->elseBody)) {
        // create branch instruction to jump to the merge block
        builder.CreateBr(mergeBB);
    }

    function->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);

    log.debug("Exit IfStatement");
}

void IrGenerator::visitForStatementNode(ForStatementNode *node) {
    log.debug("Enter ForStatement");
    pushScope();

    visitNode(node->init);

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *loopHeaderBB = llvm::BasicBlock::Create(context, "loop-header", function);
    llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(context, "loop-body", function);
    llvm::BasicBlock *loopExitBB = llvm::BasicBlock::Create(context, "loop-exit", function);
    builder.CreateBr(loopHeaderBB);
    builder.SetInsertPoint(loopHeaderBB);

    visitNode(node->condition);
    auto *condition = nodesToValues[node->condition];

    builder.CreateCondBr(condition, loopBodyBB, loopExitBB);

    builder.SetInsertPoint(loopBodyBB);

    if (node->body != nullptr) {
        visitNode(node->body);
    }

    visitNode(node->update);

    popScope();

    builder.CreateBr(loopHeaderBB);

    builder.SetInsertPoint(loopExitBB);

    log.debug("Exit ForStatement");
}

std::string IrGenerator::getTypeFormatSpecifier(AstNode* node) {
    auto type = typeResolver.getTypeOf(module, node);
    if (type == ast::DataType(ast::SimpleDataType::INTEGER)) {
        return "ld";
    }
    if (type == ast::DataType(ast::SimpleDataType::FLOAT)) {
        return "f";
    }
    return "%d";
}

void IrGenerator::visitAssertNode(AssertNode *node) {
    log.debug("Enter Assert");

    visitNode(node->condition);
    auto *condition = nodesToValues[node->condition];

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

    if (node->condition->type == ast::NodeType::BINARY_OPERATION) {
        auto binaryOperation = &node->condition->binary_operation;
        const std::string leftTypeSpecifier = getTypeFormatSpecifier(binaryOperation->left);
        const std::string rightTypeSpecifier = getTypeFormatSpecifier(binaryOperation->right);
        const std::string format = "> assert %s\nE assert %" + leftTypeSpecifier +
                                   binaryOperation->operation_string() + "%" + rightTypeSpecifier + "\n";
        auto *const formatStr = builder.CreateGlobalStringPtr(format);
        auto *const conditionStr = builder.CreateGlobalStringPtr(to_string(AST_NODE(binaryOperation)));
        auto *const left = nodesToValues[binaryOperation->left];
        auto *const right = nodesToValues[binaryOperation->right];
        std::vector<llvm::Value *> args = {
              formatStr,
              conditionStr,
              left,
              right,
        };
        createStdLibCall("printf", args);
    } else {
        const std::string format = "E assert %s\n";
        auto *const formatStr = builder.CreateGlobalStringPtr(format);
        auto *const conditionStr = builder.CreateGlobalStringPtr(to_string(node->condition));
        std::vector<llvm::Value *> args = {
              formatStr,
              conditionStr,
        };
        createStdLibCall("printf", args);
    }

    auto *exitCode = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 1);
    std::vector<llvm::Value *> args = {exitCode};
    createStdLibCall("exit", args);

    // create branch instruction to jump to the merge block
    builder.CreateBr(mergeBB);

    function->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);

    log.debug("Exit Assert");
}
