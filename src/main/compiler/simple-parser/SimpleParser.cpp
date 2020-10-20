#include "SimpleParser.h"

#include <iostream>

#include "../ast/Types.h"

Token SimpleParser::getNextToken() {
    const Token token = lexer.getToken();
    module->tokens.push_back(token);
    return token;
}

std::string indent(int level) {
    std::string result = "";
    for (int i = 0; i < level; i++) {
        result += " ";
    }
    return result;
}

AstNode *parseExpression(const std::vector<Token> &tokens, int &currentTokenIdx, int level);
StatementNode *parseStatement(const std::vector<Token> &tokens, int &currentTokenIdx, int level);

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

LiteralNode *parseLiteral(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    std::cout << indent(level) << "parsing literal node" << std::endl;

    if (tokens[currentTokenIdx].type == Token::INTEGER) {
        std::cout << indent(level) << "parsed integer node" << std::endl;
        int value = std::stoi(tokens[currentTokenIdx].content);
        currentTokenIdx++;
        return new IntegerNode(value);
    }

    if (tokens[currentTokenIdx].type == Token::STRING) {
        std::cout << indent(level) << "parsed string node" << std::endl;
        std::string value = tokens[currentTokenIdx].content;
        value = value.substr(1, value.size() - 2);
        currentTokenIdx++;
        return new StringNode(value);
    }

    std::cout << indent(level) << "failed to parse literal node" << std::endl;
    return nullptr;
}

VariableNode *parseVariable(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    // TODO parse variable node
    return nullptr;
}

AstNode *parseBinaryLeft(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
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

    auto variable = parseVariable(tokens, currentTokenIdx, level + 1);
    if (variable != nullptr) {
        return variable;
    }

    return nullptr;
}

