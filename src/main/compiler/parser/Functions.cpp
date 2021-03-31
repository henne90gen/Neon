#include "Parser.h"

CallNode *Parser::parseFunctionCall(int level) {
    if (!currentTokenIs(Token::IDENTIFIER)) {
        return nullptr;
    }
    auto beforeTokenIdx = currentTokenIdx;
    std::string name = currentTokenContent();
    currentTokenIdx++;

    if (!currentTokenIs(Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    log.debug(indent(level) + "parsing call node");

    std::vector<AstNode *> params = {};
    while (!currentTokenIs(Token::RIGHT_PARAN)) {
        auto *expression = parseExpression(level + 1);
        if (expression == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        params.push_back(expression);
        if (currentTokenIs(Token::COMMA)) {
            currentTokenIdx++;
        }
    }

    if (!currentTokenIs(Token::RIGHT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto *callNode = new CallNode(name);
    for (auto *const param : params) {
        callNode->getArguments().push_back(param);
    }
    return callNode;
}

FunctionNode *Parser::parseFunction(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    if (currentTokenIs(Token::EXTERN)) {
        currentTokenIdx++;
    }

    if (!currentTokenIs(Token::FUN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    if (!currentTokenIs(Token::IDENTIFIER)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    log.debug(indent(level) + "parsing function node");

    std::string functionName = currentTokenContent();
    currentTokenIdx++;

    if (!currentTokenIs(Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    std::vector<VariableDefinitionNode *> params = {};
    do {
        currentTokenIdx++;
        auto *variableDefinitionNode = parseVariableDefinition(level + 1);
        if (variableDefinitionNode == nullptr) {
            break;
        }
        params.push_back(variableDefinitionNode);
    } while (currentTokenIs(Token::COMMA));

    if (!currentTokenIs(Token::RIGHT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    ast::DataType returnType = ast::DataType();
    if (currentTokenIs(Token::SIMPLE_DATA_TYPE)) {
        returnType = ast::DataType(from_string(currentTokenContent()));
        currentTokenIdx++;
    }

    auto *body = parseScope(level + 1);

    auto *functionNode = new FunctionNode(functionName, returnType);
    functionNode->setBody(body);
    for (auto *node : params) {
        functionNode->getArguments().push_back(node);
    }
    return functionNode;
}
