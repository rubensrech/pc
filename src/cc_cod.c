#include <stdlib.h>
#include <string.h>
#include "cc_cod.h"
#include "cc_sem.h"

int l_offset = 0;
int g_offset = 0;

int tempReg = 0;
int hole = 0;
int label = 0;

GSList *breakHoles = NULL;
GSList *continueHoles = NULL;

// > Code list

void printCodeItem(gpointer codeItem, gpointer extra) {
    printf("%s", codeItem);
}

void printCodeList(GSList *codeList) {
     g_slist_foreach(codeList, printCodeItem, NULL);
}

void printNodeCodeList(comp_tree_t *node) {
    if (node == NULL) return;
    AstNodeInfo *nodeInfo = node->value;
    printCodeList(nodeInfo->code);
}

void inheritCodeList(comp_tree_t *to, comp_tree_t *from) {
    AstNodeInfo *toInfo = to->value;
    AstNodeInfo *fromInfo = from->value;
    
    toInfo->code = fromInfo->code;
}

// > General

// Called in cc_ast.c -> freeAST()
void freeCodeList(GSList *codeList) {
    g_slist_free_full(codeList, freeCodeListItem);
}

void freeCodeListItem(gpointer data) {
    char *codeStr = data;
    free(data);
}

int generateTempReg() {
    return tempReg++;
}

int generateLabel() {
    return label++;
}

char *generateLabelCode(int labelNumber) {
    char *labelCode = malloc(10);
    snprintf(labelCode, 10, "L%d: ", labelNumber);
    return labelCode;
}

char *generateLabelHole() {
    char *holeStr = malloc(10);
    snprintf(holeStr, 10, "#%d", hole);
    hole++;
    return holeStr;
}

int getSizeOf(int dataType) {
    switch (dataType) {
        case DATATYPE_FLOAT: return 4;
        case DATATYPE_INT: return 4;
        case DATATYPE_BOOL: return 1;       // ??
        case DATATYPE_STRING: return 0;     // ??
        case DATATYPE_CHAR: return 1;       // ??
        default: return 0;
    }
}

void patchUpLabelHoles(GSList *holes, int labelNumber) {
    char *labelStr = malloc(10);
    snprintf(labelStr, 10, "L%d", labelNumber);

    g_slist_foreach(holes, patchUpLabelHole, labelStr);

    free(labelStr);
}

void patchUpLabelHole(gpointer hole, gpointer label) {
    char *holePtr = hole;
    char *labelStr = label;
    strcpy(holePtr, labelStr);
}

int patchUpBreakHoles(int labelNumber) {
    int holesToPatchUp = g_slist_length(breakHoles);
    if (holesToPatchUp > 0) {
        patchUpLabelHoles(breakHoles, labelNumber);
        g_slist_free(breakHoles);
        breakHoles = NULL;
    }
    return holesToPatchUp;
}

int patchUpContinueHoles(int labelNumber) {
    int holesToPatchUp = g_slist_length(continueHoles);
    if (holesToPatchUp > 0) {
        patchUpLabelHoles(continueHoles, labelNumber);
        g_slist_free(continueHoles);
        continueHoles = NULL;
    }
    return holesToPatchUp;
} 

void cmdsCodeListConcat(comp_tree_t *cmd1, comp_tree_t *cmd2) {
    AstNodeInfo *cmd1Info = cmd1->value;
    AstNodeInfo *cmd2Info = cmd2->value;

    cmd1Info->code = g_slist_concat(cmd1Info->code, cmd2Info->code);
}

// > Code generating

void generateCode(comp_tree_t *node) {
    AstNodeInfo *info = node->value;
    switch (info->type) {
    // Literals
    case AST_LITERAL: generateLiteralCode(node); break;
    // Arithmetic
    case AST_ARIM_INVERSAO: generateArithInvertCode(node); break;
    case AST_ARIM_SOMA: generateArithCode(node, "add"); break;
    case AST_ARIM_SUBTRACAO: generateArithCode(node, "sub"); break;
    case AST_ARIM_DIVISAO: generateArithCode(node, "div"); break;
    case AST_ARIM_MULTIPLICACAO: generateArithCode(node, "mult"); break;
    // Shift
    case AST_SHIFT_LEFT: generateShiftCode(node, "lshift"); break;
    case AST_SHIFT_RIGHT: generateShiftCode(node, "rshift"); break;
    // Variables
    case AST_IDENTIFICADOR: generateLoadVarCode(node); break;
    case AST_VETOR_INDEXADO: generateLoadArrayVarCode(node); break;
    case AST_USER_VAR: generateLoadUserVarFieldCode(node); break;
    case AST_ATRIBUICAO: generateAssignCode(node); break;
    case AST_INICIALIZACAO: generateAssignCode(node); break;
    // Comparison
    case AST_LOGICO_COMP_IGUAL: generateCompCode(node, "cmp_EQ"); break;
    case AST_LOGICO_COMP_DIF: generateCompCode(node, "cmp_NE"); break;
    case AST_LOGICO_COMP_GE: generateCompCode(node, "cmp_GE"); break;
    case AST_LOGICO_COMP_LE: generateCompCode(node, "cmp_LE"); break;
    case AST_LOGICO_COMP_G: generateCompCode(node, "cmp_GT"); break;
    case AST_LOGICO_COMP_L: generateCompCode(node, "cmp_LT"); break;
    // Logic
    case AST_LOGICO_COMP_NEGACAO: generateLogicNegCode(node); break;
    case AST_LOGICO_E: generateLogicCode(node, "and"); break;
    case AST_LOGICO_OU: generateLogicCode(node, "or"); break;
    // Control Flow
    case AST_IF_ELSE: generateIfCode(node); break;
    case AST_WHILE_DO: generateWhileCode(node); break;
    case AST_DO_WHILE: generateDoWhileCode(node); break;
    case AST_FOR: generateForCode(node); break;
    case AST_FOREACH: generateForeachCode(node); break;
    case AST_BREAK: generateBreakContinueCode(node); break;
    case AST_CONTINUE: generateBreakContinueCode(node); break;
    }
}

