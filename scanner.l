/*
  Coloque aqui o identificador do grupo e dos seus membros
*/
%{
#include "parser.h" //arquivo automaticamente gerado pelo bison

int num_lines = 1;

%}

DIGIT    [0-9]

%%

\n          ++num_lines;

int         return TK_PR_INT;
float       return TK_PR_FLOAT;
bool        return TK_PR_BOOL;
char        return TK_PR_CHAR;
string      return TK_PR_STRING;
if          return TK_PR_IF;
then        return TK_PR_THEN;
else        return TK_PR_ELSE;
while       return TK_PR_WHILE;
do          return TK_PR_DO;
input       return TK_PR_INPUT;
output      return TK_PR_OUTPUT;
return      return TK_PR_RETURN;
const       return TK_PR_CONST;
static      return TK_PR_STATIC;
foreach     return TK_PR_FOREACH;
for         return TK_PR_FOR;
switch      return TK_PR_SWITCH;
case        return TK_PR_CASE;
break       return TK_PR_BREAK;
continue    return TK_PR_CONTINUE;
class       return TK_PR_CLASS;
private     return TK_PR_PRIVATE;
public      return TK_PR_PUBLIC;
protected   return TK_PR_PROTECTED;


%%
