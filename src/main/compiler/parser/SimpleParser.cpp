#include "SimpleParser.h"

#include <iostream>

Token SimpleParser::getNextToken() {
    Token token = lexer.getToken();
    module->tokens.push_back(token);
    return token;
}

std::string indent(int level) {
    std::string result;
    for (int i = 0; i < level; i++) {
        result += " ";
    }
    return result;
}

ImportNode *parseImport(const std::vector<Token> &tokens, int &currentTokenIdx) {
    if (tokens[currentTokenIdx].type != Token::IMPORT) {
        return nullptr;
    }

    if (currentTokenIdx + 1 >= tokens.size() || tokens[currentTokenIdx + 1].type != Token::STRING) {
        return nullptr;
    }

    std::string fileName = tokens[currentTokenIdx + 1].content;
    fileName = fileName.substr(1, fileName.size() - 2);

    auto importNode = new ImportNode();
    importNode->setFileName(fileName);

    currentTokenIdx += 2;
    return importNode;
}

LiteralNode *SimpleParser::parseLiteral(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (verbose) {
        std::cout << indent(level) << "parsing literal node" << std::endl;
    }

    if (tokens[currentTokenIdx].type == Token::INTEGER) {
        if (verbose) {
            std::cout << indent(level) << "parsed integer node" << std::endl;
        }
        int value = std::stoi(tokens[currentTokenIdx].content);
        currentTokenIdx++;
        return new IntegerNode(value);
    }

    if (tokens[currentTokenIdx].type == Token::FLOAT) {
        if (verbose) {
            std::cout << indent(level) << "parsed float node" << std::endl;
        }
        float value = std::stof(tokens[currentTokenIdx].content);
        currentTokenIdx++;
        return new FloatNode(value);
    }

    if (tokens[currentTokenIdx].type == Token::BOOLEAN) {
        if (verbose) {
            std::cout << indent(level) << "parsed boolean node" << std::endl;
        }
        bool value = tokens[currentTokenIdx].content == "true";
        currentTokenIdx++;
        return new BoolNode(value);
    }

    if (tokens[currentTokenIdx].type == Token::STRING) {
        if (verbose) {
            std::cout << indent(level) << "parsed string node" << std::endl;
        }
        std::string value = tokens[currentTokenIdx].content;
        value = value.substr(1, value.size() - 2);
        currentTokenIdx++;
        return new StringNode(value);
    }

    if (verbose) {
        std::cout << indent(level) << "failed to parse literal node" << std::endl;
    }
    return nullptr;
}

VariableNode *SimpleParser::parseVariable(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER)) {
        return nullptr;
    }
    auto beforeTokenIdx = currentTokenIdx;
    std::string name = tokens[currentTokenIdx].content;
    currentTokenIdx++;

    AstNode *expression = nullptr;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_BRACKET) {
        currentTokenIdx++;

        expression = parseExpression(tokens, currentTokenIdx, level + 1);
        if (expression == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_BRACKET)) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        currentTokenIdx++;
    }

    auto result = new VariableNode(name);
    result->setArrayIndex(expression);
    return result;
}

AstNode *SimpleParser::parseBinaryLeft(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (tokens[currentTokenIdx].type == Token::LEFT_PARAN) {
        currentTokenIdx++;
        auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
        if (expression != nullptr &&
            (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_PARAN)) {
            currentTokenIdx++;
            return expression;
        }
        currentTokenIdx--;
    }

    auto literal = parseLiteral(tokens, currentTokenIdx, level + 1);
    if (literal != nullptr) {
        return literal;
    }

    auto functionCall = parseFunctionCall(tokens, currentTokenIdx, level + 1);
    if (functionCall != nullptr) {
        return functionCall;
    }

    auto variable = parseVariable(tokens, currentTokenIdx, level + 1);
    if (variable != nullptr) {
        return variable;
    }

    return nullptr;
}

AstNode *SimpleParser::parseBinaryRight(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (tokens[currentTokenIdx].type == Token::LEFT_PARAN) {
        currentTokenIdx++;
        auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
        if (expression != nullptr &&
            (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_PARAN)) {
            currentTokenIdx++;
            return expression;
        }
        currentTokenIdx--;
    }

    auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
    if (expression != nullptr) {
        return expression;
    }

    return nullptr;
}

