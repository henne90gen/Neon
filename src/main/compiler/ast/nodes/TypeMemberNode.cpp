#include "TypeMemberNode.h"

#include "../AstVisitor.h"

void TypeMemberNode::accept(AstVisitor *v) {
    v->visitTypeMemberNode(this);
}