GSList *generateIfElseCode(comp_tree_t *expNode, GSList *trueCode, GSList *falseCode) {
    AstNodeInfo *expInfo = expNode->value;

    int trueLabel = generateLabel();
    int falseLabel = generateLabel();
    int nextLabel = generateLabel();

    char *trueLabelCode = generateLabelCode(trueLabel);
    char *falseLabelCode = generateLabelCode(falseLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);
    char *jmpCode = malloc(30);
    snprintf(jmpCode, 30, "jumpI -> L%d\n", nextLabel);

    patchUpLabelHoles(expInfo->trueHoles, trueLabel);
    patchUpLabelHoles(expInfo->falseHoles, falseLabel);

    GSList *codeList = expInfo->code;   // S.code = B.code    
    codeList = g_slist_append(codeList, trueLabelCode); // S.code += "Ltrue: "
    codeList = g_slist_concat(codeList, trueCode); // S.code += S1.code
    codeList = g_slist_append(codeList, jmpCode); // S.code += "jumpI -> S.next"
    codeList = g_slist_append(codeList, falseLabelCode); // S.code += "Lfalse: "
    codeList = g_slist_concat(codeList, falseCode); // S.code += S2.code
    codeList = g_slist_append(codeList, nextLabelCode); // S.code += "Lnext: "

    return codeList;
}

// Arithmetic

void generateLiteralCode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    TokenInfo *tokenInfo = nodeInfo->tokenInfo;

    int codeSize = 30;
    char *code = malloc(codeSize);
    int reg = generateTempReg();
    char *boolHole, *breakLineCode;
      
    switch (nodeInfo->dataType) {
    case DATATYPE_INT:
        snprintf(code, codeSize, "loadI %d => r%d\n", tokenInfo->value.intVal, reg);
        break;
    case DATATYPE_CHAR:
        snprintf(code, codeSize, "loadI %d => r%d\n", tokenInfo->value.charVal, reg);
        break;
    case DATATYPE_BOOL:
        boolHole = generateLabelHole();
        breakLineCode = malloc(2);
        snprintf(code, codeSize, "jumpI -> ");
        strcpy(breakLineCode, "\n");
        nodeInfo->code = g_slist_append(nodeInfo->code, code);
        nodeInfo->code = g_slist_append(nodeInfo->code, boolHole);
        nodeInfo->code = g_slist_append(nodeInfo->code, breakLineCode);
        if (strcmp(tokenInfo->lexeme, "true") == 0)
            nodeInfo->trueHoles = g_slist_append(nodeInfo->trueHoles, boolHole);
        else
            nodeInfo->falseHoles = g_slist_append(nodeInfo->falseHoles, boolHole);
        return;
    default: break;
    }

    nodeInfo->code = g_slist_append(nodeInfo->code, code);
    nodeInfo->resultReg = reg;
}

void generateArithInvertCode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = node->first->value;

    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();
    int expReg = expInfo->resultReg;

    snprintf(code, maxCodeSize, "multI r%d, -1 => r%d\n", expReg, resultReg);

    GSList *codeList = expInfo->code;    
    codeList = g_slist_append(codeList, code);

    nodeInfo->code = codeList;
    nodeInfo->resultReg = resultReg;
}

void generateArithCode(comp_tree_t *node, const char *op) {
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fstOpInfo = node->first->value;
    AstNodeInfo *sndOpInfo = node->last->value;

    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();
    int fstOpReg = fstOpInfo->resultReg;
    int sndOpReg = sndOpInfo->resultReg;

    snprintf(code, maxCodeSize, "%s r%d, r%d => r%d\n", op, fstOpReg, sndOpReg, resultReg);

    GSList *codeList = fstOpInfo->code;
    codeList = g_slist_concat(codeList, sndOpInfo->code);    
    codeList = g_slist_append(codeList, code);

    nodeInfo->code = codeList;
    nodeInfo->resultReg = resultReg;
}

// Shift