BinaryOperationNode *SimpleParser::parseBinaryOperation(const std::vector<Token> &tokens, int &currentTokenIdx,
                                                        int level) const {
    if (verbose) {
        std::cout << indent(level) << "parsing binary operation node" << std::endl;
    }

    int tokenIdxBeforeLeft = currentTokenIdx;
    auto left = parseBinaryLeft(tokens, currentTokenIdx, level + 1);
    if (left == nullptr) {
        return nullptr;
    }

    if (verbose) {
        std::cout << indent(level) << "parsed binary operation left" << std::endl;
    }

    if (currentTokenIdx >= tokens.size()) {
        currentTokenIdx = tokenIdxBeforeLeft;
        if (verbose) {
            std::cout << indent(level) << "failed to parse binary operation" << std::endl;
        }
        return nullptr;
    }

    auto operationType = ast::BinaryOperationType::ADDITION;
    if (tokens[currentTokenIdx].type == Token::PLUS) {
        operationType = ast::BinaryOperationType::ADDITION;
    } else if (tokens[currentTokenIdx].type == Token::MINUS) {
        operationType = ast::BinaryOperationType::SUBTRACTION;
    } else if (tokens[currentTokenIdx].type == Token::STAR) {
        operationType = ast::BinaryOperationType::MULTIPLICATION;
    } else if (tokens[currentTokenIdx].type == Token::DIV) {
        operationType = ast::BinaryOperationType::DIVISION;
    } else if (tokens[currentTokenIdx].type == Token::LESS_THAN) {
        operationType = ast::BinaryOperationType::LESS_THAN;
    } else if (tokens[currentTokenIdx].type == Token::LESS_EQUALS) {
        operationType = ast::BinaryOperationType::LESS_EQUALS;
    } else if (tokens[currentTokenIdx].type == Token::GREATER_THAN) {
        operationType = ast::BinaryOperationType::GREATER_THAN;
    } else if (tokens[currentTokenIdx].type == Token::GREATER_EQUALS) {
        operationType = ast::BinaryOperationType::GREATER_EQUALS;
    } else if (tokens[currentTokenIdx].type == Token::DOUBLE_EQUALS) {
        operationType = ast::BinaryOperationType::EQUALS;
    } else if (tokens[currentTokenIdx].type == Token::NOT_EQUALS) {
        operationType = ast::BinaryOperationType::NOT_EQUALS;
    } else {
        currentTokenIdx = tokenIdxBeforeLeft;
        if (verbose) {
            std::cout << indent(level) << "failed to parse binary operation" << std::endl;
        }
        return nullptr;
    }

    currentTokenIdx++;

    auto right = parseBinaryRight(tokens, currentTokenIdx, level + 1);
    if (right == nullptr) {
        if (verbose) {
            std::cout << indent(level) << "failed to parse right side of binary operation" << std::endl;
        }
        currentTokenIdx--;
        return nullptr;
    }

    if (verbose) {
        std::cout << indent(level) << "parsed binary operation right" << std::endl;
    }

    auto binaryOperation = new BinaryOperationNode(operationType);
    binaryOperation->setLeft(left);
    binaryOperation->setRight(right);
    return binaryOperation;
}

AstNode *SimpleParser::parseExpressionInsideParens(const std::vector<Token> &tokens, int &currentTokenIdx,
                                                   int level) const {
    auto binaryOperation = parseBinaryOperation(tokens, currentTokenIdx, level + 1);
    if (binaryOperation != nullptr) {
        return binaryOperation;
    }

    auto literal = parseLiteral(tokens, currentTokenIdx, level + 1);
    if (literal != nullptr) {
        return literal;
    }

    auto functionCall = parseFunctionCall(tokens, currentTokenIdx, level + 1);
    if (functionCall != nullptr) {
        return functionCall;
    }

    auto variable = parseVariable(tokens, currentTokenIdx, level + 1);
    if (variable != nullptr) {
        return variable;
    }

    return nullptr;
}

AstNode *SimpleParser::parseExpression(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (verbose) {
        std::cout << indent(level) << "parsing expression node" << std::endl;
    }

    // TODO why are we trying to parse a binary operation before checking for parenthesis
    auto binaryOperation = parseBinaryOperation(tokens, currentTokenIdx, level + 1);
    if (binaryOperation != nullptr) {
        return binaryOperation;
    }

    bool parens = false;
    if (tokens[currentTokenIdx].type == Token::LEFT_PARAN) {
        currentTokenIdx++;
        parens = true;
    }

    auto result = parseExpressionInsideParens(tokens, currentTokenIdx, level);
    if (result != nullptr) {
        return result;
    }

    if (parens) {
        if (tokens[currentTokenIdx].type == Token::RIGHT_PARAN) {
            currentTokenIdx++;
        } else {
            currentTokenIdx--;
        }
    }

    return nullptr;
}

