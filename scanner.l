/*
  Coloque aqui o identificador do grupo e dos seus membros
*/
%{
#include "parser.h" //arquivo automaticamente gerado pelo bison

int num_lines = 1;

%}

%%

\n    ++num_lines;
int   return TK_PR_INT;

%%
