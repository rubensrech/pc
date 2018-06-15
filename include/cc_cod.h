#include <gmodule.h>
#include "cc_ast.h"

// > Code list
void printCodeItem(gpointer codeItem, gpointer extra);
void printCodeList(GSList *codeList);
void printNodeCodeList(comp_tree_t *node);
void inheritCodeList(comp_tree_t *to, comp_tree_t *from);

// > General
void freeCodeList(GSList *codeList);
void freeCodeListItem(gpointer data);

int getSizeOf(int type);
int generateTempReg();
char *generateLabelHole();
int generateLabel();
char *generateLabelCode(int labelNumber);
void patchUpLabelHoles(GSList *holes, int labelNumber);
void patchUpLabelHole(gpointer hole, gpointer label);
int patchUpBreakHoles(int labelNumber);
int patchUpContinueHoles(int labelNumber);

void cmdsCodeListConcat(comp_tree_t *cmd1, comp_tree_t *cmd2);

// > Code generating
void generateCode(comp_tree_t *node);
GSList *generateIfElseCode(comp_tree_t *expNode, GSList *trueCode, GSList *falseCode);
// Arithmetic
void generateLiteralCode(comp_tree_t *node);
void generateArithInvertCode(comp_tree_t *node);
void generateArithCode(comp_tree_t *node, const char *op);
// Variables
void allocNewGlobalVar(int size);
void allocNewGlobalArray(int itemSize, int length);
void allocNewLocalVar(int size);

void setNodeLocalVarOffset(comp_tree_t *idNode);
void setTokenLocalVarOffset(TokenInfo *idInfo);
void setTokenGlobalVarOffset(TokenInfo *idInfo);

void setTokenArrayLength(TokenInfo *idInfo, int length);

// Loads
void generateLoadVarCode(comp_tree_t *idNode);
void generateLoadSimpleVarCode(comp_tree_t *idNode);
void generateLoadUserVarCode(comp_tree_t *idNode);
void generateLoadUserVarFieldCode(comp_tree_t *userVarNode);
GSList *getArrayAddrGeneratorCode(comp_tree_t *arrNode, int addrReg);
void generateLoadArrayVarCode(comp_tree_t *arrNode);

// Assignment
void generateLoadBoolExpForAssignCode(comp_tree_t *expNode);

void generateAssignCode(comp_tree_t *node);
void generateUserVarAssignCode(comp_tree_t *node);
void generateUserVarFieldAssignCode(comp_tree_t *node);
void generateSimpleVarAssignCode(comp_tree_t *node);
void generateArrayVarAssignCode(comp_tree_t *node);
// > Logic
void generateLoadBoolVarLogicCode(comp_tree_t *node);
void generateCompCode(comp_tree_t *node, const char *relOp);
void generateLogicCode(comp_tree_t *node, const char *op);
// > Control flow
void generateIfCode(comp_tree_t *node);
void generateIfElseCommandCode(comp_tree_t *node);
void generateWhileCode(comp_tree_t *node);
void generateDoWhileCode(comp_tree_t *node);
void generateForCode(comp_tree_t *node);
GSList *generateForeachUpdateItVarCode(comp_tree_t *destIdNode, comp_tree_t *arrIdNode, int indexReg)
GSList *generateForeachUpdateItIndexCode(comp_tree_t *destIdNode, int indexReg);
void generateForeachCode(comp_tree_t *node);
void generateBreakContinueCode(comp_tree_t *node);


void test(comp_tree_t *node);