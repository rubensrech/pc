#ifndef __CC_TABLE_H
#define __CC_TABLE_H

#include "cc_dict.h"

#define MAX_HASH_KEY_SIZE 1000

typedef struct tokenInfo {
    int line;
    int type;
    char *lexeme;
    union {
      int intVal;
      float floatVal;
      char charVal;
      int boolVal;
      char *strVal;  
    } value;
    int dataType;
    char *userDataType;
    char *scope;
    int idType;
    char *userDataType;
} TokenInfo;

void initSymbolsTable();
TokenInfo *addSymbolsTable(int tokenType);
TokenInfo *lookUpForIdInSymbolsTable(char *id, char *scope);
void freeTokenInfo(TokenInfo *info);
void freeSymbolsTable();
void printSymbolsTable();

#endif
