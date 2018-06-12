#include <stdlib.h>
#include <string.h>
#include "cc_sem.h"

ScopeInfo scopeInfo =   {
                            .isCurrScopeGlobal = 1,
                            .currScopeId = ""
                        };

PipeExpParseInfo pipeExpParseInfo = {
                                        .isParsingPipeExp = 0,
                                        .lastFuncCallRetType = DATATYPE_UNDEF
                                    };

comp_dict_t *funcTable;
comp_dict_t *userTypesTable;

/* Data type */

/* > Setters */
void setIdTokenDataType(TokenInfo *id, int dataType) {
    id->dataType = dataType;    
}

void setIdNodeTokenDataType(comp_tree_t *node, int dataType) {
    AstNodeInfo *nodeInfo = node->value;
    setIdTokenDataType(nodeInfo->tokenInfo, dataType);
}

void setNodeDataType(comp_tree_t *node, int dataType) {
    AstNodeInfo *nodeInfo = node->value;
    nodeInfo->dataType = dataType;
}

/* > Getters */
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

/* > Checks */
int checkDataTypeMatching(int dataType1, int dataType2, int shouldThrow) {
    int arithmeticDataTypes[2] = { DATATYPE_INT, DATATYPE_FLOAT };
    // Allows: int <= float, float <= int
    int floatIntConversion = inArray(arithmeticDataTypes, 2, dataType1) && inArray(arithmeticDataTypes, 2, dataType2);

    if (!floatIntConversion && (dataType1 != dataType2)) {
        if (shouldThrow) {
            throwSemanticError("Types mismatch 1", IKS_ERROR_WRONG_TYPE);
        }
        return 0; 
    } else {
        return 1;
    }
}

int checkArimExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2) {
    int arithmeticDataTypes[3] = { DATATYPE_INT, DATATYPE_FLOAT, DATATYPE_CHAR };

    int exp1DataType = getASTNodeDataType(exp1);
    // Use exp2 = NULL when only exp1 matters (example: - exp)
    int exp2DataType = (exp2 != NULL) ? getASTNodeDataType(exp2) : DATATYPE_INT;

    if (!inArray(arithmeticDataTypes, 3, exp1DataType) || !inArray(arithmeticDataTypes, 3, exp2DataType)) {
        throwSemanticError("Types mismatch 2", IKS_ERROR_WRONG_TYPE);
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
        throwSemanticError("Types mismatch 4", IKS_ERROR_WRONG_TYPE);
    } 
    
    return DATATYPE_BOOL;
}

int checkLogicExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2) {
    int exp1DataType = getASTNodeDataType(exp1);
    // Use exp2 = NULL when only exp1 matters (example: !exp)
    int exp2DataType = (exp2 != NULL) ? getASTNodeDataType(exp2) : DATATYPE_BOOL;

    if (exp1DataType != DATATYPE_BOOL || exp2DataType != DATATYPE_BOOL) {
        throwSemanticError("Types mismatch 3", IKS_ERROR_WRONG_TYPE);
    }

    return DATATYPE_BOOL;
}

void checkExpNodeDataTypeIsBool(comp_tree_t *exp) {
    int expDataType = getASTNodeDataType(exp);
    if (expDataType != DATATYPE_BOOL) {
        throwSemanticError("Expression type mismatch (expected bool)", IKS_ERROR_WRONG_TYPE);
    }
}

void checkExpNodeDataTypeIsInt(comp_tree_t *exp) {
    int expDataType = getASTNodeDataType(exp);
    if (expDataType != DATATYPE_INT) {
        throwSemanticError("Expression type mismatch (expected integer)", IKS_ERROR_WRONG_TYPE);
    }
}

/* Scope control */

void setCurrentScopeToGlobalScope() {
    scopeInfo.isCurrScopeGlobal = 1;
}

void createNewScope(char *id) {
    scopeInfo.isCurrScopeGlobal = 0;
    scopeInfo.currScopeId = id;
}

char *getCurrentScope() {
    if (scopeInfo.isCurrScopeGlobal) {
        return GLOBAL_SCOPE_ID;
    } else {
        return scopeInfo.currScopeId;
    }
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
            // Inherit/load global attributes
            id->scope = globalId->scope;

            id->idType = globalId->idType;
            id->dataType = globalId->dataType;
            id->userDataType = globalId->userDataType;
            id->offset = globalId->offset;
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

void checkIdUsedAsMultiple(int usedAs1, int usedAs2, TokenInfo *id) {
    char errorMsg[MAX_ERROR_MSG_SIZE];

    if (id->idType != usedAs1 && id->idType != usedAs2) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Wrong use for identifier '%s'", id->lexeme);
        throwSemanticError(errorMsg, IKS_ERROR_VARIABLE);
    }
}