void generateShiftCode(comp_tree_t *node, const char *shiftOp) {
    comp_tree_t *idNode = node->first;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *idInfo = idNode->value;
    AstNodeInfo *shAmtInfo = node->last->value;

    int varValReg;
    int shAmtReg = shAmtInfo->resultReg;
    int shiftResultReg = generateTempReg();
    char *shiftCode = malloc(30);

    // Generate code for load id
    generateCode(idNode);
    varValReg = idInfo->resultReg;

    // Generate code for var assignment
    comp_tree_t *shiftedValNode = makeASTNode(-1, NULL);
    comp_tree_t *assignNode = makeASTBinaryNode(AST_ATRIBUICAO, NULL, idNode, shiftedValNode);
    AstNodeInfo *shiftedValInfo = shiftedValNode->value;
    AstNodeInfo *assignInfo = assignNode->value;
    shiftedValInfo->resultReg = shiftResultReg;
    generateCode(assignNode);

    // Generate code for shift cmd
    snprintf(shiftCode, 30, "%s r%d, r%d => r%d\n", shiftOp, varValReg, shAmtReg, shiftResultReg);

    GSList *loadVarCode = idInfo->code;
    GSList *storeVarCode = assignInfo->code;

    GSList *codeList = shAmtInfo->code;
    codeList = g_slist_concat(codeList, loadVarCode);
    codeList = g_slist_append(codeList, shiftCode);
    codeList = g_slist_concat(codeList, storeVarCode);

    nodeInfo->code = codeList;
}

// Variables
// - Declarations/allocations
void allocNewGlobalVar(int size) {
    g_offset += size;
}

void allocNewGlobalArray(int itemSize, int length) {
    g_offset += itemSize * length;
}

void allocNewLocalVar(int size) {
    l_offset += size;
}

void setNodeLocalVarOffset(comp_tree_t *idNode) {
    TokenInfo *idInfo = getTokenInfoFromIdNode(idNode);
    idInfo->offset = l_offset;
}

void setTokenLocalVarOffset(TokenInfo *idInfo) {
    idInfo->offset = l_offset;
}

void setTokenGlobalVarOffset(TokenInfo *idInfo) {
    idInfo->offset = g_offset;
}

void setTokenArrayLength(TokenInfo *idInfo, int length) {
    idInfo->length = length;
}

// - Loads

void generateLoadVarCode(comp_tree_t *idNode) {
    int idType = getTokenInfoFromIdNode(idNode)->idType;
    switch (idType) {
    case VAR_ID: generateLoadSimpleVarCode(idNode); break;
    case USER_TYPE_ID: generateLoadUserVarCode(idNode); break;
    }
}

void generateLoadSimpleVarCode(comp_tree_t *idNode) {
    AstNodeInfo *nodeInfo = idNode->value;
    TokenInfo *idInfo = nodeInfo->tokenInfo;

    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();
    int varAddr = idInfo->offset;

    if (strcmp(idInfo->scope, "#GLOBAL#") == 0)
        snprintf(code, maxCodeSize, "loadAI rbss, %d => r%d\n", idInfo->offset, resultReg);
    else
        snprintf(code, maxCodeSize, "loadAI rfp, %d => r%d\n", idInfo->offset, resultReg);

    nodeInfo->code = g_slist_append(nodeInfo->code, code);
    nodeInfo->resultReg = resultReg;
}

// -> Array vars
GSList *getArrayAddrGeneratorCode(comp_tree_t *arrNode, int addrReg) {
    comp_tree_t *idNode = arrNode->first;
    comp_tree_t *indexNode = idNode->next;
    AstNodeInfo *idInfo = idNode->value;
    AstNodeInfo *indexInfo = indexNode->value;
    TokenInfo *idToken = idInfo->tokenInfo;  

    int maxCodeSize = 30;
    char *multCode = malloc(maxCodeSize);
    char *sumCode = malloc(maxCodeSize);
    int typeSize = getSizeOf(idToken->dataType);
    int indexReg = indexInfo->resultReg;
    int multReg = generateTempReg();
    int varBase = idToken->offset;

    // multReg = i * w => offset inside array
    snprintf(multCode, maxCodeSize, "multI r%d, %d => r%d\n", indexReg, typeSize, multReg);
    // addrReg = base + (i * w) => relative address (offset from rbss/rfp)
    snprintf(sumCode, maxCodeSize, "addI r%d, %d => r%d\n", multReg, varBase, addrReg);

    GSList *codeList = indexInfo->code;
    codeList = g_slist_append(codeList, multCode);
    codeList = g_slist_append(codeList, sumCode);
    
    return codeList;
}

void generateLoadArrayVarCode(comp_tree_t *arrNode) {
    comp_tree_t *idNode = arrNode->first;
    AstNodeInfo *arrInfo = arrNode->value;
    AstNodeInfo *idInfo = idNode->value;
    TokenInfo *idToken = idInfo->tokenInfo;  

    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();
    int addrReg = generateTempReg();

    if (strcmp(idToken->scope, "#GLOBAL#") == 0) {
        snprintf(code, maxCodeSize, "loadAO rbss, r%d => r%d\n", addrReg, resultReg);
    } else {
        snprintf(code, maxCodeSize, "loadAO rfp, r%d => r%d\n", addrReg, resultReg);
    }

    GSList *addrCode = getArrayAddrGeneratorCode(arrNode, addrReg);
    GSList *codeList;
    codeList = addrCode;
    codeList = g_slist_append(codeList, code);

    arrInfo->code = codeList;
    arrInfo->resultReg = resultReg;
}

