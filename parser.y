/*
  Grupo Kappa
  Rubens Luiz Rech Junior - 275601
  Giovane Dutra Ribeiro - 252965
*/

%code requires {
#include "main.h"
#include "cc_misc.h"
#include "cc_tree.h"
#include "cc_ast.h"

extern comp_tree_t *ast;
}

%union {
   TokenInfo *valor_lexico;
   comp_tree_t *ast;
}

/* Declaração dos tokens da linguagem */
%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_BOOL
%token TK_PR_CHAR
%token TK_PR_STRING
%token TK_PR_IF
%token TK_PR_THEN
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_DO
%token TK_PR_INPUT
%token TK_PR_OUTPUT
%token TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token TK_PR_FOREACH
%token TK_PR_FOR
%token TK_PR_SWITCH
%token TK_PR_CASE
%token TK_PR_BREAK
%token TK_PR_CONTINUE
%token TK_PR_CLASS
%token TK_PR_PRIVATE
%token TK_PR_PUBLIC
%token TK_PR_PROTECTED
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TK_OC_SL
%token TK_OC_SR
%token TK_OC_PG
%token TK_OC_PB
%token <valor_lexico>TK_LIT_INT
%token <valor_lexico>TK_LIT_FLOAT
%token <valor_lexico>TK_LIT_FALSE
%token <valor_lexico>TK_LIT_TRUE
%token <valor_lexico>TK_LIT_CHAR
%token <valor_lexico>TK_LIT_STRING
%token <valor_lexico>TK_IDENTIFICADOR
%token TOKEN_ERRO

%left TK_OC_OR
%left TK_OC_AND
%left TK_OC_EQ TK_OC_NE
%left '<' '>' TK_OC_LE TK_OC_GE
%left '-' '+'
%left TK_OC_PG TK_OC_PB
%left '*' '/' '%'
%right '!'
%right TK_PR_THEN TK_PR_ELSE

%error-verbose

%type <ast>programa
%type <ast>code
%type <ast>func_dec
%type <ast>block
%type <ast>commands
%type <ast>command

%type<ast>if_stm
%type <ast>assig_cmd
%type <ast>var_dec
%type <ast>init_var
%type <ast>while
%type <ast>do_while
%type <ast>return_cmd

%type <ast>func_call
%type <ast>params
%type <ast>params_list
%type <ast>param

%type <ast>exp
%type <ast>literal

%type <ast>int
%type <ast>float
%type <ast>true
%type <ast>false
%type <ast>char
%type <ast>string

%type <ast>array
%type <ast>id

%%
/* Regras (e ações) da gramática */

programa: /* empty */   { $$ = makeASTNode(AST_PROGRAMA, NULL); ast = $$; }
         | code         { $$ = makeASTUnaryNode(AST_PROGRAMA, NULL, $1); ast = $$; };

code:  type_def ';'             { $$ = NULL; }
     | global_def ';'           { $$ = NULL; }
     | func_dec                 { $$ = $1; }
     | type_def ';' code        { $$ = $3; }
     | global_def ';' code      { $$ = $3; }
     | func_dec code            { 
                                        if ($2 != NULL) tree_set_list_next_node($1, $2);
                                        $$ = $1;
                                };

/* Auxiliary rules */

id: TK_IDENTIFICADOR            { $$ = makeASTNode(AST_IDENTIFICADOR, $1); };
array: id '[' exp ']'           { $$ = makeASTBinaryNode(AST_VETOR_INDEXADO, NULL, $1, $3); };

int: TK_LIT_INT                 { $$ = makeASTNode(AST_LITERAL, $1); };
float: TK_LIT_FLOAT             { $$ = makeASTNode(AST_LITERAL, $1); };
false: TK_LIT_FALSE             { $$ = makeASTNode(AST_LITERAL, $1); };
true: TK_LIT_TRUE               { $$ = makeASTNode(AST_LITERAL, $1); };
char: TK_LIT_CHAR               { $$ = makeASTNode(AST_LITERAL, $1); };
string: TK_LIT_STRING           { $$ = makeASTNode(AST_LITERAL, $1); };

/* New Type Declaration */
        
type_def: TK_PR_CLASS TK_IDENTIFICADOR '[' type_def_campos ']';

type_def_campos: type_def_campo
                | type_def_campos ':' type_def_campo;
              
type_def_campo: type_def_camp_enc native_type TK_IDENTIFICADOR;

type_def_camp_enc: TK_PR_PROTECTED
                  | TK_PR_PRIVATE
                  | TK_PR_PUBLIC;

native_type: TK_PR_INT
            | TK_PR_FLOAT
            | TK_PR_CHAR
            | TK_PR_BOOL
            | TK_PR_STRING;

/* Global Variables Declaration */

global_def: global_var
           | global_arr;

global_var: TK_PR_STATIC type TK_IDENTIFICADOR
         | type TK_IDENTIFICADOR;