void checkIdNodeUsedAsMultiple(int usedAs1, int usedAs2, comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    checkIdUsedAsMultiple(usedAs1, usedAs2, nodeInfo->tokenInfo);
}

TokenInfo *searchIdInGlobalScope(char *id) {
    return lookUpForIdInSymbolsTable(id, GLOBAL_SCOPE_ID);
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
    list_free(descriptor->params);
    free(descriptor);
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
    funcDesc->returnUserDataType = idInfo->userDataType;
    funcDesc->params = params;

    key = funcDesc->id;

    dict_put(funcTable, key, funcDesc);
}

int countFuncParameters(comp_tree_t *params) {
	return count_list_items(params);
}

void checkFuncCall(comp_tree_t *funcAST) {
    char errorMsg[MAX_ERROR_MSG_SIZE];

    comp_tree_t *funcIdNode = funcAST->first;
    AstNodeInfo *funcIdInfo = funcIdNode->value;
    char *funcId = funcIdInfo->tokenInfo->lexeme;

    comp_tree_t *funcParamsNode = funcAST->last;
    int hasParams = funcAST->childnodes > 1;
    int paramsCount = (hasParams) ? countFuncParameters(funcParamsNode) : 0;

    // Get func descriptor from hash table
    FuncDesc *funcDesc = dict_get(funcTable, funcId);
    if (funcDesc == NULL) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Call to undefined func '%s'", funcId);
        throwSemanticError(errorMsg, IKS_ERROR_UNDECLARED);
    }

    // PARAMETERS COUNT CHECK
    int expectedParamsCount = countFuncParameters(funcDesc->params);
    if (paramsCount != expectedParamsCount) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Invalid call to '%s' - expected %d, got %d param(s)", funcId, expectedParamsCount, paramsCount);
        if (paramsCount > expectedParamsCount) {
            throwSemanticError(errorMsg, IKS_ERROR_EXCESS_ARGS);
        } else {
            throwSemanticError(errorMsg, IKS_ERROR_MISSING_ARGS);
        }
    }

    // PARAMETERS DATA TYPE MATCHING CHECK
    comp_tree_t *currExpcParam, *currParam;
    TokenInfo *currExpcParamInfo;
    AstNodeInfo *currParamNodeInfo;
    int paramTypeMatches;
    int didSetDotParamType;
    int currParameterCount = 0;
    int dotArgsFound = 0;
    int nodeType;

    if (expectedParamsCount > 0) {
        currExpcParam = funcDesc->params;
        currParam = funcParamsNode;
        while (currExpcParam != NULL) {
            currParameterCount++;
            currExpcParamInfo = getASTNodeTokenInfo(currExpcParam);
            currParamNodeInfo = currParam->value;

            // PipeExp => Set '.'(dot) param dataType
            didSetDotParamType = setPipeExpDotParamDataType(currParam);
            nodeType = ((AstNodeInfo*)(currParam->value))->type;
            if (nodeType == AST_DOT_PARAM) {
                dotArgsFound++;   
            }
            
            // printf("> EXPECTED => dataType: %d\n", currExpcParamInfo->dataType);
            // printf("> GOT      => dataType: %d\n", currParamNodeInfo->dataType);
            paramTypeMatches = checkDataTypeMatching(currExpcParamInfo->dataType, currParamNodeInfo->dataType, 0);
            if (!paramTypeMatches) {
                if (didSetDotParamType) {
                    // PipeExp => invalid '.' param type
                    snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Invalid pipe expression with '%s' - type mismatch on param %d", funcId, currParameterCount);
                    throwSemanticError(errorMsg, IKS_ERROR_DOT_PARAM_TYPE);
                } else {
                    snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Invalid call to '%s' - type mismatch on param %d", funcId, currParameterCount);
                    throwSemanticError(errorMsg, IKS_ERROR_WRONG_TYPE_ARGS);
                }
            }
            
            currExpcParam = currExpcParam->list_next;
            currParam = currParam->list_next;
        }
        // End checking func arguments
        if (pipeExpParseInfo.isParsingPipeExp && dotArgsFound == 0) {
            snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Invalid call to '%s' - '.' not found in pipe expression", funcId);
            throwSemanticError(errorMsg, IKS_ERROR_DOT_PARAM_MISS);
        } 
    }
}

