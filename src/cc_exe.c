#include <stdio.h>
#include <gmodule.h>
#include <string.h>

#include "cc_exe.h"
#include "cc_cod.h"
#include "cc_table.h"
#include "cc_sem.h"

int currFuncVarsOffset = 0;

int getCurrFuncVarsOffset() {
    return currFuncVarsOffset;
}

void setCurrFuncVarsOffset(TokenInfo *funcIdToken) {
    char *funcName = funcIdToken->lexeme;

    if (strcmp(funcName, "main") == 0)
        currFuncVarsOffset = MAIN_AR_OFFSET_LOCVARS;
    else
        currFuncVarsOffset = AR_OFFSET_LOCVARS;
}

void generateFullCode(comp_tree_t *progNode) {
    comp_tree_t *codeNode = progNode->first;
    AstNodeInfo *progInfo = progNode->value;
    AstNodeInfo *codeInfo = codeNode->value;

    int codeSize = 30;
    char *setRARPCode = malloc(codeSize);
    char *setRSPCode = malloc(codeSize);
    char *setRBSSCode = malloc(codeSize);
    char *jumpToMainCode = malloc(codeSize);

    int codeInstructions = countCodeListInstructions(codeInfo->code);
    codeInstructions += 4; // Sum the 4 instructions belowmainFuncLabel
    int mainFuncLabel = getMainFuncLabel();

    snprintf(setRARPCode, codeSize, "loadI %d => rarp\n", DEFAULT_STACK_ADDR);
    snprintf(setRSPCode, codeSize, "loadI %d => rsp\n", DEFAULT_STACK_ADDR);
    snprintf(setRBSSCode, codeSize, "loadI %d => rbss\n", codeInstructions);
    snprintf(jumpToMainCode, codeSize, "jumpI -> L%d\n", mainFuncLabel);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, setRARPCode);
    codeList = g_slist_append(codeList, setRSPCode);
    codeList = g_slist_append(codeList, setRBSSCode);
    codeList = g_slist_append(codeList, jumpToMainCode);
    codeList = g_slist_concat(codeList, codeInfo->code);

    progInfo->code = codeList;
}

int countCodeListInstructions(GSList *codeList) {
    GSList *codeListItem = codeList;
    char *codeItem;
    int instructionsCount = 0;

    while (codeListItem != NULL) {
        codeItem = codeListItem->data;

        if (strchr(codeItem, '\n') != NULL)
            instructionsCount++;

        codeListItem = codeListItem->next;
    }

    return instructionsCount;
}

int getMainFuncLabel() {
    int funcLabel = getFuncLabel("main");
    
    if (funcLabel == -1) {
        fprintf(stderr, "ERROR: main function not defined\n");
        exit(-1);
    }
    
    return funcLabel;
}


void generateFuncCode(comp_tree_t *funcNode) {
    comp_tree_t *blockNode = funcNode->first;
    AstNodeInfo *funcInfo = funcNode->value;
    AstNodeInfo *blockInfo = blockNode->value;
    TokenInfo *idToken = funcInfo->tokenInfo;

    int paramsLocalVarsSize = getCurrFuncLocalVarsSize();

    char *funcName = idToken->lexeme;
    int funcLabel = getFuncLabel(funcName);
    char *funcLabelCode = generateLabelCode(funcLabel);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, funcLabelCode);

    if (strcmp(funcName, "main") != 0) {
        // Function is NOT main()
        GSList *saveRSP = generateSaveRSPCode(); 
        GSList *saveRARP = generateSaveRARPCode();
        GSList *createAR = generateCreateARCode(paramsLocalVarsSize);

        codeList = g_slist_concat(codeList, saveRSP);           // Save RSP + Save RARP
        codeList = g_slist_concat(codeList, saveRARP);          // Save RSP + Save RARP
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

    resetLocalVarsOffset();

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

void generateLabelForFunc(TokenInfo *funcIdToken) {
    char *funcName = funcIdToken->lexeme;
    int funcLabel = generateLabel();
    setFuncLabel(funcName, funcLabel);
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


void generateFuncCallCode(comp_tree_t *callNode) {
    comp_tree_t *idNode = callNode->first;
    AstNodeInfo *callInfo = callNode->value;
    TokenInfo *idToken = getTokenInfoFromIdNode(idNode);

    char *funcName = idToken->lexeme;
    int funcLabel = getFuncLabel(funcName);
    int funcRetValReg = generateTempReg();

    GSList *codeList = NULL;

    if (callNode->childnodes == 2) {
        // Func has arguments -> Passes arguments to AR
        codeList = generateFuncCallArgsPassCode(callNode->last);
    }

    GSList *retAddrCode = generateFuncCallRetAddrCode();
    GSList *jumpCode = generateFuncCallJumpCode(funcLabel);
    GSList *retCode = generateFuncCallLoadRetValCode(funcRetValReg);

    codeList = g_slist_concat(codeList, retAddrCode);
    codeList = g_slist_concat(codeList, jumpCode);
    
    codeList = g_slist_concat(codeList, retCode);

    callInfo->resultReg = funcRetValReg;
    callInfo->code = codeList;
}

GSList *generateFuncCallArgsPassCode(comp_tree_t *fstArgNode) {
    comp_tree_t *currArg = fstArgNode;
    AstNodeInfo *currArgInfo;

    int currArgsOffset = 0;
    int currArgSize;
    int currArgReg;
    int codeSize = 35;

    GSList *codeList = NULL;

    while (currArg != NULL) {
        currArgInfo = currArg->value;
        currArgSize = getSizeOf(currArgInfo->dataType);
        currArgReg = currArgInfo->resultReg;

        char *currArgStrCode = malloc(codeSize);
        snprintf(currArgStrCode, codeSize, "storeAI r%d => rsp, %d\n", currArgReg, AR_OFFSET_LOCVARS + currArgsOffset);

        codeList = g_slist_concat(codeList, currArgInfo->code);
        codeList = g_slist_append(codeList, currArgStrCode);

        currArgsOffset += currArgSize;
        currArg = currArg->list_next;
    }

    return codeList;
}

GSList *generateFuncCallRetAddrCode() {
    int codeSize = 30;
    int tempReg = generateTempReg();

    char *calcRetAddrCode = malloc(codeSize);
    char *passRetAddrCode = malloc(codeSize);

    snprintf(calcRetAddrCode, codeSize, "addI rpc, %d => r%d\n", RET_TREAT_OFFSET, tempReg);
    snprintf(passRetAddrCode, codeSize, "storeAI r%d => rsp, %d\n", tempReg, AR_OFFSET_RET_ADDR);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, calcRetAddrCode);
    codeList = g_slist_append(codeList, passRetAddrCode);

    return codeList;
}

GSList *generateFuncCallJumpCode(int funcLabel) {
    int codeSize = 30;
    char *jumpCode = malloc(codeSize);
    snprintf(jumpCode, codeSize, "jumpI -> L%d\n", funcLabel);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, jumpCode);

    return codeList;
}

GSList *generateFuncCallLoadRetValCode(int retValReg) {
    int codeSize = 30;
    char *loadRetValCode = malloc(codeSize);
    snprintf(loadRetValCode, codeSize, "loadAI rsp, %d => r%d\n", AR_OFFSET_RET_VAL, retValReg);

    GSList *codeList = NULL;
    codeList = g_slist_append(codeList, loadRetValCode);

    return codeList;
}