CallNode *SimpleParser::parseFunctionCall(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (tokens[currentTokenIdx].type != Token::IDENTIFIER) {
        return nullptr;
    }

    if (currentTokenIdx + 1 >= tokens.size() || tokens[currentTokenIdx + 1].type != Token::LEFT_PARAN) {
        return nullptr;
    }

    if (verbose) {
        std::cout << indent(level) << "parsing call node" << std::endl;
    }

    std::vector<AstNode *> params = {};
    // hello ( param1 , param2 )
    //   i  i+1  i+2 i+3  i+4 i+5
    int parameterTokenIdx = currentTokenIdx + 2;
    while (parameterTokenIdx < tokens.size() && tokens[parameterTokenIdx].type != Token::RIGHT_PARAN) {
        auto expression = parseExpression(tokens, parameterTokenIdx, level + 1);
        if (expression == nullptr) {
            return nullptr;
        }
        params.push_back(expression);
    }

    if (tokens[parameterTokenIdx].type != Token::RIGHT_PARAN) {
        return nullptr;
    }

    auto callNode = new CallNode(tokens[currentTokenIdx].content);
    for (const auto param : params) {
        callNode->getArguments().push_back(param);
    }

    currentTokenIdx = parameterTokenIdx + 1;
    return callNode;
}

VariableDefinitionNode *SimpleParser::parseVariableDefinition(const std::vector<Token> &tokens, int &currentTokenIdx,
                                                              int level) const {
    if (verbose) {
        std::cout << indent(level) << "parsing variable definition node" << std::endl;
    }

    auto beforeTokenIdx = currentTokenIdx;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SIMPLE_DATA_TYPE) {
        auto dataType = ast::DataType(from_string(tokens[currentTokenIdx].content));

        currentTokenIdx++;

        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER) {
            std::string variableName = tokens[currentTokenIdx].content;
            auto variableDefinitionNode = new VariableDefinitionNode(variableName, dataType);
            if (verbose) {
                std::cout << indent(level) << "parsed variable definition with simple data type" << std::endl;
            }
            currentTokenIdx++;
            return variableDefinitionNode;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_BRACKET) {
            currentTokenIdx++;

            auto literal = parseLiteral(tokens, currentTokenIdx, level + 1);
            if (literal == nullptr || literal->getLiteralType() != LiteralNode::INTEGER) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }
            auto arraySizeLiteral = reinterpret_cast<IntegerNode *>(literal);

            if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_BRACKET)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            currentTokenIdx++;

            if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            std::string variableName = tokens[currentTokenIdx].content;
            auto arraySize = arraySizeLiteral->getValue();
            auto variableDefinitionNode = new VariableDefinitionNode(variableName, dataType, arraySize);

            currentTokenIdx++;
            return variableDefinitionNode;
        }
    }

    // TODO parse array definitions as well
    // TODO parse custom types as well

    if (verbose) {
        std::cout << indent(level) << "failed to parse variable definition" << std::endl;
    }

    return nullptr;
}

SequenceNode *SimpleParser::parseScope(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_CURLY_BRACE)) {
        return nullptr;
    }
    if (verbose) {
        std::cout << indent(level) << "parsing scope" << std::endl;
    }

    int beforeTokenIdx = currentTokenIdx;

    currentTokenIdx++;

    std::vector<AstNode *> children = {};
    while (true) {
        auto statement = parseStatement(tokens, currentTokenIdx, level + 1);
        if (statement == nullptr) {
            break;
        }

        children.push_back(statement);
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_CURLY_BRACE)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto body = new SequenceNode();
    for (auto child : children) {
        body->getChildren().push_back(child);
    }
    return body;
}

