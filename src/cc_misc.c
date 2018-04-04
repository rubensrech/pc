#include <string.h>
#include <stdlib.h>
#include "cc_misc.h"
#include "cc_dict.h"
#include "main.h"

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
  void *value;

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
  int *line;

  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        line = entry->value;
        cc_dict_etapa_1_print_entrada(entry->key, *line);
        entry = entry->next;
      }
  }
}

void removeQuotes(char *token) {
  int i;
  if (token[0] == '\'' || token[0] == '"') {
    // Remove literal strings/chars quotes
    token[strlen(token)-1] = '\0';
    for (i = 0; i < strlen(token); i++) {
      token[i] = token[i+1];
    }
  }
}

void addSymbolsTable() {
  char *token = strdup(yytext);
  int *line = (int *)malloc(sizeof(int));

  removeQuotes(token);

  (*line) = num_lines;

  dict_put(symbolsTable, token, line);
  free(token); // token is duplicated (strdup) inside dict_put function
}
