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

%%
/* Regras (e ações) da gramática */

programa: /* empty */
          | programa content;

content: type_def ';'
        | global_def ';';

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

%%
