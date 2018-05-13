#ifndef __CC_TABLE_H
#define __CC_TABLE_H

#include "cc_dict.h"

#define MAX_HASH_KEY_SIZE 1000

#define DATATYPE_UNDEF    0
#define DATATYPE_FLOAT    1
#define DATATYPE_INT      2
#define DATATYPE_BOOL     3
#define DATATYPE_STRING   4
#define DATATYPE_CHAR     5

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
    int scope;
} TokenInfo;

void initSymbolsTable();
TokenInfo *addSymbolsTable(int tokenType);
TokenInfo *lookUpForIdInSymbolsTable(char *id, int scope);
void freeTokenInfo(TokenInfo *info);
void freeSymbolsTable();
void printSymbolsTable();

#endif
