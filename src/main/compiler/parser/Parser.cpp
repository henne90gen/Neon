#include "Parser.h"

#include <iostream>

Token Parser::getNextToken() {
    Token token = lexer.getToken();
    tokens.push_back(token);
    return token;
}

bool Parser::currentTokenIs(Token::TokenType tokenType) const {
    return currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == tokenType;
}

std::string Parser::currentTokenContent() const { return tokens[currentTokenIdx].content; }

std::string Parser::indent(int level) {
    std::string result;
    for (int i = 0; i < level; i++) {
        result += " ";
    }
    return result;
}

ImportNode *Parser::parseImport() {
    if (!currentTokenIs(Token::IMPORT)) {
        return nullptr;
    }
    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    if (!currentTokenIs(Token::STRING)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    std::string fileName = currentTokenContent();
    fileName = fileName.substr(1, fileName.size() - 2);

    currentTokenIdx++;
    return tree.createImport(fileName);
}

VariableNode *Parser::parseVariable(int level) {
    if (!currentTokenIs(Token::IDENTIFIER)) {
        return nullptr;
    }

    auto beforeTokenIdx = currentTokenIdx;
    std::string name = currentTokenContent();

    currentTokenIdx++;

    AstNode *expression = nullptr;
    if (currentTokenIs(Token::LEFT_BRACKET)) {
        currentTokenIdx++;

        expression = parseExpression(level + 1);
        if (expression == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        if (!currentTokenIs(Token::RIGHT_BRACKET)) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        currentTokenIdx++;
    }

    return tree.createVariable(name, expression);
}

VariableDefinitionNode *Parser::parseVariableDefinition(int level) {
    log.debug(indent(level) + "parsing variable definition node");

    auto beforeTokenIdx = currentTokenIdx;
    if (currentTokenIs(Token::SIMPLE_DATA_TYPE)) {
        auto dataType = ast::DataType(from_string(currentTokenContent()));

        currentTokenIdx++;

        if (currentTokenIs(Token::IDENTIFIER)) {
            log.debug(indent(level) + "parsed variable definition with simple data type");
            std::string variableName = currentTokenContent();
            currentTokenIdx++;
            return tree.createVariableDefinition(variableName, dataType, 0);
        }
        if (currentTokenIs(Token::LEFT_BRACKET)) {
            currentTokenIdx++;

            auto *literal = parseLiteral(level + 1);
            if (literal == nullptr || literal->type != LiteralType::INTEGER) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            if (!currentTokenIs(Token::RIGHT_BRACKET)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            currentTokenIdx++;

            if (!currentTokenIs(Token::IDENTIFIER)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            std::string variableName = currentTokenContent();
            currentTokenIdx++;
            return tree.createVariableDefinition(variableName, dataType, literal->i);
        }
    } else if (currentTokenIs(Token::IDENTIFIER)) {
        auto dataType = ast::DataType(currentTokenContent());

        currentTokenIdx++;

        if (!currentTokenIs(Token::IDENTIFIER)) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        log.debug(indent(level) + "parsed variable definition with simple data type");
        std::string variableName = currentTokenContent();
        currentTokenIdx++;
        return tree.createVariableDefinition(variableName, dataType, 0);
    }

    log.debug(indent(level) + "failed to parse variable definition");

    return nullptr;
}

SequenceNode *Parser::parseScope(int level) {
    if (!currentTokenIs(Token::LEFT_CURLY_BRACE)) {
        return nullptr;
    }
    log.debug(indent(level) + "parsing scope");

    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    std::vector<AstNode *> children = {};
    while (true) {
        auto *statement = parseStatement(level + 1);
        if (statement == nullptr) {
            break;
        }

        children.push_back(AST_NODE(statement));
    }

    if (!currentTokenIs(Token::RIGHT_CURLY_BRACE)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    return tree.createSequence(children);
}

AstNode *Parser::parseAssignmentLeft(int level) {
    auto *variableDefinitionNode = parseVariableDefinition(level + 1);
    if (variableDefinitionNode != nullptr) {
        return AST_NODE(variableDefinitionNode);
    }

    auto *memberAccess = parseMemberAccess(level + 1);
    if (memberAccess != nullptr) {
        return AST_NODE(memberAccess);
    }

    auto *variableNode = parseVariable(level + 1);
    if (variableNode != nullptr) {
        return AST_NODE(variableNode);
    }

    return nullptr;
}

AssignmentNode *Parser::parseAssignment(int level) {
    log.debug(indent(level) + "parsing assignment statement");
    auto beforeTokenIdx = currentTokenIdx;
    auto *left = parseAssignmentLeft(level + 1);
    if (left == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    log.debug(indent(level) + "parsed assignment left");

    if (!currentTokenIs(Token::SINGLE_EQUALS)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    currentTokenIdx++;

    auto *right = parseExpression(level + 1);
    if (right == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    log.debug(indent(level) + "parsed assignment right");

    return tree.createAssignment(left, right);
}

AssertNode *Parser::parseAssert(int level) {
    if (!currentTokenIs(Token::ASSERT)) {
        return nullptr;
    }

    log.debug(indent(level) + "parsing assert statement");

    currentTokenIdx++;

    auto *expression = parseExpression(level + 1);
    if (expression == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }

    return tree.createAssert(expression);
}

CommentNode *Parser::parseComment(int /*level*/) {
    if (!currentTokenIs(Token::COMMENT)) {
        return nullptr;
    }

    log.debug("parsed comment node");

    auto *result = tree.createComment(currentTokenContent());
    currentTokenIdx++;
    return result;
}

StatementNode *Parser::parseStatement(int level) {
    log.debug(indent(level) + "parsing statement node");

    while (currentTokenIs(Token::NEW_LINE)) {
        currentTokenIdx++;
    }

    auto *commentNode = parseComment(level + 1);
    if (commentNode != nullptr) {
        return tree.createStatement(AST_NODE(commentNode), false);
    }

    auto *importNode = parseImport();
    if (importNode != nullptr) {
        return tree.createStatement(AST_NODE(importNode), false);
    }

    auto *typeNode = parseTypeDeclaration(level + 1);
    if (typeNode != nullptr) {
        return tree.createStatement(AST_NODE(typeNode), false);
    }

    auto *assertNode = parseAssert(level + 1);
    if (assertNode != nullptr) {
        return tree.createStatement(AST_NODE(assertNode), false);
    }

    auto *callNode = parseFunctionCall(level + 1);
    if (callNode != nullptr) {
        return tree.createStatement(AST_NODE(callNode), false);
    }

    auto *functionNode = parseFunction(level + 1);
    if (functionNode != nullptr) {
        return tree.createStatement(AST_NODE(functionNode), false);
    }

    auto *ifNode = parseIf(level + 1);
    if (ifNode != nullptr) {
        return tree.createStatement(AST_NODE(ifNode), false);
    }

    auto *forNode = parseFor(level + 1);
    if (forNode != nullptr) {
        return tree.createStatement(AST_NODE(forNode), false);
    }

    auto *assignmentNode = parseAssignment(level + 1);
    if (assignmentNode != nullptr) {
        return tree.createStatement(AST_NODE(assignmentNode), false);
    }

    auto *variableDefinition = parseVariableDefinition(level + 1);
    if (variableDefinition != nullptr) {
        return tree.createStatement(AST_NODE(variableDefinition), false);
    }

    auto *returnStatement = parseReturnStatement(level + 1);
    if (returnStatement != nullptr) {
        return returnStatement;
    }

    return nullptr;
}

void Parser::run(Module *module) {
    while (true) {
        const Token token = getNextToken();
        if (token.type == Token::INVALID) {
            break;
        }
    }

    std::vector<AstNode *> children = {};
    bool error = false;

    while (currentTokenIdx < tokens.size()) {
        auto token = tokens[currentTokenIdx];
        if (token.type == Token::INVALID) {
            error = currentTokenIdx != tokens.size() - 1;
            break;
        }

        if (token.type == Token::NEW_LINE) {
            currentTokenIdx++;
            continue;
        }

        auto *statementNode = parseStatement(0);
        if (statementNode != nullptr) {
            children.push_back(AST_NODE(statementNode));
            continue;
        }

        log.error("Unexpected token: " + to_string(token.type) + ": " + token.content);
        error = true;
        break;
    }

    if (error) {
        return;
    }

    auto *root = tree.root();
    root->type = ast::NodeType::SEQUENCE;
    for (auto *child : children) {
        root->sequence.children.push_back(child);
    }
    tree.completed();

    module->ast = tree;
    module->tokens = tokens;
}
