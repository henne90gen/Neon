#pragma once

#include "../../Module.h"
#include "../Logger.h"
#include "../ast/nodes/AllNodes.h"

class Parser {
  public:
    Parser(Lexer &lexer, Module *module, const Logger &logger) : lexer(lexer), module(module), log(logger) {}

    void run();

  private:
    Lexer &lexer;
    Module *module;
    const Logger &log;
    int currentTokenIdx = 0;

    Token getNextToken();
    [[nodiscard]] bool currentTokenIs(Token::TokenType tokenType) const;
    [[nodiscard]] std::string currentTokenContent() const;

    StatementNode *parseStatement(int level);
    AssertNode *parseAssert(int level);
    ImportNode *parseImport();
    StatementNode *parseReturnStatement(int level);
    ForStatementNode *parseFor(int level);
    IfStatementNode *parseIf(int level);
    AssignmentNode *parseAssignment(int level);
    FunctionNode *parseFunction(int level);
    SequenceNode *parseScope(int level);
    VariableDefinitionNode *parseVariableDefinition(int level);
    CallNode *parseFunctionCall(int level);
    VariableNode *parseVariable(int level);
    AstNode *parseAssignmentLeft(int level);
    LiteralNode *parseLiteral(int level);
    CommentNode *parseComment(int level);
    TypeDeclarationNode *parseTypeDeclaration(int level);
    TypeMemberNode *parseMemberVariable(int level);

    AstNode *parseExpression(int level);
    AstNode *parseEquality(int level);
    AstNode *parseComparison(int level);
    AstNode *parseTerm(int level);
    AstNode *parseFactor(int level);
    AstNode *parseUnary(int level);
    AstNode *parsePrimary(int level);

    static std::string indent(int level);
};