// -> User vars
void generateLoadUserVarCode(comp_tree_t *idNode) {
    // Implemented right in assign func
    // -> generateUserVarAssignCode()
}

void generateLoadUserVarFieldCode(comp_tree_t *userVarNode) {
    comp_tree_t *varNode = userVarNode->first;
    comp_tree_t *fieldNode = userVarNode->last;
    AstNodeInfo *nodeInfo = userVarNode->value;
    TokenInfo *varToken = getTokenInfoFromIdNode(varNode);
    TokenInfo *fieldToken = getTokenInfoFromIdNode(fieldNode);

    int varOffset = varToken->offset;
    int fieldOffset = getUserTypeFieldOffset(varToken->userDataType, fieldToken->lexeme);
    int totalOffset = varOffset + fieldOffset;

    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();

    if (strcmp(varToken->scope, "#GLOBAL#") == 0)
        snprintf(code, maxCodeSize, "loadAI rbss, %d => r%d\n", totalOffset, resultReg);
    else
        snprintf(code, maxCodeSize, "loadAI rfp, %d => r%d\n", totalOffset, resultReg);

    nodeInfo->code = g_slist_append(nodeInfo->code, code);
    nodeInfo->resultReg = resultReg;
}

// Assignment

void generateLoadBoolExpForAssignCode(comp_tree_t *expNode) {
    AstNodeInfo *expInfo = expNode->value;

    // Vars must be loaded from memory
    if (expInfo->type == AST_IDENTIFICADOR) return;
    // Only generate code if exp is boolean
    if (expInfo->dataType != DATATYPE_BOOL) return;

    int maxCodeSize = 30;
    int resultReg = generateTempReg();
    char *trueCode = malloc(maxCodeSize);
    char *falseCode = malloc(maxCodeSize);
    GSList *trueCodeList = NULL;
    GSList *falseCodeList = NULL;

    snprintf(trueCode, maxCodeSize, "loadI 1 => r%d\n", resultReg);
    snprintf(falseCode, maxCodeSize, "loadI 0 => r%d\n", resultReg);

    trueCodeList = g_slist_append(trueCodeList, trueCode);
    falseCodeList = g_slist_append(falseCodeList, falseCode);

    GSList *codeList = generateIfElseCode(expNode, trueCodeList, falseCodeList);

    expInfo->code = codeList;
    expInfo->resultReg = resultReg;
}

void generateAssignCode(comp_tree_t *node) {
    comp_tree_t *expNode = node->last;
    AstNodeInfo *destInfo = node->first->value;
    int idType;

    generateLoadBoolExpForAssignCode(expNode);

    switch (destInfo->type) {
    case AST_IDENTIFICADOR:
        idType = destInfo->tokenInfo->idType;
        if (idType == VAR_ID)
            generateSimpleVarAssignCode(node);
        else if (idType == USER_TYPE_ID)
            generateUserVarAssignCode(node);
        break;
    case AST_VETOR_INDEXADO: generateArrayVarAssignCode(node); break;
    case AST_USER_VAR: generateUserVarFieldAssignCode(node); break;
    }
}

void generateUserVarFieldAssignCode(comp_tree_t *node) {
    comp_tree_t *destNode = node->first;
    comp_tree_t *varNode = destNode->first;
    comp_tree_t *fieldNode = destNode->last;
    comp_tree_t *expNode = node->last;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = expNode->value;
    TokenInfo *varToken = getTokenInfoFromIdNode(varNode);
    TokenInfo *fieldToken = getTokenInfoFromIdNode(fieldNode);

    int maxCodeSize = 30; 
    char *code = malloc(maxCodeSize);
    int expValue = expInfo->resultReg;

    int varOffset = varToken->offset;
    int fieldOffset = getUserTypeFieldOffset(varToken->userDataType, fieldToken->lexeme);
    int totalOffset = varOffset + fieldOffset;

    if (strcmp(varToken->scope, "#GLOBAL#") == 0)
        snprintf(code, maxCodeSize, "storeAI r%d => rbss, %d\n", expValue, totalOffset);
    else
        snprintf(code, maxCodeSize, "storeAI r%d => rfp, %d\n", expValue, totalOffset);

    GSList *codeList = expInfo->code;
    codeList = g_slist_append(codeList, code);

    nodeInfo->code = codeList;
}

