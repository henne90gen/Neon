#include "ImportFinder.h"

#include "../nodes/AllNodes.h"

void ImportFinder::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void ImportFinder::visitStatementNode(StatementNode *node) {
    node->getChild()->accept(this);
}

void ImportFinder::visitImportNode(ImportNode *node) {
    importedModules.push_back(node->getFileName());
}

std::vector<std::string> ImportFinder::run(Module *module) {
    module->root->accept(this);
    return importedModules;
}
