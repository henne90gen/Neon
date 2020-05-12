#include "TypeDeclarationNode.h"

#include "../AstVisitor.h"

void TypeDeclarationNode::accept(AstVisitor *v) {
    v->visitTypeDeclarationNode(this);
}
