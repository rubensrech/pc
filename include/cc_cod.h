#include <gmodule.h>
#include "cc_ast.h"

// > Code list
void printCodeItem(gpointer codeItem, gpointer extra);
void printCodeList(GSList *codeList);
void printNodeCodeList(comp_tree_t *node);
void inheritCodeList(comp_tree_t *to, comp_tree_t *from);

// > General
void replace_str(char *str, char *orig, char *rep);
int getSizeOf(int type);
int generateTempReg();
char *remendo();
int generateLabel();

void cmdsCodeListConcat(comp_tree_t *cmd1, comp_tree_t *cmd2);

// > Code generating

void generateCode(comp_tree_t *node);

void generateLiteralCode(comp_tree_t *node);

void generateArithInvertCode(comp_tree_t *node);
void generateArithCode(comp_tree_t *node, const char *op);

void setNodeLocalVarOffset(comp_tree_t *idNode);
void setTokenLocalVarOffset(TokenInfo *idInfo);
void setTokenGlobalVarOffset(TokenInfo *idInfo);

void generateLoadVarCode(comp_tree_t *idNode);

GSList *getArrayAddrGeneratorCode(comp_tree_t *arrNode, int addrReg);
void generateLoadArrayVarCode(comp_tree_t *arrNode);

void generateAssignCode(comp_tree_t *node);
void generateSimpleVarAssignCode(comp_tree_t *node);
void generateArrayVarAssignCode(comp_tree_t *node);

void generateCompCode(comp_tree_t *node, const char *relOp);
void generateLogicCode(comp_tree_t *node, const char *op);
