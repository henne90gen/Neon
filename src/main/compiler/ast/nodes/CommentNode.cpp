#include "CommentNode.h"

#include "../AstVisitor.h"

void CommentNode::accept(AstVisitor *v) { v->visitCommentNode(this); }

const std::string &CommentNode::getContent() const { return content; }
