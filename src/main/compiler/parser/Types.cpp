#include "Parser.h"

LiteralNode *Parser::parseLiteral(int level) {
    log.debug(indent(level) + "parsing literal node");

    if (currentTokenIs(Token::INTEGER)) {
        log.debug(indent(level) + "parsing integer node");
        int value = std::stoi(currentTokenContent());
        currentTokenIdx++;
        return new IntegerNode(value);
    }

    if (currentTokenIs(Token::FLOAT)) {
        log.debug(indent(level) + "parsed float node");
        float value = std::stof(currentTokenContent());
        currentTokenIdx++;
        return new FloatNode(value);
    }

    if (currentTokenIs(Token::BOOLEAN)) {
        log.debug(indent(level) + "parsed boolean node");
        bool value = currentTokenContent() == "true";
        currentTokenIdx++;
        return new BoolNode(value);
    }

    if (currentTokenIs(Token::STRING)) {
        log.debug(indent(level) + "parsed string node");
        std::string value = currentTokenContent();
        value = value.substr(1, value.size() - 2);
        currentTokenIdx++;
        return new StringNode(value);
    }

    log.debug(indent(level) + "failed to parse literal node");
    return nullptr;
}

TypeMemberNode *Parser::parseMemberVariable(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto node = parseVariableDefinition(level + 1);
    if (node == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    return new TypeMemberNode(node);
}

TypeDeclarationNode *Parser::parseTypeDeclaration(int level) {
    if (!currentTokenIs(Token::TYPE)) {
        return nullptr;
    }

    int beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    if (!currentTokenIs(Token::IDENTIFIER)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    std::string name = currentTokenContent();
    currentTokenIdx++;

    if (!currentTokenIs(Token::LEFT_CURLY_BRACE)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    std::vector<TypeMemberNode *> memberVariables = {};
    do {
        while (currentTokenIs(Token::NEW_LINE)) {
            currentTokenIdx++;
        }

        auto memberVariable = parseMemberVariable(level + 1);
        if (memberVariable == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        memberVariables.push_back(memberVariable);

        while (currentTokenIs(Token::NEW_LINE)) {
            currentTokenIdx++;
        }
    } while (!currentTokenIs(Token::RIGHT_CURLY_BRACE));

    if (!currentTokenIs(Token::RIGHT_CURLY_BRACE)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto node = new TypeDeclarationNode(name);
    node->setMembers(memberVariables);
    return node;
}