FunctionNode *SimpleParser::parseFunction(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    bool isExternFunc = false;
    if (tokens[currentTokenIdx].type == Token::EXTERN) {
        isExternFunc = true;
        currentTokenIdx++;
    }

    if (!(tokens[currentTokenIdx].type == Token::FUN &&
          (currentTokenIdx + 1 < tokens.size() && tokens[currentTokenIdx + 1].type == Token::IDENTIFIER))) {
        if (isExternFunc) {
            currentTokenIdx--;
        }
        return nullptr;
    }

    if (verbose) {
        std::cout << indent(level) << "parsing function node" << std::endl;
    }

    std::vector<VariableDefinitionNode *> params = {};
    int paramsIdx = 2;
    do {
        paramsIdx++;
        int idx = currentTokenIdx + paramsIdx;
        auto variableDefinitionNode = parseVariableDefinition(tokens, idx, level + 1);
        if (variableDefinitionNode == nullptr) {
            break;
        }
        params.push_back(variableDefinitionNode);
        paramsIdx = idx - currentTokenIdx;
    } while (currentTokenIdx + paramsIdx < tokens.size() && tokens[currentTokenIdx + paramsIdx].type == Token::COMMA);

    if (currentTokenIdx + paramsIdx < tokens.size() && tokens[currentTokenIdx + paramsIdx].type != Token::RIGHT_PARAN) {
        if (isExternFunc) {
            currentTokenIdx -= 1;
        }
        return nullptr;
    }

    std::string functionName = tokens[currentTokenIdx + 1].content;
    currentTokenIdx += paramsIdx + 1;
    ast::DataType returnType = ast::DataType();
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SIMPLE_DATA_TYPE) {
        returnType = ast::DataType(from_string(tokens[currentTokenIdx].content));
        currentTokenIdx += 1;
    }

    auto body = parseScope(tokens, currentTokenIdx, level + 1);

    auto functionNode = new FunctionNode(functionName, returnType);
    functionNode->setBody(body);
    for (auto node : params) {
        functionNode->getArguments().push_back(node);
    }
    return functionNode;
}

AstNode *SimpleParser::parseAssignmentLeft(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto variableDefinitionNode = parseVariableDefinition(tokens, currentTokenIdx, level + 1);
    if (variableDefinitionNode != nullptr) {
        return variableDefinitionNode;
    }

    auto variableNode = parseVariable(tokens, currentTokenIdx, level + 1);
    if (variableNode != nullptr) {
        return variableNode;
    }

    return nullptr;
}

AssignmentNode *SimpleParser::parseAssignment(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (verbose) {
        std::cout << indent(level) << "parsing assignment statement" << std::endl;
    }
    auto beforeTokenIdx = currentTokenIdx;
    auto left = parseAssignmentLeft(tokens, currentTokenIdx, level + 1);
    if (left == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    if (verbose) {
        std::cout << indent(level) << "parsed assignment left" << std::endl;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SINGLE_EQUALS)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    currentTokenIdx++;

    auto right = parseExpression(tokens, currentTokenIdx, level + 1);
    if (right == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (verbose) {
        std::cout << indent(level) << "parsed assignment right" << std::endl;
    }

    auto assignmentNode = new AssignmentNode();
    assignmentNode->setLeft(left);
    assignmentNode->setRight(right);
    return assignmentNode;
}

IfStatementNode *SimpleParser::parseIf(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IF)) {
        return nullptr;
    }
    if (verbose) {
        std::cout << indent(level) << "parsing if statement" << std::endl;
    }

    currentTokenIdx++;

    auto condition = parseExpression(tokens, currentTokenIdx, level + 1);
    if (condition == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_CURLY_BRACE)) {
        currentTokenIdx--;
        return nullptr;
    }

    SequenceNode *ifBody = parseScope(tokens, currentTokenIdx, level + 1);
    if (ifBody == nullptr || ifBody->getChildren().empty()) {
        ifBody = nullptr;
    }

    SequenceNode *elseBody = nullptr;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::ELSE) {
        currentTokenIdx++;
        elseBody = parseScope(tokens, currentTokenIdx, level + 1);
        if (elseBody == nullptr) {
            currentTokenIdx--;
            return nullptr;
        } else if (elseBody->getChildren().empty()) {
            elseBody = nullptr;
        }
    }

    auto ifNode = new IfStatementNode();
    ifNode->setCondition(condition);
    ifNode->setIfBody(ifBody);
    ifNode->setElseBody(elseBody);
    return ifNode;
}

