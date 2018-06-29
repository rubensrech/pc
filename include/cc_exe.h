#define AR_STATIC_SIZE     16

#define AR_OFFSET_RET_ADDR 0
#define AR_OFFSET_OLD_RSP  4
#define AR_OFFSET_OLD_RARP 8
#define AR_OFFSET_RET_VAL  12
#define AR_OFFSET_LOCVARS  16

#include "cc_tree.h"
#include "cc_ast.h"

void generateFuncCode(comp_tree_t *funcNode);
GSList *generateSaveRSPCode();
GSList *generateSaveRARPCode();
GSList *generateCreateARCode(int paramsLocalVarsSize);

void generateReturnCmdCode(comp_tree_t *retNode);