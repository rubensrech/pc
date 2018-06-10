#include <stdlib.h>
#include <string.h>
#include "cc_cod.h"
#include "cc_sem.h"

int l_offset = 0;
int g_offset = 0;

int tempReg = 0;
int remendoNum = 0;
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

void replace_str(char *str, char *orig, char *rep) {
    char buffer[100];
    char *p;

    if (!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
        return;

    strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' start
    buffer[p-str] = '\0';

    sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
    
    strcpy(str, buffer);    
}

int generateTempReg() {
    return tempReg++;
}

int generateLabel() {
    return label++;
}

char *remendo() {
    char *tmp = malloc(10);
    snprintf(tmp, 10, "#%d", remendoNum);
    remendoNum++;
    return tmp;
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

    GSList *codeList = expInfo->code;
    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();
    int expReg = expInfo->resultReg;

    snprintf(code, maxCodeSize, "multI r%d, -1 => r%d\n", expReg, resultReg);
    codeList = g_slist_append(codeList, code);

    nodeInfo->code = codeList;
    nodeInfo->resultReg = resultReg;
}

void generateArithCode(comp_tree_t *node, const char *op) {
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fstOpInfo = node->first->value;
    AstNodeInfo *sndOpInfo = node->last->value;

    int maxCodeSize = 30;
    GSList *codeList = fstOpInfo->code;
    if (sndOpInfo->code != NULL) codeList = g_slist_concat(codeList, sndOpInfo->code);

    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();
    int fstOpReg = fstOpInfo->resultReg;
    int sndOpReg = sndOpInfo->resultReg;

    snprintf(code, maxCodeSize, "%s r%d, r%d => r%d\n", op, fstOpReg, sndOpReg, resultReg);
    codeList = g_slist_append(codeList, code);
    
    nodeInfo->code = codeList;
    nodeInfo->resultReg = resultReg;
}

// > Variables

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
    // codeList = g_slist_append(codeList, code);
}

GSList *getArrayAddrGeneratorCode(comp_tree_t *arrNode, int addrReg) {
    comp_tree_t *idNode = arrNode->first;
    comp_tree_t *indexNode = idNode->next;
    AstNodeInfo *idInfo = idNode->value;
    AstNodeInfo *indexInfo = indexNode->value;
    TokenInfo *idToken = idInfo->tokenInfo;  

    int maxCodeSize = 30;
    GSList *codeList = indexInfo->code;

    char *multCode = malloc(maxCodeSize);
    int typeSize = getSizeOf(idToken->dataType);
    int indexReg = indexInfo->resultReg;
    int multReg = generateTempReg();
    // multReg = i * w => offset inside array
    snprintf(multCode, maxCodeSize, "multI r%d, %d => r%d\n", indexReg, typeSize, multReg);
    codeList = g_slist_append(codeList, multCode);

    char *sumCode = malloc(maxCodeSize);
    int varBase = idToken->offset;
    // addrReg = base + (i * w) => relative address (offset from rbss/rfp)
    snprintf(sumCode, maxCodeSize, "addI r%d, %d => r%d\n", multReg, varBase, addrReg);
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

    GSList *codeList;
    
    int addrReg = generateTempReg();
    GSList *addrCode = getArrayAddrGeneratorCode(arrNode, addrReg);
    codeList = addrCode;

    if (strcmp(idToken->scope, "#GLOBAL#") == 0) {
        snprintf(code, maxCodeSize, "loadAO rbss, r%d => r%d\n", addrReg, resultReg);
    } else {
        snprintf(code, maxCodeSize, "loadAO rfp, r%d => r%d\n", addrReg, resultReg);
    }

    codeList = g_slist_append(codeList, code);
    // free(addrCode);

    arrInfo->code = codeList;
    arrInfo->resultReg = resultReg;
    // codeList = g_slist_append(codeList, code);
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
            // TO-DO
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

    GSList *codeList = expInfo->code;

    int maxCodeSize = 30; 
    char *code = malloc(maxCodeSize);

    int expValue = expInfo->resultReg;
    int varAddr = idToken->offset;
    
    if (strcmp(idToken->scope, "#GLOBAL#") == 0)
        snprintf(code, maxCodeSize, "storeAI r%d => rbss, %d\n", expValue, varAddr);
    else
        snprintf(code, maxCodeSize, "storeAI r%d => rfp, %d\n", expValue, varAddr);

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
    GSList *codeList;

    int expValueReg = expInfo->resultReg;
    int addrReg = generateTempReg();
    GSList *addrCode = getArrayAddrGeneratorCode(arrNode, addrReg);

    codeList = addrCode;
    codeList = g_slist_concat(codeList, expInfo->code);
    
    if (strcmp(idToken->scope, "#GLOBAL#") == 0)
        snprintf(code, maxCodeSize, "storeAO r%d => rbss, r%d\n", expValueReg, addrReg);
    else
        snprintf(code, maxCodeSize, "storeAO r%d => rfp, r%d\n", expValueReg, addrReg);

    codeList = g_slist_append(codeList, code);

    nodeInfo->code = codeList;
}

