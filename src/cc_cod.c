#include <stdlib.h>
#include "cc_cod.h"
#include "cc_sem.h"

int l_offset = 0;
int g_offset = 0;

int tempReg = 0;

int generateTempReg() {
    return tempReg++;
}

int getSizeOf(int type) {
    switch (type) {
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
    case AST_ARIM_INVERSAO: generateLiteralCode(node); break;
    // Arithmetic
    case AST_ARIM_SOMA: generateArimCode(node, "add"); break;
    case AST_ARIM_SUBTRACAO: generateArimCode(node, "sub"); break;
    case AST_ARIM_DIVISAO: generateArimCode(node, "div"); break;
    case AST_ARIM_MULTIPLICACAO: generateArimCode(node, "mult"); break;
    }
}

void generateLiteralCode(comp_tree_t *node) {
    AstNodeInfo *nodeInfo = node->value;
    int maxCodeSize = 30;
    char *code = malloc(maxCodeSize);
    int reg = generateTempReg();
    int negSign = 0;
    

    if (nodeInfo->type == AST_ARIM_INVERSAO) {
        negSign = 1;
        nodeInfo = node->first->value;
    }
    
    switch (nodeInfo->dataType) {
        case DATATYPE_INT:
            if (negSign)
                snprintf(code, maxCodeSize, "loadI -%s => r%d\n", nodeInfo->tokenInfo->lexeme, reg);
            else
                snprintf(code, maxCodeSize, "loadI %s => r%d\n", nodeInfo->tokenInfo->lexeme, reg);
            break;
        default: break;
    }

    nodeInfo->code = code;
    nodeInfo->resultReg = reg;
    printf("%s", code);
}

void generateArimCode(comp_tree_t *node, const char *op) {
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