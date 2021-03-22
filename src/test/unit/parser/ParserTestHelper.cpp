#include "ParserTestHelper.h"

#include "compiler/ast/nodes/AllNodes.h"
#include "compiler/ast/visitors/AstPrinter.h"
#include "compiler/parser/Parser.h"

#include <catch2/catch.hpp>
#include <iostream>

void assertAstsAreEqual(SimpleTree *expected, SimpleTree *actual, int level) {
    REQUIRE(actual != nullptr);
    INFO(std::to_string(level) + ": " + to_string(expected->type) + " | " + to_string(actual->type));
    REQUIRE(expected->type == actual->type);
    REQUIRE(expected->children.size() == actual->children.size());
    for (unsigned long i = 0; i < expected->children.size(); i++) {
        assertAstsAreEqual(expected->children[i], actual->children[i], level + 1);
    }
}

SimpleTree *createSimpleFromSpecification(const std::vector<AstNodeSpec> &spec, int &index, int indentation = 0) {
    auto firstLine = spec[index];
    auto node = new SimpleTree();
    node->type = firstLine.type;
    while (index + 1 < spec.size() && spec[index + 1].indent > indentation) {
        index++;
        auto child = createSimpleFromSpecification(spec, index, indentation + 1);
        node->children.push_back(child);
    }
    return node;
}

SimpleTree *createSimpleFromSequence(SequenceNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    for (auto child : node->getChildren()) {
        result->children.push_back(createSimpleFromAst(child));
    }
    return result;
}

SimpleTree *createSimpleFromStatement(StatementNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getChild()));
    return result;
}

SimpleTree *createSimpleFromUnary(UnaryOperationNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getChild()));
    return result;
}

SimpleTree *createSimpleFromBinary(BinaryOperationNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getLeft()));
    result->children.push_back(createSimpleFromAst(node->getRight()));
    return result;
}

SimpleTree *createSimpleFromFunction(FunctionNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    for (auto argument : node->getArguments()) {
        result->children.push_back(createSimpleFromAst(argument));
    }
    if (!node->isExternal()) {
        result->children.push_back(createSimpleFromAst(node->getBody()));
    }
    return result;
}

SimpleTree *createSimpleFromAssignment(AssignmentNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getLeft()));
    result->children.push_back(createSimpleFromAst(node->getRight()));
    return result;
}

SimpleTree *createSimpleFromCall(CallNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    for (auto argument : node->getArguments()) {
        result->children.push_back(createSimpleFromAst(argument));
    }
    return result;
}

SimpleTree *createSimpleFromIf(IfStatementNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getCondition()));
    if (node->getIfBody() != nullptr) {
        result->children.push_back(createSimpleFromAst(node->getIfBody()));
    }
    if (node->getElseBody() != nullptr) {
        result->children.push_back(createSimpleFromAst(node->getElseBody()));
    }
    return result;
}

SimpleTree *createSimpleFromFor(ForStatementNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    if (node->getInit() != nullptr) {
        result->children.push_back(createSimpleFromAst(node->getInit()));
    }
    if (node->getCondition() != nullptr) {
        result->children.push_back(createSimpleFromAst(node->getCondition()));
    }
    if (node->getUpdate() != nullptr) {
        result->children.push_back(createSimpleFromAst(node->getUpdate()));
    }
    if (node->getBody() != nullptr) {
        result->children.push_back(createSimpleFromAst(node->getBody()));
    }
    return result;
}

SimpleTree *createSimpleFromTypeDeclaration(TypeDeclarationNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    for (const auto &member : node->getMembers()) {
        result->children.push_back(createSimpleFromAst(member));
    }
    return result;
}

SimpleTree *createSimpleFromAssert(AssertNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    result->children.push_back(createSimpleFromAst(node->getCondition()));
    return result;
}

SimpleTree *createSimpleFromNode(AstNode *node) {
    auto result = new SimpleTree();
    result->type = node->getAstNodeType();
    return result;
}

SimpleTree *createSimpleFromAst(AstNode *node) {
    if (node == nullptr) {
        return nullptr;
    }
    switch (node->getAstNodeType()) {
    case ast::NodeType::LITERAL:
    case ast::NodeType::VARIABLE_DEFINITION:
    case ast::NodeType::VARIABLE:
    case ast::NodeType::IMPORT:
    case ast::NodeType::TYPE_MEMBER:
    case ast::NodeType::MEMBER_ACCESS:
    case ast::NodeType::COMMENT:
        return createSimpleFromNode(node);
    case ast::NodeType::SEQUENCE:
        return createSimpleFromSequence((SequenceNode *)node);
    case ast::NodeType::STATEMENT:
        return createSimpleFromStatement((StatementNode *)node);
    case ast::NodeType::UNARY_OPERATION:
        return createSimpleFromUnary((UnaryOperationNode *)node);
    case ast::NodeType::BINARY_OPERATION:
        return createSimpleFromBinary((BinaryOperationNode *)node);
    case ast::NodeType::FUNCTION:
        return createSimpleFromFunction((FunctionNode *)node);
    case ast::NodeType::ASSIGNMENT:
        return createSimpleFromAssignment((AssignmentNode *)node);
    case ast::NodeType::CALL:
        return createSimpleFromCall((CallNode *)node);
    case ast::NodeType::IF_STATEMENT:
        return createSimpleFromIf((IfStatementNode *)node);
    case ast::NodeType::FOR_STATEMENT:
        return createSimpleFromFor((ForStatementNode *)node);
    case ast::NodeType::TYPE_DECLARATION:
        return createSimpleFromTypeDeclaration((TypeDeclarationNode *)node);
    case ast::NodeType::ASSERT:
        return createSimpleFromAssert((AssertNode *)node);
    default:
        std::cerr << "Could not create simple helper tree node for " << to_string(node->getAstNodeType()) << std::endl;
        exit(1);
        return nullptr;
    }
}

void assertProgramCreatesAstWithSimpleParser(const std::vector<std::string> &program, std::vector<AstNodeSpec> &spec) {
    int index = 0;
    auto expected = createSimpleFromSpecification(spec, index);
    CodeProvider *codeProvider = new StringCodeProvider(program, true);
    auto context = new llvm::LLVMContext();
    auto prog = new Module("test.ne", *context);
    Logger logger = {};
    auto lexer = Lexer(codeProvider, logger);

    Parser parser(lexer, prog, logger);
    parser.run();

    auto astPrinter = AstPrinter(prog);
    astPrinter.run();

    auto actual = prog->root;

    assertAstsAreEqual(expected, createSimpleFromAst(actual), 0);
}