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
  TokenInfo *info;

  // comp_print_table();

  // Free symbols table
  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        next = entry->next;
        info = dict_remove(symbolsTable, entry->key);
        entry = next;
        free(info);
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
  struct comp_dict_item *entry;
  TokenInfo *info;
  char token[MAX_HASH_KEY_SIZE];

  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        info = entry->value;

        sscanf(entry->key, "%s $$ %*d", token);
        cc_dict_etapa_2_print_entrada(token, info->line, info->type);
        
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

TokenInfo *addSymbolsTable(int tokenType) {
  TokenInfo *info = malloc(sizeof(struct tokenInfo)); 
  char key[MAX_HASH_KEY_SIZE+1];
  char *token;
  
  token = strdup(yytext);
  
  info->line = num_lines;
  info->type = tokenType;

  // Get token real value
  switch (tokenType) {
    case POA_LIT_STRING:
      removeQuotes(token);
      info->value.strVal = token;
      break;
    case POA_LIT_CHAR:
      removeQuotes(token);
      info->value.charVal = token[0];
      break;
    case POA_IDENT:
      info->value.strVal = token;
      break;
    case POA_LIT_BOOL:
      info->value.boolVal = !strcmp(token, "true");
      break;
    case POA_LIT_FLOAT:
      info->value.floatVal = atof(token);
      break;
    case POA_LIT_INT:
      info->value.intVal = atoi(token);
      break;
  }

  // Generate hash table key (TOKEN $$ TYPE)
  snprintf(key, MAX_HASH_KEY_SIZE, "%s $$ %d", token, tokenType);

  free(token);

  // Key is duplicated (strdup) inside dict_put function
  return (TokenInfo*)dict_put(symbolsTable, key, info);
}

void debugPrintTokenInfo(TokenInfo *info) {
    printf(">> ---------------\n");
    printf(">> Line: %d\n", info->line);
    printf(">> Type: %d\n", info->type);
    switch (info->type) {
      case POA_LIT_STRING: printf(">> Value: %s\n", info->value.strVal); break;
      case POA_LIT_CHAR: printf(">> Value: %c\n", info->value.charVal); break;
      case POA_IDENT: printf(">> Value: %s\n", info->value.strVal); break;
      case POA_LIT_BOOL: printf(">> Value: %d\n", info->value.boolVal); break;
      case POA_LIT_FLOAT: printf(">> Value: %.2f\n", info->value.floatVal); break;
      case POA_LIT_INT: printf(">> Value: %d\n", info->value.intVal); break;
    }
}
