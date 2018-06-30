#include <stdio.h>
#include <gmodule.h>
#include <string.h>

#include "cc_exe.h"
#include "cc_cod.h"
#include "cc_table.h"
#include "cc_sem.h"

void generateFuncCode(comp_tree_t *funcNode) {
    comp_tree_t *blockNode = funcNode->first;
    AstNodeInfo *funcInfo = funcNode->value;
    AstNodeInfo *blockInfo = blockNode->value;
    TokenInfo *idToken = funcInfo->tokenInfo;
    char *funcName = idToken->lexeme;

    int paramsLocalVarsSize = getCurrFuncLocalVarsSize();
    resetLocalVarsOffset();

    GSList *codeList = NULL;

    if (strcmp(funcName, "main") != 0) {
        // Function is NOT main()
        GSList *saveRSP = generateSaveRSPCode(); 
        GSList *saveRARP = generateSaveRARPCode();
        GSList *createAR = generateCreateARCode(paramsLocalVarsSize);

        codeList = g_slist_concat(saveRSP, saveRARP);           // Save RSP + Save RARP
        codeList = g_slist_concat(codeList, createAR);          // Create AR
        codeList = g_slist_concat(codeList, blockInfo->code);   // [func block code]

        if (!checkFuncHasReturnCmd(funcNode)) {
            // If func does not have return command -> return on function end
            GSList *retSupportCode = generateReturnSupportCode();
            codeList = g_slist_concat(codeList, retSupportCode); // Destroy AR + jump-back
        }
    } else {
        // Function is main()
        // Create AR -> alloc local vars
        int codeSize = 30;
        char *allocLocalVarsCode = malloc(codeSize);
        snprintf(allocLocalVarsCode, codeSize, "addI rsp, %d => rsp\n", paramsLocalVarsSize);

        codeList = g_slist_append(codeList, allocLocalVarsCode);    // Create AR (local vars only)
        codeList = g_slist_concat(codeList, blockInfo->code);       // [func block code]
    }

    funcInfo->code = codeList;
}

GSList *generateSaveRSPCode() {
    int codeSize = 30;
    char *saveRSPCode = malloc(codeSize);
    snprintf(saveRSPCode, codeSize, "storeAI rsp => rsp, %d\n", AR_OFFSET_OLD_RSP);

    GSList *codeList = NULL;    
    codeList = g_slist_append(codeList, saveRSPCode);

    return codeList;
}

GSList *generateSaveRARPCode() {
    int codeSize = 35;
    char *saveRARPCode = malloc(codeSize);
    snprintf(saveRARPCode, codeSize, "storeAI rarp => rsp, %d\n", AR_OFFSET_OLD_RARP);

    GSList *codeList = NULL;    
    codeList = g_slist_append(codeList, saveRARPCode);

    return codeList;
}

GSList *generateCreateARCode(int paramsLocalVarsSize) {
    int codeSize = 40;
    char *setRARPCode = malloc(codeSize);
    char *setRSPCode = malloc(codeSize);

    int ARsize = AR_STATIC_SIZE + paramsLocalVarsSize;

    snprintf(setRARPCode, codeSize, "i2i rsp => rarp\n");
    snprintf(setRSPCode, codeSize, "addI rsp, %d => rsp\n", ARsize);

    GSList *codeList = NULL;    
    codeList = g_slist_append(codeList, setRARPCode);
    codeList = g_slist_append(codeList, setRSPCode);

    return codeList;
}

void generateReturnCmdCode(comp_tree_t *retNode) {
    comp_tree_t *expNode = retNode->first;
    AstNodeInfo *retInfo = retNode->value;
    AstNodeInfo *expInfo = expNode->value;

    int expResReg = expInfo->resultReg;
    int codeSize = 40;
    char *saveRetValCode = malloc(codeSize);

    snprintf(saveRetValCode, codeSize, "storeAI r%d => rarp, %d\n", expResReg, AR_OFFSET_RET_VAL);

    GSList *retSupportCode = generateReturnSupportCode();

    GSList *codeList = expInfo->code;
    codeList = g_slist_append(codeList, saveRetValCode);
    codeList = g_slist_concat(codeList, retSupportCode);

    retInfo->code = codeList;
}

GSList *generateReturnSupportCode() {
    int retAddrReg = generateTempReg();
    int oldSPReg = generateTempReg();
    int oldRARPReg = generateTempReg();

    int codeSize = 30;
    char *loadRetAddrCode = malloc(codeSize);
    char *loadOldSPCode = malloc(codeSize);
    char *loadOldRARPCode = malloc(codeSize);
    char *updateSPCode = malloc(codeSize);
    char *updateRARPCode = malloc(codeSize);
    char *jumpBackCode = malloc(codeSize);

    // > Destroy AR
    snprintf(loadRetAddrCode, codeSize, "loadAI rarp, %d => r%d\n", AR_OFFSET_RET_ADDR, retAddrReg);
    snprintf(loadOldSPCode, codeSize, "loadAI rarp, %d => r%d\n", AR_OFFSET_OLD_RSP, oldSPReg);
    snprintf(loadOldRARPCode, codeSize, "loadAI rarp, %d => r%d\n", AR_OFFSET_OLD_RARP, oldRARPReg);
    snprintf(updateSPCode, codeSize, "i2i r%d => rsp\n", oldSPReg);
    snprintf(updateRARPCode, codeSize, "i2i r%d => rarp\n", oldRARPReg);
    // > Transfer execution (jump back to caller function)
    snprintf(jumpBackCode, codeSize, "jump -> r%d\n", retAddrReg);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, loadRetAddrCode);
    codeList = g_slist_append(codeList, loadOldSPCode);
    codeList = g_slist_append(codeList, loadOldRARPCode);
    codeList = g_slist_append(codeList, updateSPCode);
    codeList = g_slist_append(codeList, updateRARPCode);
    codeList = g_slist_append(codeList, jumpBackCode);

    return codeList;
}