// > Logic

void generateCompCode(comp_tree_t *node, const char *relOp) {
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fstOpInfo = node->first->value;
    AstNodeInfo *sndOpInfo = node->last->value;

    int maxCodeSize = 30;
    GSList *codeList = fstOpInfo->code;
    codeList = g_slist_concat(codeList, sndOpInfo->code);

    char *cmpCode = malloc(maxCodeSize);
    int resultReg = generateTempReg();
    int fstOpReg = fstOpInfo->resultReg;
    int sndOpReg = sndOpInfo->resultReg;

    snprintf(cmpCode, maxCodeSize, "%s r%d, r%d -> r%d\n", relOp, fstOpReg, sndOpReg, resultReg);
    codeList = g_slist_append(codeList, cmpCode);

    char *cbrCode0 = malloc(maxCodeSize);
    snprintf(cbrCode0, maxCodeSize, "cbr r%d -> ", resultReg);
    codeList = g_slist_append(codeList, cbrCode0);

    char *x = remendo();
    codeList = g_slist_append(codeList, x);

    char *cbrCodeComma = malloc(2);
    strcpy(cbrCodeComma, ", ");
    codeList = g_slist_append(codeList, cbrCodeComma);

    char *y = remendo();
    codeList = g_slist_append(codeList, y);

    char *cbrCodeLineBreak = malloc(1);
    strcpy(cbrCodeLineBreak, "\n");
    codeList = g_slist_append(codeList, cbrCodeLineBreak);

    nodeInfo->code = codeList;
    nodeInfo->resultReg = resultReg;
    nodeInfo->trueList = g_slist_append(nodeInfo->trueList, GINT_TO_POINTER(x));
    nodeInfo->falseList = g_slist_append(nodeInfo->falseList, GINT_TO_POINTER(y));
}

void generateLogicCode(comp_tree_t *node, const char *op) {
    AstNodeInfo *nodeInfo = node->value;
    AstNodeInfo *fstOpInfo = node->first->value;
    AstNodeInfo *sndOpInfo = node->last->value;

    int maxCodeSize = 10;
    GSList *codeList = fstOpInfo->code;

    char *labelCode = malloc(maxCodeSize);
    int x = generateLabel();
    snprintf(labelCode, maxCodeSize, "L%d: ", x);
    codeList = g_slist_append(codeList, labelCode);

    codeList = g_slist_concat(codeList, sndOpInfo->code);

    printf(">>>>>>>>>>>>>%s\n", op);
    // strcpy(fstOpInfo->trueList->data, "L0");
    // printf("remendos TRUE: %s\n", fstOpInfo->trueList->data);
    printf("true: %d %d\n", g_slist_length(sndOpInfo->trueList), g_slist_length(fstOpInfo->trueList));
    printf("false: %d %d\n", g_slist_length(sndOpInfo->falseList), g_slist_length(fstOpInfo->falseList));
    printf(">>>>>>>>>>>>>\n");

    nodeInfo->code = codeList;    
}