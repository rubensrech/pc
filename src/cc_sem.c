#include <stdlib.h>
#include "cc_sem.h"

int scope = 0;
int scope_uniq = 0;

void setIdDataType(TokenInfo *id, int dataType) {
    // Check id is already declared (tried to redeclare)
    if (id->dataType != DATATYPE_UNDEF) {
        throwSemanticError("Identifier redeclaration", IKS_ERROR_DECLARED);
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
    if (idDataType != initDataType) {
        throwSemanticError("Incompatitle types", IKS_ERROR_INCOMP_TYPES);
    }
}

void checkIdDeclared(TokenInfo *id) {
    TokenInfo *globalId;
    // Check id declared in current scope
    if (id->dataType == DATATYPE_UNDEF) {
        // Check id declared in global scope
        globalId = searchIdInGlobalScope(id->lexeme);
        if (globalId != NULL) {
            // Found in global scope -> set id as defined
            id->dataType = globalId->dataType;
        } else {
            throwSemanticError("Undeclared identifier", IKS_ERROR_UNDECLARED);
        }
    }
}

void checkIdNodeDeclared(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    checkIdDeclared(nodeInfo->tokenInfo);
}

void throwSemanticError(const char *errorMsg, int errorCode) {
    printf("Semantic error: %s - on line %d\n", errorMsg, getLineNumber());
    exit(errorCode);
}

TokenInfo *searchIdInGlobalScope(char *id) {
    const int globalScope = 0;
    return lookUpForIdInSymbolsTable(id, globalScope);
}