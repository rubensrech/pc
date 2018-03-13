#include <string.h>
#include <stdlib.h>
#include "cc_misc.h"
#include "cc_dict.h"
#include "main.h"

#define MAX_LEXEME_SIZE 200

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
  fprintf (stderr, "%s - line (%d)\n", mensagem, getLineNumber());
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
  struct comp_dict_item *entry, *tmp;

  // Free symbols table
  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        tmp = dict_remove(symbolsTable, entry->key);
        entry = entry->next;
        free(tmp);
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

void addSymbolsTable() {
  char *token = (char *)calloc(strlen(yytext), sizeof(char));
  int *line = (int *)malloc(sizeof(int));

  if (yytext[0] == '\'' || yytext[0] == '"') {
    // Remove literal strings/chars quotes
    strncpy(token, yytext+1, strlen(yytext)-2);
  } else {
    strcpy(token, yytext);
  }

  (*line) = num_lines;

  dict_put(symbolsTable, token, line);
  // printf("> Added to symbols table: %s - %d\n", token, *line);
}