ForStatementNode *SimpleParser::parseFor(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::FOR)) {
        return nullptr;
    }
    if (verbose) {
        std::cout << indent(level) << "parsing for statement" << std::endl;
    }

    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    auto init = parseStatement(tokens, currentTokenIdx, level + 1);
    if (init == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SEMICOLON)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto condition = parseExpression(tokens, currentTokenIdx, level + 1);
    if (condition == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SEMICOLON)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto update = parseStatement(tokens, currentTokenIdx, level + 1);
    if (update == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    auto body = parseScope(tokens, currentTokenIdx, level + 1);
    if (body == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    auto forStatement = new ForStatementNode();
    forStatement->setInit(init);
    forStatement->setCondition(condition);
    forStatement->setUpdate(update);
    forStatement->setBody(body);
    return forStatement;
}

StatementNode *createStatementNode(AstNode *child) {
    auto statement = new StatementNode();
    statement->setChild(child);
    return statement;
}

StatementNode *SimpleParser::parseReturnStatement(const std::vector<Token> &tokens, int &currentTokenIdx,
                                                  int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RETURN)) {
        return nullptr;
    }
    if (verbose) {
        std::cout << indent(level) << "parsing return statement" << std::endl;
    }

    currentTokenIdx++;

    auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
    if (expression == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }

    auto *statement = createStatementNode(expression);
    statement->setIsReturnStatement(true);
    return statement;
}

AssertNode *SimpleParser::parseAssert(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::ASSERT)) {
        return nullptr;
    }
    if (verbose) {
        std::cout << indent(level) << "parsing assert statement" << std::endl;
    }

    currentTokenIdx++;

    auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
    if (expression == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }
    auto *result = new AssertNode();
    result->setCondition(expression);
    return result;
}

StatementNode *SimpleParser::parseStatement(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (verbose) {
        std::cout << indent(level) << "parsing statement node" << std::endl;
    }

    while (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::NEW_LINE) {
        currentTokenIdx++;
    }

    auto importNode = parseImport(tokens, currentTokenIdx);
    if (importNode != nullptr) {
        return createStatementNode(importNode);
    }

    auto assertNode = parseAssert(tokens, currentTokenIdx, level + 1);
    if (assertNode != nullptr) {
        return createStatementNode(assertNode);
    }

    auto callNode = parseFunctionCall(tokens, currentTokenIdx, level + 1);
    if (callNode != nullptr) {
        return createStatementNode(callNode);
    }

    auto functionNode = parseFunction(tokens, currentTokenIdx, level + 1);
    if (functionNode != nullptr) {
        return createStatementNode(functionNode);
    }

    auto ifNode = parseIf(tokens, currentTokenIdx, level + 1);
    if (ifNode != nullptr) {
        return createStatementNode(ifNode);
    }

    auto forNode = parseFor(tokens, currentTokenIdx, level + 1);
    if (forNode != nullptr) {
        return createStatementNode(forNode);
    }

    auto assignmentNode = parseAssignment(tokens, currentTokenIdx, level + 1);
    if (assignmentNode != nullptr) {
        return createStatementNode(assignmentNode);
    }

    auto variableDefinition = parseVariableDefinition(tokens, currentTokenIdx, level + 1);
    if (variableDefinition != nullptr) {
        return createStatementNode(variableDefinition);
    }

    auto returnStatement = parseReturnStatement(tokens, currentTokenIdx, level + 1);
    if (returnStatement != nullptr) {
        return returnStatement;
    }

    return nullptr;
}

void SimpleParser::run() {
    while (true) {
        const Token token = getNextToken();
        if (token.type == Token::INVALID) {
            break;
        }
    }

    std::vector<AstNode *> nodeStack = {};
    nodeStack.push_back(new SequenceNode());

    int count = 0;
    int currentTokenIdx = 0;
    while (currentTokenIdx < module->tokens.size() && count < 5) {
        auto token = module->tokens[currentTokenIdx];
        if (token.type == Token::INVALID) {
            break;
        }

        if (token.type == Token::NEW_LINE) {
            currentTokenIdx++;
            continue;
        }

        auto statementNode = parseStatement(module->tokens, currentTokenIdx, 0);
        if (statementNode != nullptr) {
            assert(nodeStack.back()->getAstNodeType() == ast::NodeType::SEQUENCE);
            reinterpret_cast<SequenceNode *>(nodeStack.back())->getChildren().push_back(statementNode);
            continue;
        }

        if (verbose) {
            std::cerr << "Unexpected token: " << token.type << ": " << token.content << std::endl;
        }
        count++;
    }
    module->root = nodeStack[0];
}
