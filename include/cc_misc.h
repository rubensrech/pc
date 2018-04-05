#ifndef __MISC_H
#define __MISC_H
#include <stdio.h>

typedef struct hashEntry {
    int line;
    int type;
    union {
      int intVal;
      float floatVal;
      char charVal;
      int boolVal;
      char *strVal;  
    } value;
} HashValue;

int getLineNumber (void);
void yyerror (char const *mensagem);
void main_init (int argc, char **argv);
void main_finalize (void);

void comp_print_table (void);
void addSymbolsTable(int tokenType);
void debugPrintHashValue(HashValue *value);

#endif
