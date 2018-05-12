#include "cc_sem.h" 

void setIdDataType(TokenInfo *id, int dataType) {
    id->dataType = dataType;
}

void setIdNodeDataType(comp_tree_t *node, int dataType) {
    AstNodeInfo *nodeInfo = node->value;
    nodeInfo->tokenInfo->dataType = dataType;
}


int getASTNodeTokenDataType(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    if (nodeInfo->type == AST_ARIM_INVERSAO) {
        nodeInfo = node->first->value;
    }
    return nodeInfo->tokenInfo->dataType;
}

int checkDataTypeMatching(int idDataType, int litDataType) {
    if (idDataType != litDataType) {
        printf("Semantic error (incompatible types) - line %d\n", getLineNumber());
        return 0;
    }
    return 1;
}