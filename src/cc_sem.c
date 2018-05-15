#include <stdlib.h>
#include "cc_sem.h"

int scope = 0;
int scope_uniq = 0;
comp_dict_t *funcTable;

/* Data type */

void setIdTokenDataType(TokenInfo *id, int dataType) {
    id->dataType = dataType;    
}

void setIdNodeTokenDataType(comp_tree_t *node, int dataType) {
    AstNodeInfo *nodeInfo = node->value;
    setIdTokenDataType(nodeInfo->tokenInfo, dataType);
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

int getASTNodeDataType(comp_tree_t *node) {
    return ((AstNodeInfo*)node->value)->dataType;
}

void checkDataTypeMatching(int dataType1, int dataType2) {
    // MISSING FIXES!!
    int arithmeticDataTypes[2] = { DATATYPE_INT, DATATYPE_FLOAT };
    int floatIntConversion = inArray(arithmeticDataTypes, 2, dataType1) && inArray(arithmeticDataTypes, 2, dataType2);

    if (!floatIntConversion && (dataType1 != dataType2)) {
        throwSemanticError("Incompatitle types 1", IKS_ERROR_INCOMP_TYPES);
    }
}

int checkArimExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2) {
    int arithmeticDataTypes[2] = { DATATYPE_INT, DATATYPE_FLOAT };

    int exp1DataType = getASTNodeDataType(exp1);
    // Use exp2 = NULL when only exp1 matters (example: - exp)
    int exp2DataType = (exp2 != NULL) ? getASTNodeDataType(exp2) : DATATYPE_INT;

    if (!inArray(arithmeticDataTypes, 2, exp1DataType) || !inArray(arithmeticDataTypes, 2, exp2DataType)) {
        throwSemanticError("Incompatitle types 2", IKS_ERROR_INCOMP_TYPES);
    }

    if (exp1DataType == DATATYPE_FLOAT || exp2DataType == DATATYPE_FLOAT)
        return DATATYPE_FLOAT;
    else
        return DATATYPE_INT;
}

int checkCompExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2) {
    int comparableDataTypes[3] = { DATATYPE_INT, DATATYPE_FLOAT, DATATYPE_CHAR };

    int exp1DataType = getASTNodeDataType(exp1);
    int exp2DataType = getASTNodeDataType(exp2);
    
    if (!inArray(comparableDataTypes, 3, exp1DataType) || !inArray(comparableDataTypes, 3, exp2DataType)) {
        throwSemanticError("Incompatitle types 4", IKS_ERROR_INCOMP_TYPES);
    } 
    
    return DATATYPE_BOOL;
}

int checkLogicExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2) {
    int exp1DataType = getASTNodeDataType(exp1);
    // Use exp2 = NULL when only exp1 matters (example: !exp)
    int exp2DataType = (exp2 != NULL) ? getASTNodeDataType(exp2) : DATATYPE_BOOL;

    if (exp1DataType != DATATYPE_BOOL || exp2DataType != DATATYPE_BOOL) {
        throwSemanticError("Incompatitle types 3", IKS_ERROR_INCOMP_TYPES);
    }

    return DATATYPE_BOOL;
}

void setNodeDataType(comp_tree_t *node, int dataType) {
    AstNodeInfo *nodeInfo = node->value;
    nodeInfo->dataType = dataType;
}

/* ID: Declaration and Use */

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