void checkFuncHasReturnCmd(comp_tree_t *funcNode) {
    char errorMsg[MAX_ERROR_MSG_SIZE];
    TokenInfo *funcIdInfo = ((AstNodeInfo *)funcNode->value)->tokenInfo;
    char *funcId = funcIdInfo->lexeme;

    // Search for return command inside func
    int commandType;
    comp_tree_t *commandNode = funcNode->first;
    comp_tree_t *returnNode = NULL;
    while (commandNode != NULL && returnNode == NULL) {
        commandType = ((AstNodeInfo *)commandNode->value)->type;
        if (commandType == AST_RETURN)
            returnNode = commandNode;
        commandNode = commandNode->list_next;
    }

    if (returnNode == NULL) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Function '%s' has no return command", funcId);
        if (ENABLE_RETURN_CHECK)
            throwSemanticError(errorMsg, IKS_ERROR_NO_RETURN);
    }
}

void checkFuncReturnDataType(comp_tree_t *returnNode) {
    char errorMsg[MAX_ERROR_MSG_SIZE];
    int retType = getASTNodeDataType(returnNode->first);

    // Get func expexted return type (based on funcTable and currScope)
    char *funcId = getCurrentScope();
    FuncDesc *funcDesc = dict_get(funcTable, funcId);
    if (funcDesc == NULL) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Call to undefined func '%s'", funcId);
        throwSemanticError(errorMsg, IKS_ERROR_UNDECLARED);
    }
    int expectedRetType = funcDesc->returnDataType;

    int retTypesMatch = checkDataTypeMatching(expectedRetType, retType, 0);
    if (!retTypesMatch) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Return type mismatch in function '%s'", funcId);
        if (ENABLE_RETURN_CHECK)
            throwSemanticError(errorMsg, IKS_ERROR_RETURN_TYPE);
    }

    // Check user data type matching
    if (expectedRetType == DATATYPE_USER_TYPE && retType == DATATYPE_USER_TYPE) {
        AstNodeInfo *expInfo = returnNode->first->value;
        char *retUserType = expInfo->userDataType;
        char *expectedRetUserType = funcDesc->returnUserDataType;
        if (strcmp(retUserType, expectedRetUserType) != 0) {
            snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Return type mismatch in function '%s': expected '%s', got '%s'", funcId, expectedRetUserType, retUserType);
            if (ENABLE_RETURN_CHECK)
                throwSemanticError(errorMsg, IKS_ERROR_RETURN_TYPE);
        }
    }  
}

/* Pipe Expressions */
void setCurrParsingPipeExp(int lastFuncCallRetType) {
    pipeExpParseInfo.isParsingPipeExp = 1;
    pipeExpParseInfo.lastFuncCallRetType = lastFuncCallRetType;
}

void endParsingPipeExp() {
    pipeExpParseInfo.isParsingPipeExp = 0;
    pipeExpParseInfo.lastFuncCallRetType = DATATYPE_UNDEF;
}

/*  
 * In case currParam is pipe expression '.'(dot) param
 *  -> Set its dataType based on last func call in pipe expression
 *  -> Return 1, if did set dotParam dataType, else return 0 
 */
int setPipeExpDotParamDataType(comp_tree_t *dotParamNode) {
    int nodeType = ((AstNodeInfo*)(dotParamNode->value))->type;

    int dotDataType = pipeExpParseInfo.lastFuncCallRetType;
    int expcParamDataType = ((AstNodeInfo*)(dotParamNode->value))->dataType;
    int paramTypeMatches;

    if (nodeType == AST_DOT_PARAM) {
        if (pipeExpParseInfo.isParsingPipeExp) {
            setNodeDataType(dotParamNode, dotDataType);
            return 1;
        } else {
            throwSemanticError("Dot param used out of pipe expression context", IKS_ERROR_WRONG_DOT_PARAM);
        }
    }
    return 0;
}

/* User types */

void initUserTypesTable() {
    userTypesTable = dict_new();
}

void freeUserTypesTable() {
    int i;
    struct comp_dict_item *entry, *next;
    UserTypeDesc *descriptor;

    for (i = 0; i < userTypesTable->size; i++) {
        entry = userTypesTable->data[i];
        while (entry != NULL) {
            next = entry->next;
            descriptor = dict_remove(userTypesTable, entry->key);
            freeUserTypeDescriptor(descriptor);
            entry = next;
        }
    }

    dict_free(userTypesTable);
}

void freeUserTypeDescriptor(UserTypeDesc *descriptor) {
    list_free(descriptor->fields);
    free(descriptor);
}

void printUserTypesTable() {
    int i;
    struct comp_dict_item *entry;
    UserTypeDesc *descriptor;

    printf("------ User Types Table ------\n");

    for (i = 0; i < userTypesTable->size; i++) {
        entry = userTypesTable->data[i];
        while (entry != NULL) {
            descriptor = entry->value;

            printf("> %s (fields: %d)\n", descriptor->typeName, countUserTypeFields(descriptor->fields));
    
            entry = entry->next;
        }
    }
}

