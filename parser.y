/*
  Grupo Kappa
  Rubens Luiz Rech Junior - 275601
  Giovane Dutra Ribeiro - 252965
*/

%code requires{
#include "main.h"
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
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_LIT_FALSE
%token TK_LIT_TRUE
%token TK_LIT_CHAR
%token TK_LIT_STRING
%token TK_IDENTIFICADOR
%token TOKEN_ERRO

%left TK_OC_OR
%left TK_OC_AND
%left TK_OC_EQ TK_OC_NE
%left '<' '>' TK_OC_LE TK_OC_GE
%left '-' '+'
%left '*' '/' '%'
%right '!'

%%
/* Regras (e ações) da gramática */

programa: /* empty */
          | programa content;

content: type_def ';'
        | global_def ';'
        | func_dec;

/* Types declaration */
        
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

/* Globals declaration */

global_def: global_var
           | global_arr;

global_var: TK_PR_STATIC type TK_IDENTIFICADOR
         | type TK_IDENTIFICADOR;

global_arr: TK_PR_STATIC type TK_IDENTIFICADOR '[' int_pos ']'
         | type TK_IDENTIFICADOR '[' int_pos ']';

type: native_type
     | TK_IDENTIFICADOR; // use TK_IDENTIFICADOR for detecting types defined by user??

int_pos: TK_LIT_INT; // how to accept only positive integers!?

/* Function declaration */

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

command: var_dec_cmd
        | shift_cmd
        | assig_cmd
        | block;

/* Local variables declaration */

var_dec_cmd: TK_PR_STATIC TK_PR_CONST type TK_IDENTIFICADOR init_var ';'
        | TK_PR_STATIC type TK_IDENTIFICADOR init_var ';'
        | TK_PR_CONST type TK_IDENTIFICADOR init_var ';'
        | type TK_IDENTIFICADOR init_var ';';

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

/* Shift command */

shift_cmd: TK_IDENTIFICADOR TK_OC_SL int_pos ';'
          | TK_IDENTIFICADOR TK_OC_SR int_pos ';';

/* Assignment */

assig_cmd: TK_IDENTIFICADOR '=' exp ';'
          | TK_IDENTIFICADOR '[' exp ']' '=' exp ';'
          | TK_IDENTIFICADOR '.' TK_IDENTIFICADOR '=' exp ';';

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
/* Doubts */
    | exp '%' exp
    | TK_LIT_TRUE
    | TK_LIT_FALSE
    | '!' exp
    | '.'
    | '-' exp;
    /*
    | func_call
    | pipe_exp
    */

    
    

%%
