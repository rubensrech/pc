#ifndef __MISC_H
#define __MISC_H
#include <stdio.h>

typedef struct tokenInfo {
    int line;
    int type;
    union {
      int intVal;
      float floatVal;
      char charVal;
      int boolVal;
      char *strVal;  
    } value;
} TokenInfo;

int getLineNumber (void);
void yyerror (char const *mensagem);
void main_init (int argc, char **argv);
void main_finalize (void);

void comp_print_table (void);
TokenInfo *addSymbolsTable(int tokenType);
void debugPrintTokenInfo(TokenInfo *info);

#endif
