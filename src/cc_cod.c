#include <stdlib.h>
#include <string.h>
#include "cc_cod.h"
#include "cc_sem.h"

int l_offset = 0;
int g_offset = 0;

int tempReg = 0;

int generateTempReg() {
    return tempReg++;
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
    case AST_ATRIBUICAO: generateAssignCode(node);
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

    nodeInfo->code = code;
    nodeInfo->resultReg = reg;
    printf("%s", code);
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

    nodeInfo->code = code;
    nodeInfo->resultReg = resultReg;
    printf("%s", code);
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
    nodeInfo->code = code;
    nodeInfo->resultReg = resultReg;
    printf("%s", code);
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

    nodeInfo->code = code;
    nodeInfo->resultReg = resultReg;
    printf("%s", code);
}

void generateLoadArrayVarCode(comp_tree_t *arrNode) {
    comp_tree_t *idNode = arrNode->first;
    comp_tree_t *indexNode = idNode->next;
    AstNodeInfo *arrInfo = arrNode->value;
    AstNodeInfo *idInfo = idNode->value;
    AstNodeInfo *indexInfo = indexNode->value;
    TokenInfo *idToken = idInfo->tokenInfo;  

    int maxCodeSize = 90;
    char *code = malloc(maxCodeSize);
    int resultReg = generateTempReg();

    char multCode[30];
    int typeSize = getSizeOf(idToken->dataType);
    int indexReg = indexInfo->resultReg;
    int multReg = generateTempReg();
    // multReg = i * w => offset inside array
    snprintf(multCode, 30, "multI r%d, %d => r%d\n", indexReg, typeSize, multReg);

    char sumCode[30];
    int varBase = idToken->offset;
    int addrReg = generateTempReg();
    // addrReg = base + (i * w) => relative address (offset from rbss/rfp)
    snprintf(sumCode, 30, "addI r%d, %d => r%d\n", multReg, varBase, addrReg);
    

    if (strcmp(idToken->scope, "#GLOBAL#") == 0) {
        snprintf(code, maxCodeSize, "%s%sloadAO rbss, r%d => r%d\n", multCode, sumCode, addrReg, resultReg);
    } else {
        snprintf(code, maxCodeSize, "%s%sloadAO rfp, r%d => r%d\n", multCode, sumCode, addrReg, resultReg);
    }

    arrInfo->code = code;
    arrInfo->resultReg = resultReg;
    printf("%s", code);
}

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
        // Assigning to array
        // TO-DO
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

    nodeInfo->code = code;
    printf("%s", code);
}
