#include "ImportNode.h"

#include "../AstVisitor.h"

#include <utility>

void ImportNode::accept(AstVisitor *v) { v->visitImportNode(this); }

void ImportNode::setFileName(std::string fileName) { this->fileName = std::move(fileName); }

std::string ImportNode::getFileName() { return fileName; }