void checkIdDeclared(TokenInfo *id) {
    TokenInfo *globalId;
    char errorMsg[MAX_ERROR_MSG_SIZE];

    // Check id declared in current scope
    if (id->idType == ID_TYPE_UNDEF) {
        // Check id declared in global scope
        globalId = searchIdInGlobalScope(id->lexeme);
        if (globalId != NULL && globalId->idType != ID_TYPE_UNDEF) {
            // Found in global scope -> set id as defined and set its properties
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

TokenInfo *searchIdInGlobalScope(char *id) {
    const int globalScope = 0;
    return lookUpForIdInSymbolsTable(id, globalScope);
}

/* Functions */

void initFuncTable() {
    funcTable = dict_new();
}

void freeFuncTable() {
    int i;
    struct comp_dict_item *entry, *next;
    FuncDesc *descriptor;

    for (i = 0; i < funcTable->size; i++) {
        entry = funcTable->data[i];
        while (entry != NULL) {
            next = entry->next;
            descriptor = dict_remove(funcTable, entry->key);
            freeFuncDescriptor(descriptor);
            entry = next;
        }
    }

    dict_free(funcTable);
}

void freeFuncDescriptor(FuncDesc *descriptor) {
    freeParamsList(descriptor->params);
    free(descriptor);
}

void freeParamsList(comp_tree_t *list) {
    comp_tree_t *ptr = list;
	if (list != NULL) {
		freeParamsList(ptr->list_next);
        free(list);
    }
}

void printFuncTable() {
    int i;
    struct comp_dict_item *entry;
    FuncDesc *descriptor;

    printf("------ Func Table ------\n");

    for (i = 0; i < funcTable->size; i++) {
        entry = funcTable->data[i];
        while (entry != NULL) {
            descriptor = entry->value;

            printf("> %s (params: %d, retType: %d)\n", descriptor->id, countFuncParameters(descriptor->params), descriptor->returnDataType);
    
            entry = entry->next;
        }
    }
}

void insertFuncTable(TokenInfo *idInfo, comp_tree_t *params) {
    FuncDesc *funcDesc = malloc(sizeof(struct funcDesc));
    char *key;

    // printf("Insert func dec: %s\n", idInfo->lexeme);

    funcDesc->id = idInfo->lexeme;
    funcDesc->returnDataType = idInfo->dataType;
    funcDesc->params = params;

    key = funcDesc->id;

    dict_put(funcTable, key, funcDesc);
}

int countFuncParameters(comp_tree_t *params) {
	if (params != NULL) {
		return 1 + countFuncParameters(params->list_next);
	} else {
		return 0;
	}
}

void checkFuncCall(comp_tree_t *funcAST) {
    char errorMsg[MAX_ERROR_MSG_SIZE];
    comp_tree_t *funcIdNode = funcAST->first;
    comp_tree_t *funcParamsNode = funcAST->last;
    AstNodeInfo *funcIdInfo = funcIdNode->value;
    char *funcId = funcIdInfo->tokenInfo->lexeme;
    int hasParams = funcAST->childnodes > 1;
    int paramsCount = (hasParams) ? countFuncParameters(funcParamsNode) : 0;

    comp_tree_t *currExpcParam, *currParam;
    TokenInfo *currExpcParamInfo;
    AstNodeInfo *currParamNodeInfo;

    printf("Checking: %s, with %d param(s)\n", funcId, paramsCount);

    FuncDesc *funcDesc = dict_get(funcTable, funcId);
    if (funcDesc == NULL) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Call to undefined func '%s'", funcId);
        throwSemanticError(errorMsg, IKS_ERROR_UNDECLARED);
    }
    int expectedParamsCount = countFuncParameters(funcDesc->params);

    if (paramsCount != expectedParamsCount) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Invalid call to '%s' - expected %d, got %d param(s)", funcId, expectedParamsCount, paramsCount);
        if (paramsCount > expectedParamsCount) {
            throwSemanticError(errorMsg, IKS_ERROR_EXCESS_ARGS);
        } else {
            throwSemanticError(errorMsg, IKS_ERROR_MISSING_ARGS);
        }
    }

    // Missing check params data type!!
    if (expectedParamsCount > 0) {
        currExpcParam = funcDesc->params;
        currParam = funcParamsNode;
        while (currExpcParam != NULL) {
            currExpcParamInfo = getASTNodeTokenInfo(currExpcParam);
            currParamNodeInfo = currParam->value;
            
            printf("> EXPECTED => dataType: %d\n", currExpcParamInfo->dataType);
            printf("> GOT      => dataType: %d\n", currParamNodeInfo->dataType);
            
            currExpcParam = currExpcParam->list_next;
            currParam = currParam->list_next;
        }
    }
}

/* Auxiliary */

void throwSemanticError(char *errorMsg, int errorCode) {
    printf("Semantic error: %s - on line %d\n", errorMsg, getLineNumber());
    exit(errorCode);
}

int inArray(int array[], int size, int val) {
    int i;
    for (i = 0; i < size; i++) {
        if (array[i] == val)
            return 1;
    }
    return 0;
}

