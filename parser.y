/*
  Grupo Kappa
  Rubens Luiz Rech Junior - 275601
  Giovane Dutra Ribeiro - 252965
*/

%code requires {
#include "main.h"
#include "cc_misc.h"
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

%union {
   TokenInfo *valor_lexico;
}

%%
/* Regras (e ações) da gramática */

programa: /* empty */
          | programa content;

content: type_def ';'
        | global_def ';'
        | func_dec;

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

func_dec: TK_PR_STATIC type TK_IDENTIFICADOR '(' params_dec ')' block
         | type TK_IDENTIFICADOR '(' params_dec ')' block;

params_dec: /* empty */
           | params_dec_list;

params_dec_list: param_dec
                | params_dec_list ',' param_dec;

param_dec: TK_PR_CONST type TK_IDENTIFICADOR
          | type TK_IDENTIFICADOR;

block: '{' commands '}';

commands: /* empty */
         | commands command;

command: var_dec ';'
        | shift_cmd ';'
        | assig_cmd ';'
        | io_cmd ';'
        | func_call ';'
        | return_cmd ';'
        | break_cmd ';'
        | continue_cmd ';'
        | case_cmd
        | pipe_exp ';'
        | do_while ';'
        | block ';'
        | if_stm
        | foreach
        | while
        | switch
        | for;

/* Local variables declaration - command */

var_dec: TK_PR_STATIC TK_PR_CONST native_type TK_IDENTIFICADOR init_var 
        | TK_PR_STATIC native_type TK_IDENTIFICADOR init_var
        | TK_PR_CONST native_type TK_IDENTIFICADOR init_var
        | native_type TK_IDENTIFICADOR init_var
        
        /* Cannot initialize user type variables */
        | TK_PR_STATIC TK_PR_CONST TK_IDENTIFICADOR TK_IDENTIFICADOR 
        | TK_PR_STATIC TK_IDENTIFICADOR TK_IDENTIFICADOR
        | TK_PR_CONST TK_IDENTIFICADOR TK_IDENTIFICADOR
        | TK_IDENTIFICADOR TK_IDENTIFICADOR;

init_var: /* empty */
         | TK_OC_LE literal
         | TK_OC_LE TK_IDENTIFICADOR;

literal: TK_LIT_INT
        | '+' TK_LIT_INT
        | '-' TK_LIT_INT
        | TK_LIT_FLOAT
        | '+' TK_LIT_FLOAT
        | '-' TK_LIT_FLOAT
        | TK_LIT_FALSE
        | TK_LIT_TRUE
        | TK_LIT_CHAR
        | TK_LIT_STRING;

/* Shift command - command */

shift_cmd: TK_IDENTIFICADOR TK_OC_SL int_pos
          | TK_IDENTIFICADOR TK_OC_SR int_pos;

/* Assignment - command */

assig_cmd: TK_IDENTIFICADOR '=' exp
          | TK_IDENTIFICADOR '[' exp ']' '=' exp
          | TK_IDENTIFICADOR '.' TK_IDENTIFICADOR '=' exp;

/* Input and output - command */

io_cmd: TK_PR_INPUT exp
       | TK_PR_OUTPUT exps_list;

/* Function call - command */

func_call: TK_IDENTIFICADOR '(' params ')';

params: /* empty */
       | params_list;

params_list: param
            | params_list ',' param;
        
param: exp;

/* Return, break, continue and case - commands */

return_cmd: TK_PR_RETURN exp;

break_cmd: TK_PR_BREAK;

continue_cmd: TK_PR_CONTINUE;

case_cmd: TK_PR_CASE TK_LIT_INT ':';

/* Pipes - command */

pipe_exp: func_call TK_OC_PG func_call
         | func_call TK_OC_PB func_call
         | pipe_exp TK_OC_PG func_call
         | pipe_exp TK_OC_PB func_call;

/* Control flow - commands */

if_stm: TK_PR_IF '(' exp ')' TK_PR_THEN block;
       | TK_PR_IF '(' exp ')' TK_PR_THEN block TK_PR_ELSE block;

foreach: TK_PR_FOREACH '(' TK_IDENTIFICADOR ':' exps_list ')' block;

while: TK_PR_WHILE '(' exp ')' TK_PR_DO block;

do_while: TK_PR_DO block TK_PR_WHILE '(' exp ')';

switch: TK_PR_SWITCH '(' exp ')' block;

for: TK_PR_FOR '(' cmd_list ':' exp ':' cmd_list ')' block;

cmd_list: cmd
         | cmd_list ',' cmd;

cmd:    var_dec
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

exp: TK_IDENTIFICADOR
    | TK_IDENTIFICADOR '[' exp ']'
    | TK_LIT_INT
    | TK_LIT_FLOAT
    | exp '+' exp
    | exp '-' exp
    | exp '*' exp
    | exp '/' exp
    | '(' exp ')'
    | exp TK_OC_EQ exp
    | exp TK_OC_NE exp
    | exp TK_OC_GE 
    | exp TK_OC_LE exp
    | exp '>' exp
    | exp '<' exp
    | exp TK_OC_AND exp
    | exp TK_OC_OR exp
    | func_call
    | pipe_exp
/* Doubts */
    | TK_LIT_STRING
    | TK_LIT_CHAR
    | exp '%' exp
    | TK_LIT_TRUE
    | TK_LIT_FALSE
    | '!' exp
    | '.'
    | '-' exp;

exps_list: exp
          | exps_list ',' exp;

%%
