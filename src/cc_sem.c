#include <stdlib.h>
#include "cc_sem.h"

int scope = 0;
int scope_uniq = 0;

void setIdType(TokenInfo *id, int idType) {
    char errorMsg[MAX_ERROR_MSG_SIZE];

    // Check id is already declared (tried to redeclare)
    if (id->idType != ID_TYPE_UNDEF) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Identifier '%s' redeclared", id->lexeme);
        throwSemanticError(errorMsg, IKS_ERROR_DECLARED);
    } else {
        id->idType = idType;
    }
}

void setIdNodeIdType(comp_tree_t *node, int idType) {
    AstNodeInfo *nodeInfo = node->value;
    setIdType(nodeInfo->tokenInfo, idType);
}

void setIdDataType(TokenInfo *id, int dataType) {
    // Check id is already declared (tried to redeclare)
    id->dataType = dataType;    
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
    char errorMsg[MAX_ERROR_MSG_SIZE];

    // Check id declared in current scope
    if (id->idType == ID_TYPE_UNDEF) {
        // Check id declared in global scope
        globalId = searchIdInGlobalScope(id->lexeme);
        if (globalId != NULL && globalId->idType != ID_TYPE_UNDEF) {
            // Found in global scope -> set id as defined
            id->idType = globalId->idType;
            id->dataType = globalId->dataType;
        } else {
            snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Undeclared identifier '%s'", id->lexeme);
            throwSemanticError(errorMsg, IKS_ERROR_UNDECLARED);
        }
    }
}

void checkIdNodeDeclared(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    checkIdDeclared(nodeInfo->tokenInfo);
}

void checkIdUsedAs(int usedAs, TokenInfo *id) {
    char errorMsg[MAX_ERROR_MSG_SIZE];

    if (usedAs != id->idType) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Wrong use for identifier '%s'", id->lexeme);

        switch (id->idType) {
        case VAR_ID: throwSemanticError(errorMsg, IKS_ERROR_VARIABLE); break;
        case ARRAY_ID: throwSemanticError(errorMsg, IKS_ERROR_VECTOR); break;
        case FUNC_ID: throwSemanticError(errorMsg, IKS_ERROR_FUNCTION); break;
        case USER_TYPE_ID: throwSemanticError(errorMsg, IKS_ERROR_USER_TYPE); break;
        }
        
    }
}

void checkIdNodeUsedAs(int usedAs, comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    checkIdUsedAs(usedAs, nodeInfo->tokenInfo);
}

void throwSemanticError(char *errorMsg, int errorCode) {
    printf("Semantic error: %s - on line %d\n", errorMsg, getLineNumber());
    exit(errorCode);
}

TokenInfo *searchIdInGlobalScope(char *id) {
    const int globalScope = 0;
    return lookUpForIdInSymbolsTable(id, globalScope);
}