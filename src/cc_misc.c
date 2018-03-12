#include "cc_misc.h"

extern int num_lines;
extern char *yytext;

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

}

void main_finalize (void)
{
  //implemente esta função com rotinas de inicialização, se necessário
  //comp_print_table();
}

void comp_print_table (void)
{
  //para cada entrada na tabela de símbolos
  //Etapa 1: chame a função cc_dict_etapa_1_print_entrada
  //implemente esta função
}
