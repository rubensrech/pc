#include "cc_ast.h"

int getSizeOf(int type);
int generateTempReg();

void generateCode(comp_tree_t *node);
void generateLiteralCode(comp_tree_t *node);
void generateArithInvertCode(comp_tree_t *node);
void generateArithCode(comp_tree_t *node, const char *op);


void setNodeLocalVarOffset(comp_tree_t *idNode);
void setTokenLocalVarOffset(TokenInfo *idInfo);

void setTokenGlobalVarOffset(TokenInfo *idInfo);

void generateLoadVarCode(comp_tree_t *idNode);