global_arr: TK_PR_STATIC type TK_IDENTIFICADOR '[' int_pos ']'
         | type TK_IDENTIFICADOR '[' int_pos ']';

type: native_type
     | TK_IDENTIFICADOR; // Detects user created types

int_pos: TK_LIT_INT; // The '+' Sign was removed from the regEx to accept only positive numbers with TK_LIT_INT

/* Function Declaration */

func_dec: TK_PR_STATIC type TK_IDENTIFICADOR '(' params_dec ')' block   { $$ = makeASTUnaryNode(AST_FUNCAO, $3, $7); }
         | type TK_IDENTIFICADOR '(' params_dec ')' block               { $$ = makeASTUnaryNode(AST_FUNCAO, $2, $6); };

params_dec: /* empty */
           | params_dec_list;

params_dec_list: param_dec
                | params_dec_list ',' param_dec;

param_dec: TK_PR_CONST type TK_IDENTIFICADOR
          | type TK_IDENTIFICADOR;

block:  '{' '}'                    { $$ = makeASTNode(AST_BLOCO, NULL); /* EMPTY BLOCK?? */ }
      | '{' commands '}'           { $$ = $2; };

commands: command                  { $$ = $1; }
         | command commands        { 
                                        if ($1 != NULL && $2 != NULL) {
                                                tree_set_list_next_node($1, $2);
                                                $$ = $1;
                                        }
                                        if ($1 == NULL) $$ = $2;
                                   };

/* Simple Commands */
command:  var_dec ';'           { $$ = $1; }
        | shift_cmd ';'         {}
        | assig_cmd ';'         { $$ = $1; }
        | io_cmd ';'            {}
        | func_call ';'         {}
        | return_cmd ';'        { $$ = $1; }
        | break_cmd ';'         {}
        | continue_cmd ';'      {}
        | case_cmd              {}
        | pipe_exp ';'          {}
        | do_while ';'          { $$ = $1; }
        | block ';'             { $$ = $1; }
        | if_stm                { $$ = $1; }
        | foreach               {}
        | while                 { $$ = $1; }
        | switch                {}
        | for                   {};

/* Local Variables Declaration - command */

var_dec: TK_PR_STATIC TK_PR_CONST native_type id init_var { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $4, $5); }
        | TK_PR_STATIC native_type id init_var            { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $3, $4); }
        | TK_PR_CONST native_type id init_var             { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $3, $4); }
        | native_type id init_var                         { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $2, $3); }

        | TK_PR_STATIC TK_PR_CONST native_type TK_IDENTIFICADOR         { $$ = NULL; }
        | TK_PR_STATIC native_type TK_IDENTIFICADOR                     { $$ = NULL; }
        | TK_PR_CONST native_type TK_IDENTIFICADOR                      { $$ = NULL; }
        | native_type TK_IDENTIFICADOR                                  { $$ = NULL; }
        
        /* Cannot initialize user type variables */
        | TK_PR_STATIC TK_PR_CONST TK_IDENTIFICADOR TK_IDENTIFICADOR         { $$ = NULL; }
        | TK_PR_STATIC TK_IDENTIFICADOR TK_IDENTIFICADOR                     { $$ = NULL; }
        | TK_PR_CONST TK_IDENTIFICADOR TK_IDENTIFICADOR                      { $$ = NULL; }
        | TK_IDENTIFICADOR TK_IDENTIFICADOR                                  { $$ = NULL; };

init_var: TK_OC_LE literal      { $$ = $2; }
         | TK_OC_LE id          { $$ = $2; };

literal:  int                   { $$ = $1; }
        | '+' int               { $$ = $2; }
        | '-' int               { $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2); }
        | float                 { $$ = $1; }
        | '+' float             { $$ = $2; }
        | '-' float             { $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2); }
        | false                 { $$ = $1; }
        | true                  { $$ = $1; }
        | char                  { $$ = $1; }
        | string                { $$ = $1; };

/* Shift command - command */

shift_cmd: TK_IDENTIFICADOR TK_OC_SL int_pos
          | TK_IDENTIFICADOR TK_OC_SR int_pos;

/* Assignment - command */

assig_cmd: id '=' exp                   { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $3); }
          | array '=' exp               { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $3); }
          | id '.' id '=' exp           { $$ = makeASTTernaryNode(AST_ATRIBUICAO, NULL, $1, $3, $5); }

          | id '=' '+' exp              { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $4); }
          | array '=' '+' exp           { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $4); }
          | id '.' id '=' '+' exp       { $$ = makeASTTernaryNode(AST_ATRIBUICAO, NULL, $1, $3, $6); };

/* Input and output - command */

io_cmd: TK_PR_INPUT exp
       | TK_PR_OUTPUT exps_list;

/* Function call - command */

func_call: id '(' params ')'    {
                                        if ($3 != NULL) $$ = makeASTBinaryNode(AST_CHAMADA_DE_FUNCAO, NULL, $1, $3);
                                        else $$ = makeASTUnaryNode(AST_CHAMADA_DE_FUNCAO, NULL, $1);
                                };

