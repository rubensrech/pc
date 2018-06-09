#include <gmodule.h>
#include "cc_ast.h"

// > Code list
void printCodeItem(gpointer codeItem, gpointer extra);
void printFullCode();

// > General
void replace_str(char *str, char *orig, char *rep);
int getSizeOf(int type);
int generateTempReg();
int remendo();

// > Code generating

void generateCode(comp_tree_t *node);

void generateCompCode(comp_tree_t *node, const char *relOp);

void generateLiteralCode(comp_tree_t *node);

void generateArithInvertCode(comp_tree_t *node);
void generateArithCode(comp_tree_t *node, const char *op);

void setNodeLocalVarOffset(comp_tree_t *idNode);
void setTokenLocalVarOffset(TokenInfo *idInfo);
void setTokenGlobalVarOffset(TokenInfo *idInfo);

void generateLoadVarCode(comp_tree_t *idNode);

char *getArrayAddrGeneratorCode(comp_tree_t *arrNode, int addrReg);
void generateLoadArrayVarCode(comp_tree_t *arrNode);

void generateAssignCode(comp_tree_t *node);
void generateSimpleVarAssignCode(comp_tree_t *node);
void generateArrayVarAssignCode(comp_tree_t *node);