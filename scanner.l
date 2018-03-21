/*
  Coloque aqui o identificador do grupo e dos seus membros
*/

%x comment

%{
#include "parser.h" //arquivo automaticamente gerado pelo bison
#include "cc_misc.h"

int num_lines = 1;
%}

DIGIT           [0-9]
LIT_INTEGER     (\+|\-)?{DIGIT}+
LIT_FLOAT       {LIT_INTEGER}+\.{DIGIT}+((e|E)(\+|\-)?{DIGIT}+)?
LIT_CHAR        '([^']|(\\.))'
LIT_STR         \"[^\"]*\"
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

true        return TK_LIT_TRUE;
false       return TK_LIT_FALSE;

{ID}        { addSymbolsTable(); return TK_IDENTIFICADOR; }

{LIT_FLOAT}     { addSymbolsTable(); return TK_LIT_FLOAT; }
{LIT_INTEGER}   { addSymbolsTable(); return TK_LIT_INT; }
{LIT_CHAR}      { addSymbolsTable(); return TK_LIT_CHAR; }
{LIT_STR}       { addSymbolsTable(); return TK_LIT_STRING; }

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
[^,;:()[\]{}+\-*/<>=!&%#^.]			return TOKEN_ERRO;

%%
