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
