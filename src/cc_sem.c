#include <stdlib.h>
#include "cc_sem.h"

void setIdDataType(TokenInfo *id, int dataType) {
    // Check id is already declared (tried to redeclare)
    if (id->dataType != DATATYPE_UNDEF) {
        printf("Semantic error (already declared) - line %d\n", getLineNumber());
        exit(IKS_ERROR_DECLARED);
    } else {
        id->dataType = dataType;
    }
}

void setIdNodeDataType(comp_tree_t *node, int dataType) {
    AstNodeInfo *nodeInfo = node->value;
    setIdDataType(nodeInfo->tokenInfo, dataType);
}

int getASTNodeTokenDataType(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    // When 'node' is an AST_ARIM_INVERSAO node (ex: -5),
    // returns dataType from AST child node which contains the literal (float/int)
    if (nodeInfo->type == AST_ARIM_INVERSAO) { 
        nodeInfo = node->first->value;
    }
    return nodeInfo->tokenInfo->dataType;
}

void checkDataTypeMatching(int idDataType, int initDataType) {
    // Check declaration for var init with id, ex: int a <= b;
    /*if (initDataType == DATATYPE_UNDEF) {
        printf("Semantic error (undeclared) - line %d\n", getLineNumber());
        exit(IKS_ERROR_UNDECLARED);
    } else*/  if (idDataType != initDataType) {
        printf("Semantic error (incompatible types) - line %d\n", getLineNumber());
        exit(IKS_ERROR_INCOMP_TYPES);
    }
}

void checkIdDeclared(TokenInfo *id) {
    if (id->dataType == DATATYPE_UNDEF) {
        printf("Semantic error (undeclared) - line %d\n", getLineNumber());
        exit(IKS_ERROR_UNDECLARED);
    }
}

void checkIdNodeDeclared(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    checkIdDeclared(nodeInfo->tokenInfo);
}