/*
  Grupo Kappa
  Rubens Luiz Rech Junior - 275601
  Giovane Dutra Ribeiro - 252965
*/

%x comment

%{
#include "parser.h" //arquivo automaticamente gerado pelo bison
#include "cc_table.h"

int num_lines = 1;
%}

/* Removed the rule for integers and flot with signs in front of them */
DIGIT           [0-9]
LIT_INTEGER     {DIGIT}+
LIT_FLOAT       [0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
LIT_CHAR        '([^']|(\\.))'
LIT_STR         \"([^\"\\\n]|\\.)*\"
ID              [_a-zA-Z][a-zA-Z0-9_]*

%%

[ \t]+                                    /* remove whitespaces */
\n               ++num_lines;             /* regular line count */

"//".*                                    /* ignore single-line comments */

"/*"                     BEGIN(comment);
<comment>[^*\n]*                          /* ignore anything but a '*' */
<comment>"*"+[^*/\n]*                     /* ignore '*' not followed by '/' */
<comment>\n		           ++num_lines;     /* count lines inside multi-line comments */
<comment>"*"+"/"	       BEGIN(INITIAL);

int         { yylval.dataType = DATATYPE_INT; return TK_PR_INT; }
float       { yylval.dataType = DATATYPE_FLOAT; return TK_PR_FLOAT; }
bool        { yylval.dataType = DATATYPE_BOOL; return TK_PR_BOOL; }
char        { yylval.dataType = DATATYPE_CHAR; return TK_PR_CHAR; }
string      { yylval.dataType = DATATYPE_STRING; return TK_PR_STRING; }
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

true            { yylval.valor_lexico = addSymbolsTable(POA_LIT_BOOL); return TK_LIT_TRUE; }
false           { yylval.valor_lexico = addSymbolsTable(POA_LIT_BOOL); return TK_LIT_FALSE; }

{ID}            { yylval.valor_lexico = addSymbolsTable(POA_IDENT); return TK_IDENTIFICADOR; }

{LIT_INTEGER}   { yylval.valor_lexico = addSymbolsTable(POA_LIT_INT); return TK_LIT_INT; }
{LIT_FLOAT}     { yylval.valor_lexico = addSymbolsTable(POA_LIT_FLOAT); return TK_LIT_FLOAT; }
{LIT_CHAR}      { yylval.valor_lexico = addSymbolsTable(POA_LIT_CHAR); return TK_LIT_CHAR; }
{LIT_STR}       { yylval.valor_lexico = addSymbolsTable(POA_LIT_STRING); return TK_LIT_STRING; }

[0-9]+[a-zA-Z]+     return TOKEN_ERRO;

"<="        return TK_OC_LE;
">="        return TK_OC_GE;
"=="        return TK_OC_EQ;
"!="        return TK_OC_NE;
"&&"        return TK_OC_AND;
"||"        return TK_OC_OR;
">>"        return TK_OC_SR;
"<<"        return TK_OC_SL;
"%>%"       return TK_OC_PG;
"%|%"       return TK_OC_PB;

[,;:()[\]{}+\-*/<>=!&%#^.]    return (int)yytext[0];
.			                        return TOKEN_ERRO;

%%