void generateUserVarAssignCode(comp_tree_t *node) {
    comp_tree_t *fromNode = node->last;
    comp_tree_t *toNode = node->first;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fromInfo = fromNode->value;
    AstNodeInfo *toInfo = toNode->value;
    TokenInfo *fromToken = fromInfo->tokenInfo;
    TokenInfo *toToken = toInfo->tokenInfo;

    char *typeName = fromToken->userDataType;
    comp_tree_t *currField = getUserTypeFields(typeName);
    TokenInfo *currFieldInfo;
    int fromVarOffset = fromToken->offset;
    int toVarOffset = toToken->offset;

    int maxCodeSize = 30;
    char fromScopeReg[10], toScopeReg[10];
    char *loadCode, *storeCode;
    int tmpReg;
    int currFieldOffset = 0;
    int totalFromOffset, totalToOffset;

    if (strcmp(fromToken->scope, "#GLOBAL#") == 0)
        strcpy(fromScopeReg, "rbss");
    else
        strcpy(fromScopeReg, "rfp");

    if (strcmp(toToken->scope, "#GLOBAL#") == 0)
        strcpy(toScopeReg, "rbss");
    else
        strcpy(toScopeReg, "rfp");

    GSList *codeList = NULL;

    for (; currField != NULL; currField = currField->list_next) {
        currFieldInfo = getTokenInfoFromIdNode(currField);
        totalFromOffset = fromVarOffset + currFieldOffset;
        totalToOffset = toVarOffset + currFieldOffset;
        
        loadCode = malloc(maxCodeSize);
        storeCode = malloc(maxCodeSize);
        tmpReg = generateTempReg();
        
        snprintf(loadCode, maxCodeSize, "loadAI %s, %d => r%d\n", fromScopeReg, totalFromOffset, tmpReg);
        snprintf(storeCode, maxCodeSize, "storeAI r%d => %s, %d\n", tmpReg, toScopeReg, totalToOffset);
        codeList = g_slist_append(codeList, loadCode);
        codeList = g_slist_append(codeList, storeCode);

        currFieldOffset += getSizeOf(currFieldInfo->dataType);
    }

    nodeInfo->code = codeList;
}

void generateSimpleVarAssignCode(comp_tree_t *node) {
    comp_tree_t *idNode = node->first;
    comp_tree_t *expNode = idNode->next;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *idInfo = idNode->value;
    AstNodeInfo *expInfo = expNode->value;
    TokenInfo *idToken = idInfo->tokenInfo;

    int maxCodeSize = 30; 
    char *code = malloc(maxCodeSize);
    int expValue = expInfo->resultReg;
    int varAddr = idToken->offset;

    if (strcmp(idToken->scope, "#GLOBAL#") == 0)
        snprintf(code, maxCodeSize, "storeAI r%d => rbss, %d\n", expValue, varAddr);
    else
        snprintf(code, maxCodeSize, "storeAI r%d => rfp, %d\n", expValue, varAddr);

    GSList *codeList = expInfo->code;
    codeList = g_slist_append(codeList, code);

    nodeInfo->code = codeList;
}

void generateArrayVarAssignCode(comp_tree_t *node) {
    comp_tree_t *arrNode = node->first;
    comp_tree_t *idNode = arrNode->first;
    comp_tree_t *expNode = arrNode->next;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *idInfo = idNode->value;
    AstNodeInfo *expInfo = expNode->value;
    TokenInfo *idToken = idInfo->tokenInfo;

    int maxCodeSize = 30; 
    char *code = malloc(maxCodeSize);
    int expValueReg = expInfo->resultReg;
    int addrReg = generateTempReg();

    if (strcmp(idToken->scope, "#GLOBAL#") == 0)
        snprintf(code, maxCodeSize, "storeAO r%d => rbss, r%d\n", expValueReg, addrReg);
    else
        snprintf(code, maxCodeSize, "storeAO r%d => rfp, r%d\n", expValueReg, addrReg);

    GSList *addrCode = getArrayAddrGeneratorCode(arrNode, addrReg);

    GSList *codeList;
    codeList = addrCode;
    codeList = g_slist_concat(codeList, expInfo->code);
    codeList = g_slist_append(codeList, code);

    nodeInfo->code = codeList;
}

// > Logic

void generateLoadBoolVarLogicCode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
   
    if (nodeInfo->type != AST_IDENTIFICADOR) return;

    int maxCodeSize = 30;
    int varReg = nodeInfo->resultReg;
    
    char *cmpCode = malloc(maxCodeSize);
    char *cbrCode = malloc(maxCodeSize);
    char *trueHole = generateLabelHole();
    char *cbrCodeComma = malloc(3);
    char *falseHole = generateLabelHole();
    char *cbrCodeLineBreak = malloc(2);

    snprintf(cbrCode, maxCodeSize, "cbr r%d -> ", varReg);
    strcpy(cbrCodeComma, ", ");
    strcpy(cbrCodeLineBreak, "\n");

    GSList *codeList = nodeInfo->code;
    codeList = g_slist_append(codeList, cbrCode);
    codeList = g_slist_append(codeList, trueHole);
    codeList = g_slist_append(codeList, cbrCodeComma);
    codeList = g_slist_append(codeList, falseHole);
    codeList = g_slist_append(codeList, cbrCodeLineBreak);

    nodeInfo->code = codeList;
    nodeInfo->trueHoles = g_slist_append(nodeInfo->trueHoles, trueHole);
    nodeInfo->falseHoles = g_slist_append(nodeInfo->falseHoles, falseHole);
}

