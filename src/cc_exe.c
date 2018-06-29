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

    GSList *codeList;

    if (strcmp(funcName, "main") != 0) {
        GSList *saveRSP = generateSaveRSPCode(); 
        GSList *saveRARP = generateSaveRARPCode();
        GSList *createAR = generateCreateARCode(paramsLocalVarsSize);

        codeList = g_slist_concat(saveRSP, saveRARP);           // Save RSP + Save RARP
        codeList = g_slist_concat(codeList, createAR);          // Create AR
        codeList = g_slist_concat(codeList, blockInfo->code);   // [func block code]

        printf("func name: %s\n", funcName);
        printCodeList(codeList);
        printf("\n\n");
    }

    
    // Pass return value
    // Destroy AR
    // Tranfer execution

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

    GSList *codeList = expInfo->code;
    codeList = g_slist_append(codeList, saveRetValCode);

    // Generate return code (Destroy RA, jump back)

    retInfo->code = codeList;
}