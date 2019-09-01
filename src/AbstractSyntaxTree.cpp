#include "AbstractSyntaxTree.h"

#include <iostream>

std::string to_string(AstNode::AstNodeType type) {
    switch (type) {
        case AstNode::BIN_OP_ADD:
            return "ADD";
        case AstNode::BIN_OP_SUB:
            return "SUB";
        case AstNode::BIN_OP_MUL:
            return "MUL";
        case AstNode::BIN_OP_DIV:
            return "DIV";
        case AstNode::SEQUENCE:
            return "SEQ";
        case AstNode::STATEMENT:
            return "STMT";
        case AstNode::FLOAT_LIT:
            return "FLOAT";
        case AstNode::INT_LIT:
            return "INT";
        case AstNode::BOOL_LIT:
            return "BOOL";
        case AstNode::UNARY_OP_NOT:
            return "NOT";
        default:
            return "ERROR";
    }
}

bool isBinaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::SUM ||
            node->symbol == GrammarSymbol::TERM) &&
           node->children.size() == 3;
}

bool isUnaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::NEGATION) &&
           node->children.size() == 2;
}

bool isLiteral(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::INT_LIT ||
           node->symbol == GrammarSymbol::FLOAT_LIT ||
           node->symbol == GrammarSymbol::TRUE_LIT ||
           node->symbol == GrammarSymbol::FALSE_LIT;
}

bool isSequence(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::STATEMENTS && node->children.size() > 1;
}

bool isStatement(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::STATEMENT;
}

bool isIgnored(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::SEMICOLON ||
           node->symbol == GrammarSymbol::END_OF_FILE;
}

AstNode::AstNodeType getBinaryOperationType(GrammarSymbol symbol) {
    switch (symbol) {
        case GrammarSymbol::PLUS:
            return AstNode::BIN_OP_ADD;
        case GrammarSymbol::MINUS:
            return AstNode::BIN_OP_SUB;
        case GrammarSymbol::STAR:
            return AstNode::BIN_OP_MUL;
        case GrammarSymbol::DIV:
            return AstNode::BIN_OP_DIV;
    }

    std::cout << "Could not determine binary opartor type! (" << to_string(symbol)
              << ")" << std::endl;
    exit(1);

    return AstNode::BIN_OP_ADD;
}

AstNode::AstNodeType getUnaryOperationType(GrammarSymbol symbol) {
    switch (symbol) {
        case GrammarSymbol::NOT:
            return AstNode::UNARY_OP_NOT;
    }

    std::cout << "Could not determine unary opartor type! (" << to_string(symbol)
              << ")" << std::endl;
    exit(1);

    return AstNode::UNARY_OP_NOT;
}

AstNode::AstNodeType getLiteralType(GrammarSymbol symbol) {
    switch (symbol) {
        case GrammarSymbol::INT_LIT:
            return AstNode::INT_LIT;
        case GrammarSymbol::FLOAT_LIT:
            return AstNode::FLOAT_LIT;
        case GrammarSymbol::TRUE_LIT:
        case GrammarSymbol::FALSE_LIT:
            return AstNode::BOOL_LIT;
    }

    std::cout << "Could not determine literal type! (" << to_string(symbol) << ")"
              << std::endl;
    exit(1);

    return AstNode::INT_LIT;
}

AstNode *createBinaryOperation(ParseTreeNode *node) {
    auto nodeType = getBinaryOperationType(node->children[1]->symbol);
    auto astNode = new AstNode(nodeType);

    auto leftNode = AstNode::createAstFromParseTree(node->children[0]);
    astNode->children.push_back(leftNode);

    auto rightNode = AstNode::createAstFromParseTree(node->children[2]);
    astNode->children.push_back(rightNode);
    return astNode;
}

AstNode *createUnaryOperation(ParseTreeNode *node) {
    auto nodeType = getUnaryOperationType(node->children[0]->symbol);
    auto astNode = new AstNode(nodeType);

    auto child = AstNode::createAstFromParseTree(node->children[1]);
    astNode->children.push_back(child);
    return astNode;
}

AstNode *createLiteral(ParseTreeNode *node) {
    auto nodeType = getLiteralType(node->symbol);
    auto astNode = new AstNode(nodeType);

    if (nodeType == AstNode::FLOAT_LIT) {
        astNode->data = new FloatData(std::stof(node->token.content));
    } else if (nodeType == AstNode::INT_LIT) {
        astNode->data = new IntegerData(std::stoi(node->token.content));
    } else if (nodeType == AstNode::BOOL_LIT) {
        bool value = false;
        if (node->token.content == "true") {
            value = true;
        }
        astNode->data = new BoolData(value);
    } else {
        std::cout << "Data type not supported yet!" << std::endl;
        return nullptr;
    }

    return astNode;
}

AstNode *createSequence(ParseTreeNode *node, AstNode *seqRoot = nullptr) {
    if (seqRoot == nullptr) {
        auto nodeType = AstNode::SEQUENCE;
        seqRoot = new AstNode(nodeType);
    }

    for (auto child : node->children) {
        if (isSequence(child)) {
            seqRoot = createSequence(child, seqRoot);
            continue;
        }
        auto astChild = AstNode::createAstFromParseTree(child);
        if (astChild != nullptr) {
            seqRoot->children.push_back(astChild);
        }
    }

    if (seqRoot->children.size() == 1) {
        auto result = seqRoot->children[0];
        delete seqRoot;
        return result;
    }

    return seqRoot;
}

AstNode *createStatement(ParseTreeNode *node) {
    auto nodeType = AstNode::STATEMENT;
    auto astNode = new AstNode(nodeType);
    if (node->children.size() > 1) {
        std::cout << "A statement should never have more than one child."
                  << std::endl;
    }
    auto child = AstNode::createAstFromParseTree(node->children[0]);
    astNode->children.push_back(child);
    return astNode;
}

AstNode *AstNode::createAstFromParseTree(ParseTreeNode *node) {
    if (node == nullptr) {
        return nullptr;
    }

    if (isBinaryOperation(node)) {
        return createBinaryOperation(node);
    }

    if (isUnaryOperation(node)) {
        return createUnaryOperation(node);
    }

    if (isLiteral(node)) {
        return createLiteral(node);
    }

    if (isSequence(node)) {
        return createSequence(node);
    }

    if (isStatement(node)) {
        return createStatement(node);
    }

    if (node->children.size() == 1 || node->symbol == GrammarSymbol::PROGRAM) {
        return AstNode::createAstFromParseTree(node->children[0]);
    }

    if (node->symbol == GrammarSymbol::FACTOR && node->children.size() == 3 &&
        node->children[0]->symbol == GrammarSymbol::LEFT_PARAN &&
        node->children[2]->symbol == GrammarSymbol::RIGHT_PARAN) {
        return AstNode::createAstFromParseTree(node->children[1]);
    }

    if (isIgnored(node)) {
        return nullptr;
    }

    std::cout << "Could not find a suitable AstNode for "
              << to_string(node->symbol) << std::endl;

    return nullptr;
}
