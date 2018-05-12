#include <stdlib.h>
#include "cc_sem.h"

void setIdDataType(TokenInfo *id, int dataType) {
    if (id->dataType == DATATYPE_UNDEF) {
        id->dataType = dataType;
    } else {
        printf("Semantic error (already declared) - line %d\n", getLineNumber());
        exit(IKS_ERROR_DECLARED);
    }
}

void setIdNodeDataType(comp_tree_t *node, int dataType) {
    AstNodeInfo *nodeInfo = node->value;
    if (nodeInfo->tokenInfo->dataType == DATATYPE_UNDEF) {
        nodeInfo->tokenInfo->dataType = dataType;
    } else {
        printf("Semantic error (already declared) - line %d\n", getLineNumber());
        exit(IKS_ERROR_DECLARED);
    }
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
        exit(IKS_ERROR_INCOMP_TYPES);
    }
    return 1;
}