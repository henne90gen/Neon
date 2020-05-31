#include "MemberAccessNode.h"

#include "../AstVisitor.h"

void MemberAccessNode::accept(AstVisitor *visitor) {
    visitor->visitMemberAccessNode(this);
}