int countUserTypeFields(comp_tree_t *fields) {
	return count_list_items(fields);
}

void insertUserTypeTable(TokenInfo *typeNameIdInfo, comp_tree_t *fields) {
    UserTypeDesc *userTypeDesc = malloc(sizeof(struct userTypeDesc));
    char *key;

    /// printf("Insert user type: %s (fields: %d)\n", typeNameIdInfo->lexeme, countUserTypeFields(fields));

    userTypeDesc->typeName = typeNameIdInfo->lexeme;
    userTypeDesc->fields = fields;

    key = userTypeDesc->typeName;

    dict_put(userTypesTable, key, userTypeDesc);
}

void checkUserTypeWasDeclared(TokenInfo *typeNameIdInfo) {
    char errorMsg[MAX_ERROR_MSG_SIZE];
    char *typeName = typeNameIdInfo->lexeme;
    
    UserTypeDesc *userTypeDesc = dict_get(userTypesTable, typeName);
    if (userTypeDesc == NULL) {
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Use of undeclared type '%s'", typeName);
        throwSemanticError(errorMsg, IKS_ERROR_UNDECLARED);
    }
};

void setIdTokenUserDataType(TokenInfo *id, TokenInfo *typeNameId) {
    id->userDataType = typeNameId->lexeme;
}

void setUserTypeFieldDataType(comp_tree_t *varNode, comp_tree_t *fieldNode) {
    char errorMsg[MAX_ERROR_MSG_SIZE];

    TokenInfo *varId = getTokenInfoFromIdNode(varNode);
    TokenInfo *fieldId = getTokenInfoFromIdNode(fieldNode);

    char *typeName = varId->userDataType;
    UserTypeDesc *userTypeDesc = dict_get(userTypesTable, typeName);

    char *fieldName = fieldId->lexeme;
    TokenInfo *typeDecField = lookUpFieldInUserTypeFields(fieldName, userTypeDesc->fields);
    if (typeDecField == NULL) {
        // Field not found in type declaration fields
        snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Use of unknown field '%s' in var '%s' of type '%s'", fieldName, varId->lexeme, typeName);
        throwSemanticError(errorMsg, IKS_ERROR_UNKNOWN_TYPE_FIELD);
    }

    // Set 'fieldId' ("fieldNode->tokenInfo") data type based on 'typeDecField'
    setIdTokenDataType(fieldId, typeDecField->dataType);
}

TokenInfo *lookUpFieldInUserTypeFields(char *fieldName, comp_tree_t *fields) {
    comp_tree_t *currField = fields;
    TokenInfo *currFieldInfo = getTokenInfoFromIdNode(currField);

    while (strcmp(fieldName, currFieldInfo->lexeme) != 0) {
        currField = currField->list_next;
        if (currField != NULL) {
            currFieldInfo = getTokenInfoFromIdNode(currField);
        } else {
            // Field not found
            break;
        }
    }

    if (strcmp(fieldName, currFieldInfo->lexeme) == 0) {
        return currFieldInfo;
    }

    return NULL;
}

void setNodeUserDataType(comp_tree_t *node, char *userDataType) {
    AstNodeInfo *nodeInfo = node->value;
    nodeInfo->userDataType = userDataType;
}

void checkUserDataTypeMatching(comp_tree_t *idNode, comp_tree_t *expNode) {
    char errorMsg[MAX_ERROR_MSG_SIZE];

    TokenInfo *id = getTokenInfoFromIdNode(idNode);
    AstNodeInfo *expInfo = expNode->value;

    int idDataType = id->dataType;
    int expDataType = getASTNodeDataType(expNode);

    checkDataTypeMatching(idDataType, expDataType, 1);

    // Data types matched
    if (idDataType == DATATYPE_USER_TYPE && expDataType == DATATYPE_USER_TYPE) {
        // Check user data type matching
        if (strcmp(id->userDataType, expInfo->userDataType) != 0) {
            snprintf(errorMsg, MAX_ERROR_MSG_SIZE, "Types mismatch: expected '%s', got '%s'", id->userDataType, expInfo->userDataType);
            throwSemanticError(errorMsg, IKS_ERROR_WRONG_TYPE);
        }
    }
}

/* Auxiliary */

void throwSemanticError(char *errorMsg, int errorCode) {
    fprintf(stderr, "Semantic error: %s - on line %d\n", errorMsg, getLineNumber());
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

TokenInfo *getTokenInfoFromIdNode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    return nodeInfo->tokenInfo;
}