params:  /* empty */                    { $$ = NULL; }
       | params_list                    { $$ = $1; };

params_list: param                      { $$ = $1; }
            | param ',' params_list     {
                                                tree_set_list_next_node($1, $3);
                                                $$ = $1;
                                        };
        
param: exp                              { $$ = $1; };

/* Return, break, continue and case - commands */

return_cmd: TK_PR_RETURN exp            { $$ = makeASTUnaryNode(AST_RETURN, NULL, $2); };

break_cmd: TK_PR_BREAK;

continue_cmd: TK_PR_CONTINUE;

case_cmd: TK_PR_CASE TK_LIT_INT ':'
        | TK_PR_CASE '+' TK_LIT_INT ':'
        | TK_PR_CASE '-' TK_LIT_INT ':';

/* Pipes - command */

pipe_exp: func_call TK_OC_PG func_call
         | func_call TK_OC_PB func_call
         | pipe_exp TK_OC_PG func_call
         | pipe_exp TK_OC_PB func_call;

/* Flow Control - Commands */

if_stm:  TK_PR_IF '(' exp ')' TK_PR_THEN block                          { $$ = makeASTBinaryNode(AST_IF_ELSE, NULL, $3, $6); }
       | TK_PR_IF '(' exp ')' TK_PR_THEN block TK_PR_ELSE block         { $$ = makeASTTernaryNode(AST_IF_ELSE, NULL, $3, $6, $8);  };

foreach: TK_PR_FOREACH '(' TK_IDENTIFICADOR ':' exps_list ')' block;

while: TK_PR_WHILE '(' exp ')' TK_PR_DO block                   { $$ = makeASTBinaryNode(AST_WHILE_DO, NULL, $3, $6); };

do_while: TK_PR_DO block TK_PR_WHILE '(' exp ')'                { $$ = makeASTBinaryNode(AST_DO_WHILE, NULL, $2, $5); };

switch: TK_PR_SWITCH '(' exp ')' block;

for: TK_PR_FOR '(' cmd_list ':' exp ':' cmd_list ')' block;

cmd_list: cmd
         | cmd_list ',' cmd;

cmd:      var_dec
        | shift_cmd
        | assig_cmd
        | block
        | func_call
        | return_cmd
        | break_cmd
        | continue_cmd
        | pipe_exp
        | if_stm
        | foreach
        | while
        | do_while
        | switch
        | for;

/* Expressions */

exp:  id                        { $$ = $1; }         
    | array                     { $$ = $1; }
    | exp '+' exp               { $$ = makeASTBinaryNode(AST_ARIM_SOMA, NULL, $1, $3); }
    | exp '-' exp               { $$ = makeASTBinaryNode(AST_ARIM_SUBTRACAO, NULL, $1, $3); }
    | exp '*' exp               { $$ = makeASTBinaryNode(AST_ARIM_MULTIPLICACAO, NULL, $1, $3); }
    | exp '/' exp               { $$ = makeASTBinaryNode(AST_ARIM_DIVISAO, NULL, $1, $3); }
    | '(' exp ')'               { $$ = $2; }
    | exp TK_OC_EQ exp          { $$ = makeASTBinaryNode(AST_LOGICO_COMP_IGUAL, NULL, $1, $3); }
    | exp TK_OC_NE exp          { $$ = makeASTBinaryNode(AST_LOGICO_COMP_DIF, NULL, $1, $3); }
    | exp TK_OC_GE exp          { $$ = makeASTBinaryNode(AST_LOGICO_COMP_GE, NULL, $1, $3); }
    | exp TK_OC_LE exp          { $$ = makeASTBinaryNode(AST_LOGICO_COMP_LE, NULL, $1, $3); }
    | exp '>' exp               { $$ = makeASTBinaryNode(AST_LOGICO_COMP_G, NULL, $1, $3); }
    | exp '<' exp               { $$ = makeASTBinaryNode(AST_LOGICO_COMP_L, NULL, $1, $3); }
    | exp TK_OC_AND exp         { $$ = makeASTBinaryNode(AST_LOGICO_E, NULL, $1, $3); }
    | exp TK_OC_OR exp          { $$ = makeASTBinaryNode(AST_LOGICO_OU, NULL, $1, $3); }
    | func_call                 { $$ = $1; }
    | pipe_exp                  {}
    | exp '%' exp               { $$ = makeASTBinaryNode(AST_ARIM_MOD, NULL, $1, $3); }
    | '!' exp                   { $$ = makeASTUnaryNode(AST_LOGICO_COMP_NEGACAO, NULL, $2); }
    | '.'                       {}
    | '-' exp                   { $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2); }
    | int                       { $$ = $1; }
    | float                     { $$ = $1; }
    | string                    { $$ = $1; }
    | char                      { $$ = $1; }
    | true                      { $$ = $1; }
    | false                     { $$ = $1; };

exps_list: exp
          | exps_list ',' exp;

%%
