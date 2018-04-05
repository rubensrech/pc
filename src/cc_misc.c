#include <string.h>
#include <stdlib.h>
#include "cc_misc.h"
#include "cc_dict.h"
#include "main.h"

#define MAX_HASH_KEY_SIZE 1000

extern int num_lines;
extern char *yytext;

struct comp_dict *symbolsTable;

int getLineNumber(void) {
  return num_lines;
}

int comp_get_line_number (void)
{
  //implemente esta função
  return getLineNumber();
}

void yyerror (char const *mensagem)
{
  // fprintf (stderr, "%s\n", mensagem); //altere para que apareça a linha
  fprintf (stderr, "%s - line %d\n", mensagem, getLineNumber());
  exit(1);
}

void main_init (int argc, char **argv)
{
  //implemente esta função com rotinas de inicialização, se necessário

  // Initialize symbols table
  symbolsTable = dict_new();
}

void main_finalize (void)
{
  //implemente esta função com rotinas de inicialização, se necessário
  int i;
  struct comp_dict_item *entry, *next;
  HashValue *value;

  // comp_print_table();

  // Free symbols table
  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        next = entry->next;
        value = dict_remove(symbolsTable, entry->key);
        entry = next;
        free(value);
      }
  }
  dict_free(symbolsTable);
}

void comp_print_table (void)
{
  //para cada entrada na tabela de símbolos
  //Etapa 1: chame a função cc_dict_etapa_1_print_entrada
  //implemente esta função

  int i;
  struct comp_dict_item *entry, *tmp;
  HashValue *value;

  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        value = entry->value;

        //debugPrintHashValue(value);
        cc_dict_etapa_1_print_entrada(entry->key, value->line);
        
        entry = entry->next;
      }
  }
}

void removeQuotes(char *token) {
  int i;
  if (token[0] == '\'' || token[0] == '"') {
    token[strlen(token)-1] = '\0';
    for (i = 0; i < strlen(token); i++) {
      token[i] = token[i+1];
    }
  }
}

void addSymbolsTable(int tokenType) {
  char *token;
  char key[MAX_HASH_KEY_SIZE+1];
  HashValue *entryValue = malloc(sizeof(struct hashEntry)); 

  token = strdup(yytext);
  
  entryValue->line = num_lines;
  entryValue->type = tokenType;

  // Get token real value
  switch (tokenType) {
    case POA_LIT_STRING:
      removeQuotes(token);
      entryValue->value.strVal = token;
      break;
    case POA_LIT_CHAR:
      removeQuotes(token);
      entryValue->value.charVal = token[0];
      break;
    case POA_IDENT:
      entryValue->value.strVal = token;
      break;
    case POA_LIT_BOOL:
      entryValue->value.boolVal = !strcmp(token, "true");
      break;
    case POA_LIT_FLOAT:
      entryValue->value.floatVal = atof(token);
      break;
    case POA_LIT_INT:
      entryValue->value.intVal = atoi(token);
      break;
  }

  // Generate hash table key (TOKEN $$ TYPE)
  snprintf(key, MAX_HASH_KEY_SIZE, "%s $$ %d", token, tokenType);

  free(token);

  // Key is duplicated (strdup) inside dict_put function
  dict_put(symbolsTable, key, entryValue);
}

void debugPrintHashValue(HashValue *value) {
    printf(">> ---------------\n");
    printf(">> Line: %d\n", value->line);
    printf(">> Type: %d\n", value->type);
    switch (value->type) {
      case POA_LIT_STRING: printf(">> Value: %s\n", value->value.strVal); break;
      case POA_LIT_CHAR: printf(">> Value: %c\n", value->value.charVal); break;
      case POA_IDENT: printf(">> Value: %s\n", value->value.strVal); break;
      case POA_LIT_BOOL: printf(">> Value: %d\n", value->value.boolVal); break;
      case POA_LIT_FLOAT: printf(">> Value: %.2f\n", value->value.floatVal); break;
      case POA_LIT_INT: printf(">> Value: %d\n", value->value.intVal); break;
    }
}
