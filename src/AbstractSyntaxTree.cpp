#include "AbstractSyntaxTree.h"

#include <iostream>

bool isBinaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::SUM || node->symbol == GrammarSymbol::TERM) && node->children.size() == 3;
}

bool isUnaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::NEGATION) && node->children.size() == 2;
}

bool isLiteral(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::INTEGER || node->symbol == GrammarSymbol::FLOAT ||
           node->symbol == GrammarSymbol::TRUE || node->symbol == GrammarSymbol::FALSE;
}

bool isSequence(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STMTS && node->children.size() > 1; }

bool isStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STMT; }

bool isIgnored(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::SEMICOLON || node->symbol == GrammarSymbol::ENDOFFILE;
}

BinaryOperationNode::BinaryOperationType getBinaryOperationType(GrammarSymbol symbol) {
    switch (symbol) {
    case GrammarSymbol::PLUS:
        return BinaryOperationNode::ADDITION;
    case GrammarSymbol::MINUS:
        return BinaryOperationNode::SUBTRACTION;
    case GrammarSymbol::STAR:
        return BinaryOperationNode::MULTIPLICATION;
    case GrammarSymbol::DIV:
        return BinaryOperationNode::DIVISION;
    default:
        std::cout << "Could not determine binary opartor type! (" << to_string(symbol) << ")" << std::endl;
        exit(1);
        return BinaryOperationNode::ADDITION;
    }
}

UnaryOperationNode::UnaryOperationType getUnaryOperationType(GrammarSymbol symbol) {
    switch (symbol) {
    case GrammarSymbol::NOT:
        return UnaryOperationNode::NOT;
    default:
        std::cout << "Could not determine unary opartor type! (" << to_string(symbol) << ")" << std::endl;
        exit(1);
        return UnaryOperationNode::NOT;
    }
}

AstNode *createBinaryOperation(ParseTreeNode *node) {
    auto nodeType = getBinaryOperationType(node->children[1]->symbol);
    auto astNode = new BinaryOperationNode(nodeType);

    auto leftNode = createAstFromParseTree(node->children[0]);
    astNode->setLeft(leftNode);

    auto rightNode = createAstFromParseTree(node->children[2]);
    astNode->setRight(rightNode);
    return astNode;
}

AstNode *createUnaryOperation(ParseTreeNode *node) {
    auto nodeType = getUnaryOperationType(node->children[0]->symbol);
    auto unaryOperationNode = new UnaryOperationNode(nodeType);

    auto child = createAstFromParseTree(node->children[1]);
    unaryOperationNode->setChild(child);
    return unaryOperationNode;
}

AstNode *createLiteral(ParseTreeNode *node) {
    if (node->token.content.empty()) {
        std::cout << "Missing token content. Can't create a literal." << std::endl;
        return nullptr;
    }

    switch (node->symbol) {
    case GrammarSymbol::INTEGER: {
        int value = std::stoi(node->token.content);
        return new IntegerNode(value);
    }
    case GrammarSymbol::FLOAT: {
        float value = std::stof(node->token.content);
        return new FloatNode(value);
    }
    case GrammarSymbol::TRUE:
    case GrammarSymbol::FALSE: {
        bool value = false;
        if (node->token.content == "true") {
            value = true;
        }
        return new BoolNode(value);
    }
    default:
        std::cout << "Data type not supported yet!" << std::endl;
        return nullptr;
    }
}

AstNode *createSequence(ParseTreeNode *node, SequenceNode *seqRoot = nullptr) {
    if (seqRoot == nullptr) {
        seqRoot = new SequenceNode();
    }

    for (auto child : node->children) {
        if (isSequence(child)) {
            seqRoot = (SequenceNode *)createSequence(child, seqRoot);
            continue;
        }
        auto astChild = createAstFromParseTree(child);
        if (astChild != nullptr) {
            seqRoot->getChildren().push_back(astChild);
        }
    }

    if (seqRoot->getChildren().size() == 1) {
        auto result = seqRoot->getChildren()[0];
        delete seqRoot;
        return result;
    }

    return seqRoot;
}

AstNode *createStatement(ParseTreeNode *node) {
    auto statementNode = new StatementNode();
    if (node->children.size() > 1) {
        std::cout << "A statement should never have more than one child." << std::endl;
    }
    auto child = createAstFromParseTree(node->children[0]);
    statementNode->setChild(child);
    return statementNode;
}

AstNode *createAstFromParseTree(ParseTreeNode *node) {
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
        return createAstFromParseTree(node->children[0]);
    }

    if (node->symbol == GrammarSymbol::FACTOR && node->children.size() == 3 &&
        node->children[0]->symbol == GrammarSymbol::LEFT_PARAN &&
        node->children[2]->symbol == GrammarSymbol::RIGHT_PARAN) {
        return createAstFromParseTree(node->children[1]);
    }

    if (isIgnored(node)) {
        return nullptr;
    }

    std::cout << "Could not find a suitable AstNode for " << to_string(node->symbol) << std::endl;

    return nullptr;
}

void indent(int indentation) {
    for (int i = 0; i < indentation; i++) {
        std::cout << "  ";
    }
}

void SequenceNode::print(int indentation) {
    indent(indentation);
    std::cout << "SequenceNode(size=" << children.size() << ")" << std::endl;
    for (auto child : children) {
        child->print(indentation + 1);
    }
}

void StatementNode::print(int indentation) {
    indent(indentation);
    std::cout << "StatementNode(hasChild=" << (child != nullptr) << ")" << std::endl;
    if (child != nullptr) {
        child->print(indentation + 1);
    }
}

void IntegerNode::print(int indentation) {
    indent(indentation);
    std::cout << "IntegerNode(value=" << value << ")" << std::endl;
}

void FloatNode::print(int indentation) {
    indent(indentation);
    std::cout << "FloatNode(value=" << value << ")" << std::endl;
}

void BoolNode::print(int indentation) {
    indent(indentation);
    std::cout << "BoolNode(value=" << value << ")" << std::endl;
}

void UnaryOperationNode::print(int indentation) {
    indent(indentation);
    std::string operationType = "ERROR";
    if (type == UnaryOperationNode::NOT) {
        operationType = "NOT";
    }
    std::cout << "UnaryOperationNode(hasChild=" << (child != nullptr) << ", type=" << operationType << ")" << std::endl;
    if (child != nullptr) {
        child->print(indentation + 1);
    }
}

void BinaryOperationNode::print(int indentation) {
    indent(indentation);
    std::string operationType = "ERROR";
    if (type == BinaryOperationNode::ADDITION) {
        operationType = "ADDITION";
    } else if (type == BinaryOperationNode::SUBTRACTION) {
        operationType = "SUBTRACTION";
    } else if (type == BinaryOperationNode::MULTIPLICATION) {
        operationType = "MULTIPLICATION";
    } else if (type == BinaryOperationNode::DIVISION) {
        operationType = "DIVISION";
    }
    std::cout << "UnaryOperationNode(hasLeft=" << (left != nullptr) << ", hasRight=" << (right != nullptr)
              << ", type=" << operationType << ")" << std::endl;
    if (left != nullptr) {
        left->print(indentation + 1);
    }
    if (right != nullptr) {
        right->print(indentation + 1);
    }
}
