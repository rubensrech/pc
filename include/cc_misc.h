#ifndef __MISC_H
#define __MISC_H
#include <stdio.h>

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
} TokenInfo;

typedef struct astNodeInfo {
  int type;
  TokenInfo *tokenInfo;
} AstNodeInfo;

int getLineNumber (void);
void yyerror (char const *mensagem);
void main_init (int argc, char **argv);
void main_finalize (void);

void comp_print_table (void);
TokenInfo *addSymbolsTable(int tokenType);
void freeTokenInfo(TokenInfo *info);
void freeSymbolsTable();

#define MAX_HASH_KEY_SIZE 1000

#endif
