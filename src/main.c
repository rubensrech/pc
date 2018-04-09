#include "main.h"

extern FILE *yyin;
extern char *yytext;
extern int getLineNumber();
#define print_nome(TOKEN) printf("%d " #TOKEN " [%s]\n", comp_get_line_number(), yytext);
#define print_nome2(TOKEN) printf("%d TK_ESPECIAL [%c]\n", comp_get_line_number(), TOKEN);
#define USER_INIT main_init (argc, argv);
#define USER_FINALIZE main_finalize ();

void main_avaliacao_etapa_1_tabela (void);
int main_avaliacao_etapa_1 (int argc, char **argv)
{
  int token = 0;
  while ((token = yylex())) {
    if (getenv("INF47_TABLE")){
      continue;
    }
    switch (token){
    case ',':
    case ';':
    case ':':
    case '(':
    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    case '+':
    case '-':
    case '*':
    case '/':
    case '<':
    case '>':
    case '=':
    case '!':
    case '&':
    case '.':       // $ is not valid anymore...
    case '%':
    case '#':
    case '^': print_nome2 (token); break;
    case TK_PR_INT: print_nome(TK_PR_INT); break;
    case TK_PR_FLOAT: print_nome(TK_PR_FLOAT); break;
    case TK_PR_BOOL: print_nome (TK_PR_BOOL); break;
    case TK_PR_CHAR: print_nome (TK_PR_CHAR); break;
    case TK_PR_STRING: print_nome (TK_PR_STRING); break;
    case TK_PR_IF: print_nome (TK_PR_IF); break;
    case TK_PR_THEN: print_nome (TK_PR_THEN); break;
    case TK_PR_ELSE: print_nome (TK_PR_ELSE); break;
    case TK_PR_WHILE: print_nome (TK_PR_WHILE); break;
    case TK_PR_DO: print_nome (TK_PR_DO); break;
    case TK_PR_INPUT: print_nome (TK_PR_INPUT); break;
    case TK_PR_OUTPUT: print_nome (TK_PR_OUTPUT); break;
    case TK_PR_RETURN: print_nome (TK_PR_RETURN); break;
    case TK_PR_CONST: print_nome (TK_PR_CONST); break;
    case TK_PR_STATIC: print_nome (TK_PR_STATIC); break;
    case TK_PR_FOREACH: print_nome (TK_PR_FOREACH); break;
    case TK_PR_FOR: print_nome (TK_PR_FOR); break;
    case TK_PR_SWITCH: print_nome (TK_PR_SWITCH); break;
    case TK_PR_CASE: print_nome (TK_PR_CASE); break;
    case TK_PR_BREAK: print_nome (TK_PR_BREAK); break;
    case TK_PR_CONTINUE: print_nome (TK_PR_CONTINUE); break;
    case TK_PR_CLASS: print_nome (TK_PR_CLASS); break;
    case TK_PR_PRIVATE: print_nome (TK_PR_PRIVATE); break;
    case TK_PR_PUBLIC: print_nome (TK_PR_PUBLIC); break;
    case TK_PR_PROTECTED: print_nome (TK_PR_PROTECTED); break;
    case TK_OC_LE: print_nome (TK_OC_LE); break;
    case TK_OC_GE: print_nome (TK_OC_GE); break;
    case TK_OC_EQ: print_nome (TK_OC_EQ); break;
    case TK_OC_NE: print_nome (TK_OC_NE); break;
    case TK_OC_AND: print_nome (TK_OC_AND); break;
    case TK_OC_OR: print_nome (TK_OC_OR); break;
    case TK_OC_SL: print_nome (TK_OC_SL); break;
    case TK_OC_SR: print_nome (TK_OC_SR); break;
    case TK_OC_PG: print_nome (TK_OC_PG); break;
    case TK_OC_PB: print_nome (TK_OC_PB); break;
    case TK_LIT_INT: print_nome (TK_LIT_INT); break;
    case TK_LIT_FLOAT: print_nome (TK_LIT_FLOAT); break;
    case TK_LIT_FALSE: print_nome (TK_LIT_FALSE); break;
    case TK_LIT_TRUE: print_nome (TK_LIT_TRUE); break;
    case TK_LIT_CHAR: print_nome (TK_LIT_CHAR); break;
    case TK_LIT_STRING: print_nome (TK_LIT_STRING); break;
    case TK_IDENTIFICADOR: print_nome (TK_IDENTIFICADOR); break;
    case TOKEN_ERRO:  print_nome (TOKEN_ERRO); break;
    default: printf ("<Invalid Token with code %d>\n", token); return 1; break;
    }
  }
  if (getenv("INF47_TABLE")){
    main_avaliacao_etapa_1_tabela();
  }
  return 0;
}

void cc_dict_etapa_1_print_entrada (char *token, int line)
{
  printf("%d [%s]\n", line, token);
}

void cc_dict_etapa_2_print_entrada (char *token, int line, int tipo)
{
  printf("%d [%s] %d\n", line, token, tipo);
}

void main_avaliacao_etapa_1_tabela (void)
{
  comp_print_table();
}

int main_avaliacao_etapa_2 (int argc, char **argv)
{
  int ret = yyparse();

  // To enable the pritting of the symbols table uncomment te function bellow !!
  // comp_print_table();

  return ret;
}

int main_avaliacao_etapa_3 (int argc, char **argv)
{
  gv_init(NULL);
  int ret = yyparse();
  gv_close();
  return ret;
}

int main_avaliacao_etapa_4 (int argc, char **argv)
{
  return main_avaliacao_etapa_2 (argc, argv);
}

int main_avaliacao_etapa_5 (int argc, char **argv)
{
  return main_avaliacao_etapa_2 (argc, argv);
}

int main_avaliacao_etapa_6 (int argc, char **argv)
{
  return main_avaliacao_etapa_2 (argc, argv);
}

int main_avaliacao_etapa_7 (int argc, char **argv)
{
  return main_avaliacao_etapa_2 (argc, argv);
}

int main (int argc, char **argv)
{
  //if some argument is provided, treat it as input
  if (argc != 1){
    yyin = fopen(argv[1], "r");
    //if fopen fails, yyin continues to be stdin
    if (yyin == NULL){
      yyin = stdin;
    }
  }
  USER_INIT;
  int r;
#ifdef AVALIACAO_ETAPA_1
  r = main_avaliacao_etapa_1 (argc, argv);
#elif AVALIACAO_ETAPA_2
  r = main_avaliacao_etapa_2 (argc, argv);
#elif AVALIACAO_ETAPA_3
  r = main_avaliacao_etapa_3 (argc, argv);
#elif AVALIACAO_ETAPA_4
  r = main_avaliacao_etapa_4 (argc, argv);
#elif AVALIACAO_ETAPA_5
  r = main_avaliacao_etapa_5 (argc, argv);
#elif AVALIACAO_ETAPA_6
  r = main_avaliacao_etapa_6 (argc, argv);
#elif AVALIACAO_ETAPA_7
  r = main_avaliacao_etapa_7 (argc, argv);
#else
  r = 0;
#endif
  USER_FINALIZE;
  return r;
}