AstNode *parseBinaryRight(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
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

BinaryOperationNode *parseBinaryOperation(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    std::cout << indent(level) << "parsing binary operation node" << std::endl;

    int tokenIdxBeforeLeft = currentTokenIdx;
    auto left = parseBinaryLeft(tokens, currentTokenIdx, level + 1);
    if (left == nullptr) {
        return nullptr;
    }

    std::cout << indent(level) << "parsed binary operation left" << std::endl;

    if (currentTokenIdx >= tokens.size()) {
        currentTokenIdx = tokenIdxBeforeLeft;
        std::cout << indent(level) << "failed to parse binary operation" << std::endl;
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
        std::cout << indent(level) << "failed to parse binary operation" << std::endl;
        return nullptr;
    }

    currentTokenIdx++;

    auto right = parseBinaryRight(tokens, currentTokenIdx, level + 1);
    if (right == nullptr) {
        std::cout << indent(level) << "failed to parse binary operation right" << std::endl;
        currentTokenIdx--;
        return nullptr;
    }

    std::cout << indent(level) << "parsed binary operation right" << std::endl;

    auto binaryOperation = new BinaryOperationNode(operationType);
    binaryOperation->setLeft(left);
    binaryOperation->setRight(right);
    return binaryOperation;
}

AstNode *parseExpressionInsideParens(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    auto binaryOperation = parseBinaryOperation(tokens, currentTokenIdx, level + 1);
    if (binaryOperation != nullptr) {
        return binaryOperation;
    }

    auto literal = parseLiteral(tokens, currentTokenIdx, level + 1);
    if (literal != nullptr) {
        return literal;
    }

    auto variable = parseVariable(tokens, currentTokenIdx, level + 1);
    if (variable != nullptr) {
        return variable;
    }

    return nullptr;
}

AstNode *parseExpression(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    std::cout << indent(level) << "parsing expression node" << std::endl;

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

CallNode *parseCall(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    if (tokens[currentTokenIdx].type != Token::IDENTIFIER) {
        return nullptr;
    }

    if (currentTokenIdx + 1 >= tokens.size() || tokens[currentTokenIdx + 1].type != Token::LEFT_PARAN) {
        return nullptr;
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

VariableDefinitionNode *parseVariableDefinition(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    std::cout << indent(level) << "parsing variable definition node" << std::endl;

    if (tokens[currentTokenIdx].type == Token::SIMPLE_DATA_TYPE &&
        (currentTokenIdx + 1 < tokens.size() && tokens[currentTokenIdx + 1].type == Token::IDENTIFIER)) {
        std::string variableName = tokens[currentTokenIdx + 1].content;
        auto dataType = ast::DataType(from_string(tokens[currentTokenIdx].content));
        auto variableDefinitionNode = new VariableDefinitionNode(variableName, dataType);
        currentTokenIdx += 2;
        std::cout << indent(level) << "parsed variable definition with simple data type" << std::endl;
        return variableDefinitionNode;
    }

    std::cout << indent(level) << "failed to parse variable definition" << std::endl;
    // TODO parse custom types as well

    return nullptr;
}

FunctionNode *parseFunction(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
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

    std::cout << indent(level) << "parsing function node" << std::endl;

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

    SequenceNode *body = nullptr;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_CURLY_BRACE) {
        std::cout << indent(level) << "parsing function body" << std::endl;
        currentTokenIdx++;
        body = new SequenceNode();
        while (true) {
            auto statement = parseStatement(tokens, currentTokenIdx, level + 1);
            if (statement == nullptr) {
                break;
            }

            body->getChildren().push_back(statement);
        }

        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_CURLY_BRACE) {
            currentTokenIdx++;
        } else {
            body = nullptr;
        }
    }

    auto functionNode = new FunctionNode(functionName, returnType);
    functionNode->setBody(body);
    for (auto node : params) {
        functionNode->getArguments().push_back(node);
    }
    return functionNode;
}

AstNode *parseAssignmentLeft(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
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

AssignmentNode *parseAssignment(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    std::cout << indent(level) << "parsing assignment node" << std::endl;
    auto left = parseAssignmentLeft(tokens, currentTokenIdx, level + 1);
    if (left == nullptr) {
        return nullptr;
    }
    std::cout << indent(level) << "parsed assignment left" << std::endl;

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SINGLE_EQUALS)) {
        return nullptr;
    }
    currentTokenIdx++;

    auto right = parseExpression(tokens, currentTokenIdx, level + 1);
    if (right == nullptr) {
        return nullptr;
    }

    std::cout << indent(level) << "parsed assignment right" << std::endl;

    auto assignmentNode = new AssignmentNode();
    assignmentNode->setLeft(left);
    assignmentNode->setRight(right);
    return assignmentNode;
}

StatementNode *parseIf(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    return nullptr;
}

StatementNode *createStatementNode(AstNode *ifNode) {
    auto statement = new StatementNode();
    statement->setChild(ifNode);
    return statement;
}

StatementNode *parseStatementNode(const std::vector<Token> &tokens, int &currentTokenIdx, int level) {
    std::cout << indent(level) << "parsing statement node" << std::endl;

    while (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::NEW_LINE) {
        currentTokenIdx++;
    }

    auto importNode = parseImport(tokens, currentTokenIdx);
    if (importNode != nullptr) {
        return createStatementNode(importNode);
    }

    auto callNode = parseCall(tokens, currentTokenIdx, level + 1);
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

    auto assignmentNode = parseAssignment(tokens, currentTokenIdx, level + 1);
    if (assignmentNode != nullptr) {
        return createStatementNode(assignmentNode);
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

        auto statementNode = parseStatementNode(module->tokens, currentTokenIdx, 0);
        if (statementNode != nullptr) {
            assert(nodeStack.back()->getAstNodeType() == ast::NodeType::SEQUENCE);
            reinterpret_cast<SequenceNode *>(nodeStack.back())->getChildren().push_back(statementNode);
            continue;
        }

        std::cout << token.type << ": " << token.content << std::endl;
        count++;
    }
    module->root = nodeStack[0];
}