void generateCompCode(comp_tree_t *node, const char *relOp) {
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fstOpInfo = node->first->value;
    AstNodeInfo *sndOpInfo = node->last->value;

    int maxCodeSize = 30;
    int resultReg = generateTempReg();
    int fstOpReg = fstOpInfo->resultReg;
    int sndOpReg = sndOpInfo->resultReg;
    
    char *cmpCode = malloc(maxCodeSize);
    char *cbrCode0 = malloc(maxCodeSize);
    char *trueHole = generateLabelHole();
    char *cbrCodeComma = malloc(3);
    char *falseHole = generateLabelHole();
    char *cbrCodeLineBreak = malloc(2);

    snprintf(cmpCode, maxCodeSize, "%s r%d, r%d -> r%d\n", relOp, fstOpReg, sndOpReg, resultReg);
    snprintf(cbrCode0, maxCodeSize, "cbr r%d -> ", resultReg);
    strcpy(cbrCodeComma, ", ");
    strcpy(cbrCodeLineBreak, "\n");

    GSList *codeList = fstOpInfo->code;
    codeList = g_slist_concat(codeList, sndOpInfo->code);
    codeList = g_slist_append(codeList, cmpCode);
    codeList = g_slist_append(codeList, cbrCode0);
    codeList = g_slist_append(codeList, trueHole);
    codeList = g_slist_append(codeList, cbrCodeComma);
    codeList = g_slist_append(codeList, falseHole);
    codeList = g_slist_append(codeList, cbrCodeLineBreak);

    nodeInfo->code = codeList;
    nodeInfo->resultReg = resultReg;
    nodeInfo->trueHoles = g_slist_append(nodeInfo->trueHoles, trueHole);
    nodeInfo->falseHoles = g_slist_append(nodeInfo->falseHoles, falseHole);
}

void generateLogicCode(comp_tree_t *node, const char *op) {
    comp_tree_t *fstOpNode = node->first;
    comp_tree_t *sndOpNode = node->last;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fstOpInfo = fstOpNode->value;
    AstNodeInfo *sndOpInfo = sndOpNode->value;

    int maxCodeSize = 10;
    int x = generateLabel();
    char *labelCode = generateLabelCode(x);

    generateLoadBoolVarLogicCode(fstOpNode);
    generateLoadBoolVarLogicCode(sndOpNode);

    GSList *codeList = fstOpInfo->code; // B.code = B1.code
    codeList = g_slist_append(codeList, labelCode); // B.code += "Lx: "
    codeList = g_slist_concat(codeList, sndOpInfo->code); // B.code += B2.code
    
    switch (nodeInfo->type) {
    case AST_LOGICO_E:
        patchUpLabelHoles(fstOpInfo->trueHoles, x);
        nodeInfo->trueHoles = sndOpInfo->trueHoles; // B.trueHoles = B2.trueHoles
        nodeInfo->falseHoles = g_slist_concat(fstOpInfo->falseHoles, sndOpInfo->falseHoles); // B.falseHoles = concat(B1.falseHoles, B2.falseHoles)
        break;
    case AST_LOGICO_OU:
        patchUpLabelHoles(fstOpInfo->falseHoles, x);
        nodeInfo->falseHoles = sndOpInfo->falseHoles; // B.falseHoles = B2.falseHoles
        nodeInfo->trueHoles = g_slist_concat(fstOpInfo->trueHoles, sndOpInfo->trueHoles); // B.trueHoles = concat(B1.trueHoles, B2.trueHoles)
        break;
    default: break;
    }
    
    nodeInfo->code = codeList;    
}

void generateLogicNegCode(comp_tree_t *node) {
    comp_tree_t *expNode = node->first;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = expNode->value;

    generateLoadBoolVarLogicCode(expNode);

    // Swap holes lists
    nodeInfo->code = expInfo->code;
    nodeInfo->trueHoles = expInfo->falseHoles;
    nodeInfo->falseHoles = expInfo->trueHoles;
}

// > Control flow

void generateIfCode(comp_tree_t *node) {
    if (node->childnodes == 3) {
        generateIfElseCommandCode(node);
        return;
    }
    
    comp_tree_t *expNode = node->first;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = expNode->value;
    AstNodeInfo *ifInfo = node->last->value;

    int trueLabel = generateLabel();
    int nextLabel = generateLabel();

    char *trueLabelCode = generateLabelCode(trueLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);

    generateLoadBoolVarLogicCode(expNode);

    patchUpLabelHoles(expInfo->trueHoles, trueLabel);
    patchUpLabelHoles(expInfo->falseHoles, nextLabel);

    GSList *codeList = expInfo->code;   // S.code = B.code    
    codeList = g_slist_append(codeList, trueLabelCode); // S.code += "Ltrue: "
    codeList = g_slist_concat(codeList, ifInfo->code); // S.code += S1.code
    codeList = g_slist_append(codeList, nextLabelCode); // S.code += "Lnext: "

    nodeInfo->code = codeList;
}

void generateIfElseCommandCode(comp_tree_t *node) {
    comp_tree_t *expNode = node->first;
    comp_tree_t *trueBlockNode = node->first->next;
    comp_tree_t *falseBlockNode = node->last;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *trueBlockInfo = trueBlockNode->value;
    AstNodeInfo *falseBlockInfo = falseBlockNode->value;

    generateLoadBoolVarLogicCode(expNode);

    GSList *codeList = generateIfElseCode(expNode, trueBlockInfo->code, falseBlockInfo->code); 
    nodeInfo->code = codeList;
}

