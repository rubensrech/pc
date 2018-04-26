#include <string.h>
#include <stdlib.h>
#include "cc_misc.h"
#include "cc_table.h"
#include "cc_ast.h"

extern int num_lines;

int getLineNumber(void) {
  return num_lines;
}

int comp_get_line_number (void)
{
  return getLineNumber();
}

void yyerror (char const *mensagem)
{
  fprintf (stderr, "%s - line %d\n", mensagem, getLineNumber());
  exit(1);
}

void main_init (int argc, char **argv) {
  initSymbolsTable();
  initAST();
}

void main_finalize (void) {
  // comp_print_table();
  freeSymbolsTable();
  freeAST();
}

void comp_print_table (void) {
  printSymbolsTable();
}
