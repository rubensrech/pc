#define AR_STATIC_SIZE     16
#define RET_TREAT_OFFSET   3

#define DEFAULT_STACK_ADDR 1024

#define AR_OFFSET_RET_ADDR 0
#define AR_OFFSET_OLD_RSP  4
#define AR_OFFSET_OLD_RARP 8
#define AR_OFFSET_RET_VAL  12
#define AR_OFFSET_LOCVARS  16
#define MAIN_AR_OFFSET_LOCVARS  0

#include "cc_tree.h"
#include "cc_ast.h"

/*
 * Activation Record
 *      -------------------------
 * 0    | return address        |
 * 4    | old RSP               |
 * 8    | old RARP              |
 * 12   | return value          |
 * 16   | params../local vars.. |
 * ..   -------------------------
 */

void setCurrFuncVarsOffset(TokenInfo *funcIdToken);
int getCurrFuncVarsOffset();

void generateFullCode(comp_tree_t *progNode);
int countCodeListInstructions(GSList *codeList);
int getMainFuncLabel();

void generateFuncCode(comp_tree_t *funcNode);
GSList *generateSaveRSPCode();
GSList *generateSaveRARPCode();
GSList *generateCreateARCode(int paramsLocalVarsSize);

void generateLabelForFunc(TokenInfo *funcIdToken);

void generateReturnCmdCode(comp_tree_t *retNode);
GSList *generateReturnSupportCode();

void generateFuncCallCode(comp_tree_t *callNode);
GSList *generateFuncCallArgsPassCode(comp_tree_t *fstArgNode);
GSList *generateFuncCallRetAddrCode();
GSList *generateFuncCallJumpCode(int funcLabel);
GSList *generateFuncCallLoadRetValCode(int retValReg);