void generateWhileCode(comp_tree_t *node) {  
    comp_tree_t *expNode = node->first;  
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = expNode->value;
    AstNodeInfo *blockInfo = node->last->value;

    int beginLabel = generateLabel();
    int trueLabel = generateLabel();
    int nextLabel = generateLabel();

    char *beginLabelCode = generateLabelCode(beginLabel);
    char *trueLabelCode = generateLabelCode(trueLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);
    char *jmpCode = malloc(30);
    snprintf(jmpCode, 30, "jumpI -> L%d\n", beginLabel);

    generateLoadBoolVarLogicCode(expNode);

    patchUpLabelHoles(expInfo->trueHoles, trueLabel);
    patchUpLabelHoles(expInfo->falseHoles, nextLabel);
    patchUpBreakHoles(nextLabel);
    patchUpContinueHoles(beginLabel);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, beginLabelCode);    // S.code = "Lbegin: "
    codeList = g_slist_concat(codeList, expInfo->code);     // S.code += B.code    
    codeList = g_slist_append(codeList, trueLabelCode);     // S.code += "Ltrue: "
    codeList = g_slist_concat(codeList, blockInfo->code);   // S.code += S1.code
    codeList = g_slist_append(codeList, jmpCode);           // S.code += "jumpI -> Lbegin"
    codeList = g_slist_append(codeList, nextLabelCode);     // S.code += "Lnext: "

    nodeInfo->code = codeList;
}

void generateDoWhileCode(comp_tree_t *node) {
    comp_tree_t *expNode = node->last; 
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *blockInfo = node->first->value;
    AstNodeInfo *expInfo = expNode->value;

    int beginLabel = generateLabel();
    int nextLabel = generateLabel();
    int continueLabel;
    int hasContinueJmp = g_slist_length(continueHoles);

    char *beginLabelCode = generateLabelCode(beginLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);
    char *continueLabelCode;

    generateLoadBoolVarLogicCode(expNode);

    patchUpLabelHoles(expInfo->trueHoles, beginLabel);
    patchUpLabelHoles(expInfo->falseHoles, nextLabel);
    patchUpBreakHoles(nextLabel);

    if (hasContinueJmp) {
        continueLabel = generateLabel();
        continueLabelCode = generateLabelCode(continueLabel);
        patchUpContinueHoles(continueLabel);
    }

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, beginLabelCode);    // S.code = "Lbegin: "
    codeList = g_slist_concat(codeList, blockInfo->code);   // S.code += S1.code
    if (hasContinueJmp) codeList = g_slist_append(codeList, continueLabelCode);    // S.code = "Lcontinue: "
    codeList = g_slist_concat(codeList, expInfo->code);     // S.code += B.code
    codeList = g_slist_append(codeList, nextLabelCode);     // S.code += "Lnext: "

    nodeInfo->code = codeList;
}

void generateForCode(comp_tree_t *node) {
    comp_tree_t *expNode = node->first->next; 
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *cmds1Info = node->first->value;              // 1st child
    AstNodeInfo *expInfo = expNode->value;                    // 2nd child
    AstNodeInfo *cmds2Info = node->first->next->next->value;  // 3rd child
    AstNodeInfo *blockInfo = node->last->value;               // 4th child (last)

    int beginLabel = generateLabel();
    int trueLabel = generateLabel();
    int nextLabel = generateLabel();
    int continueLabel;
    int hasContinueJmp = g_slist_length(continueHoles);

    char *beginLabelCode = generateLabelCode(beginLabel);
    char *trueLabelCode = generateLabelCode(trueLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);
    char *jmpCode = malloc(30);
    snprintf(jmpCode, 30, "jumpI -> L%d\n", beginLabel);
    char *continueLabelCode;

    generateLoadBoolVarLogicCode(expNode);

    patchUpLabelHoles(expInfo->trueHoles, trueLabel);
    patchUpLabelHoles(expInfo->falseHoles, nextLabel);
    patchUpBreakHoles(nextLabel);

    if (hasContinueJmp) {
        continueLabel = generateLabel();
        continueLabelCode = generateLabelCode(continueLabel);
        patchUpContinueHoles(continueLabel);
    }

    GSList *codeList = cmds1Info->code;                     // S.code = cmds1.code
    codeList = g_slist_append(codeList, beginLabelCode);    // S.code += "Lbegin: "
    codeList = g_slist_concat(codeList, expInfo->code);     // S.code += B.code
    codeList = g_slist_append(codeList, trueLabelCode);     // S.code += "Ltrue: "
    codeList = g_slist_concat(codeList, blockInfo->code);   // S.code += block.code
    if (hasContinueJmp) codeList = g_slist_append(codeList, continueLabelCode);    // S.code = "Lcontinue: "
    codeList = g_slist_concat(codeList, cmds2Info->code);   // S.code += cmds2.code
    codeList = g_slist_append(codeList, jmpCode);           // S.code += "jumpI -> Lbegin"
    codeList = g_slist_append(codeList, nextLabelCode);     // S.code += "Lnext: "

    nodeInfo->code = codeList;
}

