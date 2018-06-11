#include <stdlib.h>
#include <string.h>
#include "cc_cod.h"
#include "cc_sem.h"

int l_offset = 0;
int g_offset = 0;

int tempReg = 0;
int hole = 0;
int label = 0;

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
        case DATATYPE_BOOL: return 0;       // ??
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
    // Variables
    case AST_IDENTIFICADOR: generateLoadVarCode(node); break;
    case AST_VETOR_INDEXADO: generateLoadArrayVarCode(node); break;
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
    case AST_LOGICO_E: generateLogicCode(node, "and"); break;
    case AST_LOGICO_OU: generateLogicCode(node, "or"); break;
    // Control Flow
    case AST_IF_ELSE: generateIfCode(node); break;
    case AST_WHILE_DO: generateWhileCode(node); break;
    case AST_DO_WHILE: generateDoWhileCode(node); break;
    }
}

void generateLiteralCode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;

    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int reg = generateTempReg();
      
    switch (nodeInfo->dataType) {
    case DATATYPE_INT:
        snprintf(code, maxCodeSize, "loadI %s => r%d\n", nodeInfo->tokenInfo->lexeme, reg);
        break;
    default: break;
    }

    nodeInfo->code = g_slist_append(nodeInfo->code, code);
    nodeInfo->resultReg = reg;
}

// > Arithmetic

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

// > Variables

void allocNewGlobalVar(int dataType) {
    g_offset += getSizeOf(dataType);
}

void allocNewGlobalArray(int dataType, int length) {
    g_offset += getSizeOf(dataType) * length;
}

void allocNewLocalVar(int dataType) {
    l_offset += getSizeOf(dataType);
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

void generateLoadVarCode(comp_tree_t *idNode) {
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

// > Assignment

void generateAssignCode(comp_tree_t *node) {
    AstNodeInfo *destInfo = node->first->value;
    AstNodeInfo *sndChildInfo = node->first->next->value;

    if (destInfo->type == AST_IDENTIFICADOR) {
        if (sndChildInfo->type != AST_IDENTIFICADOR) {
            generateSimpleVarAssignCode(node);
        } else {
            // Assigning to user type var
        }
    } else if (destInfo->type == AST_VETOR_INDEXADO) {
        generateArrayVarAssignCode(node);
    }
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
    char *cbrCodeComma = malloc(2);
    char *falseHole = generateLabelHole();
    char *cbrCodeLineBreak = malloc(1);

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
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fstOpInfo = node->first->value;
    AstNodeInfo *sndOpInfo = node->last->value;

    int maxCodeSize = 10;
    int x = generateLabel();
    char *labelCode = generateLabelCode(x);

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

// > Control flow

void generateIfCode(comp_tree_t *node) {
    if (node->childnodes == 3) {
        generateIfElseCode(node);
        return;
    }
    
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = node->first->value;
    AstNodeInfo *ifInfo = node->last->value;

    int trueLabel = generateLabel();
    int nextLabel = generateLabel();

    char *trueLabelCode = generateLabelCode(trueLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);

    patchUpLabelHoles(expInfo->trueHoles, trueLabel);
    patchUpLabelHoles(expInfo->falseHoles, nextLabel);

    GSList *codeList = expInfo->code;   // S.code = B.code    
    codeList = g_slist_append(codeList, trueLabelCode); // S.code += "Ltrue: "
    codeList = g_slist_concat(codeList, ifInfo->code); // S.code += S1.code
    codeList = g_slist_append(codeList, nextLabelCode); // S.code += "Lnext: "

    nodeInfo->code = codeList;
}

void generateIfElseCode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = node->first->value;
    AstNodeInfo *ifInfo = node->first->next->value;
    AstNodeInfo *elseInfo = node->last->value;

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
    codeList = g_slist_concat(codeList, ifInfo->code); // S.code += S1.code
    codeList = g_slist_append(codeList, jmpCode); // S.code += "jumpI -> S.next"
    codeList = g_slist_append(codeList, falseLabelCode); // S.code += "Lfalse: "
    codeList = g_slist_concat(codeList, elseInfo->code); // S.code += S2.code
    codeList = g_slist_append(codeList, nextLabelCode); // S.code += "Lnext: "

    nodeInfo->code = codeList;
}

void generateWhileCode(comp_tree_t *node) {    
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *expInfo = node->first->value;
    AstNodeInfo *blockInfo = node->last->value;

    int beginLabel = generateLabel();
    int trueLabel = generateLabel();
    int nextLabel = generateLabel();

    char *beginLabelCode = generateLabelCode(beginLabel);
    char *trueLabelCode = generateLabelCode(trueLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);
    char *jmpCode = malloc(30);
    snprintf(jmpCode, 30, "jumpI -> L%d\n", beginLabel);

    patchUpLabelHoles(expInfo->trueHoles, trueLabel);
    patchUpLabelHoles(expInfo->falseHoles, nextLabel);

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
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *blockInfo = node->first->value;
    AstNodeInfo *expInfo = node->last->value;

    int beginLabel = generateLabel();
    int nextLabel = generateLabel();

    char *beginLabelCode = generateLabelCode(beginLabel);
    char *nextLabelCode = generateLabelCode(nextLabel);

    patchUpLabelHoles(expInfo->trueHoles, beginLabel);
    patchUpLabelHoles(expInfo->falseHoles, nextLabel);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, beginLabelCode);    // S.code = "Lbegin: "
    codeList = g_slist_concat(codeList, blockInfo->code);   // S.code += S1.code
    codeList = g_slist_concat(codeList, expInfo->code);     // S.code += B.code
    codeList = g_slist_append(codeList, nextLabelCode);     // S.code += "Lnext: "

    nodeInfo->code = codeList;
}