GSList *generateForeachUpdateItVarCode(comp_tree_t *destIdNode, comp_tree_t *arrIdNode, int indexReg) {
    comp_tree_t *arrIndexNode = makeASTNode(-1, NULL);
    comp_tree_t *arrNode = makeASTBinaryNode(AST_VETOR_INDEXADO, NULL, arrIdNode, arrIndexNode);
    comp_tree_t *assignNode = makeASTBinaryNode(AST_ATRIBUICAO, NULL, destIdNode, arrNode);
    AstNodeInfo *arrIndexInfo = arrIndexNode->value;
    AstNodeInfo *assignInfo = assignNode->value;

    arrIndexInfo->resultReg = indexReg;
    generateCode(arrNode);
    generateCode(assignNode);

    return assignInfo->code;
}

GSList *generateForeachUpdateItIndexCode(comp_tree_t *destIdNode, int indexReg) {
    comp_tree_t *indexNode = makeASTNode(-1, NULL);
    comp_tree_t *assignNode = makeASTBinaryNode(AST_ATRIBUICAO, NULL, destIdNode, indexNode);
    AstNodeInfo *indexInfo = indexNode->value;
    AstNodeInfo *assignInfo = assignNode->value;

    indexInfo->resultReg = indexReg;
    generateCode(assignNode);

    return assignInfo->code;
}

void generateForeachCode(comp_tree_t *node) {
    comp_tree_t *arrIdNode = node->first;
    comp_tree_t *itIndexIdNode = node->first->next;
    comp_tree_t *itVarIdNode = itIndexIdNode->list_next;
    comp_tree_t *blockNode = node->last;
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *blockInfo = blockNode->value;
    TokenInfo *arrIdToken = getTokenInfoFromIdNode(arrIdNode);

    int codeSize = 30;
    int arrayLength = arrIdToken->length;
    int indexReg = generateTempReg();
    int arrLengthReg = generateTempReg();
    int cmpReg = generateTempReg();
    int beginLabel = generateLabel();
    int trueLabel = generateLabel();
    int nextLabel = generateLabel();

    char *initIndexRegCode = malloc(codeSize);
    char *initArrayLengthRegCode = malloc(codeSize);
    char *cmpCode = malloc(codeSize);
    char *cbrCode = malloc(codeSize);
    char *beginLabelCode = generateLabelCode(beginLabel);
    char *trueLabelCode = generateLabelCode(trueLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);
    char *incCode = malloc(codeSize);
    char *jmpCode = malloc(codeSize);

    snprintf(initIndexRegCode, codeSize, "loadI 0 => r%d\n", indexReg);
    snprintf(initArrayLengthRegCode, codeSize, "loadI %d => r%d\n", arrayLength, arrLengthReg);
    snprintf(cmpCode, codeSize, "cmp_EQ r%d, r%d => r%d\n", indexReg, arrLengthReg, cmpReg);
    snprintf(cbrCode, codeSize, "cbr r%d -> L%d, L%d\n", cmpReg, nextLabel, trueLabel);
    snprintf(incCode, codeSize, "addI r%d, 1 => r%d\n", indexReg, indexReg);
    snprintf(jmpCode, codeSize, "jumpI -> L%d\n", beginLabel);

    GSList *updateItIndexCode = generateForeachUpdateItIndexCode(itIndexIdNode, indexReg);
    GSList *updateItVarCode = generateForeachUpdateItVarCode(itVarIdNode, arrIdNode, indexReg);
    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, initIndexRegCode);
    codeList = g_slist_append(codeList, initArrayLengthRegCode);
    codeList = g_slist_append(codeList, beginLabelCode);
    codeList = g_slist_append(codeList, cmpCode);
    codeList = g_slist_append(codeList, cbrCode);
    codeList = g_slist_append(codeList, trueLabelCode);
    codeList = g_slist_concat(codeList, updateItIndexCode);
    codeList = g_slist_concat(codeList, updateItVarCode);
    codeList = g_slist_concat(codeList, blockInfo->code);
    codeList = g_slist_append(codeList, incCode);
    codeList = g_slist_append(codeList, jmpCode);
    codeList = g_slist_append(codeList, nextLabelCode);

    nodeInfo->code = codeList;
}

void generateBreakContinueCode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    
    int maxCodeSize = 20;
    char *jmpCode = malloc(maxCodeSize);
    char *jmpLabelHole = generateLabelHole();
    char *lineBreak = malloc(2);

    snprintf(jmpCode, maxCodeSize, "jumpI -> ");
    strcpy(lineBreak, "\n");

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, jmpCode);
    codeList = g_slist_append(codeList, jmpLabelHole);
    codeList = g_slist_append(codeList, lineBreak);
    
    switch (nodeInfo->type) {
    case AST_BREAK: breakHoles = g_slist_append(breakHoles, jmpLabelHole); break;
    case AST_CONTINUE: continueHoles = g_slist_append(continueHoles, jmpLabelHole); break;
    default: return;
    }
    nodeInfo->code = codeList;
}




void test(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    printf("FUNC: breakHoles: %d\n", g_slist_length(breakHoles));
}