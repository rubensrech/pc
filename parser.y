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
#include "cc_sem.h"

extern comp_tree_t *ast;
extern int scope;
extern int scope_uniq;
}

%union {
   TokenInfo *valor_lexico;
   comp_tree_t *ast;
   int dataType;
}

/* Declaração dos tokens da linguagem */
%token <dataType>TK_PR_INT
%token <dataType>TK_PR_FLOAT
%token <dataType>TK_PR_BOOL
%token <dataType>TK_PR_CHAR
%token <dataType>TK_PR_STRING
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
%type <valor_lexico>func_id
%type <ast>block
%type <ast>commands
%type <ast>command
%type <ast>if_stm
%type <ast>assig_cmd
%type <ast>var_dec
%type <ast>init_var
%type <ast>while
%type <ast>do_while
%type <ast>return_cmd
%type <ast>io_cmd
%type <ast>shift_cmd
%type <ast>break_cmd
%type <ast>continue_cmd
%type <ast>case_cmd
%type <ast>switch
%type <ast>foreach
%type <ast>for
%type <ast>pipe_exp
%type <ast>func_call
%type <ast>params
%type <ast>params_list
%type <ast>param
%type <ast>exp
%type <ast>exps_list
%type <ast>literal
%type <ast>cmd_list
%type <ast>cmd
%type <ast>int
%type <ast>int_neg
%type <ast>float
%type <ast>true
%type <ast>false
%type <ast>char
%type <ast>string
%type <ast>array
%type <ast>id

%type <dataType>native_type
%type <dataType>type

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
int_neg: '-' int                { $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2); };
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

native_type: TK_PR_INT          { $$ = $1; }
            | TK_PR_FLOAT       { $$ = $1; }
            | TK_PR_CHAR        { $$ = $1; }
            | TK_PR_BOOL        { $$ = $1; }
            | TK_PR_STRING      { $$ = $1; };

/* Global Variables Declaration */

type: native_type               { $$ = $1; }
     | TK_IDENTIFICADOR         { $$ = DATATYPE_NONE; /* Missing: check $2 is user type name */ };

global_def: global_var
           | global_arr;

global_var: TK_PR_STATIC type TK_IDENTIFICADOR          { 
                                                                if ($2 == DATATYPE_NONE) {
                                                                        setIdType($3, USER_TYPE_ID);
                                                                } else {
                                                                        setIdType($3, VAR_ID);
                                                                } 
                                                        }
           | type TK_IDENTIFICADOR                      {
                                                                if ($1 == DATATYPE_NONE) {
                                                                        setIdType($2, USER_TYPE_ID);
                                                                } else {
                                                                        setIdType($2, VAR_ID);
                                                                }
                                                        };

global_arr: TK_PR_STATIC type TK_IDENTIFICADOR '[' TK_LIT_INT ']'
         | type TK_IDENTIFICADOR '[' TK_LIT_INT ']';

/* Function Declaration */

func_dec: func_id '(' params_dec ')' block      {
                                                        $$ = makeASTUnaryNode(AST_FUNCAO, $1, $5);
                                                        // Scope ended -> back to global scope
                                                        scope = 0;
                                                };

func_id: TK_PR_STATIC type TK_IDENTIFICADOR   { $$ = $3; scope_uniq++; scope = scope_uniq; }
        | type TK_IDENTIFICADOR               { $$ = $2; scope_uniq++; scope = scope_uniq; };

params_dec: /* empty */
           | params_dec_list;

params_dec_list: param_dec
                | params_dec_list ',' param_dec;

param_dec: TK_PR_CONST type TK_IDENTIFICADOR
          | type TK_IDENTIFICADOR;

block:  '{' '}'                    { $$ = makeASTNode(AST_BLOCO, NULL); }
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
        | shift_cmd ';'         { $$ = $1; }
        | assig_cmd ';'         { $$ = $1; }
        | io_cmd ';'            { $$ = $1; }
        | func_call ';'         { $$ = $1; }
        | return_cmd ';'        { $$ = $1; }
        | break_cmd ';'         { $$ = $1; }
        | continue_cmd ';'      { $$ = $1; }
        | case_cmd              { $$ = $1; }
        | pipe_exp ';'          { $$ = $1; }
        | do_while ';'          { $$ = $1; }
        | block ';'             { $$ = $1; }
        | if_stm                { $$ = $1; }
        | foreach               { $$ = $1; }
        | while                 { $$ = $1; }
        | switch                { $$ = $1; }
        | for                   { $$ = $1; };

/* Local Variables Declaration - command */

var_dec:
        /* Declarations with init value (only native types) */
        var_dec_mods native_type id init_var   { 
                                                        $$ = makeASTBinaryNode(AST_INICIALIZACAO, NULL, $3, $4);
                                                        setIdNodeIdType($3, VAR_ID); // also checks redeclaration
                                                        setIdNodeDataType($3, $2);
                                                        checkDataTypeMatching($2, getASTNodeTokenDataType($3));
                                                }
        | native_type id init_var               {
                                                        $$ = makeASTBinaryNode(AST_INICIALIZACAO, NULL, $2, $3);
                                                        setIdNodeIdType($2, VAR_ID); // also checks redeclaration
                                                        setIdNodeDataType($2, $1);
                                                        checkDataTypeMatching($1, getASTNodeTokenDataType($2));
                                                }

        /* Native type declarations with no init value */
        | var_dec_mods native_type TK_IDENTIFICADOR     {
                                                                $$ = NULL;
                                                                setIdType($3, VAR_ID); // also checks redeclaration
                                                                setIdDataType($3, $2);
                                                        }
        | native_type TK_IDENTIFICADOR                  {
                                                                $$ = NULL;
                                                                setIdType($2, VAR_ID); // also checks redeclaration
                                                                setIdDataType($2, $1);
                                                        }
        
        /* Cannot initialize user type variables */
        | var_dec_mods TK_IDENTIFICADOR TK_IDENTIFICADOR        {
                                                                        $$ = NULL;
                                                                        /* Missing: check $1 is user type name */
                                                                        setIdType($3, USER_TYPE_ID);
                                                                }
        | TK_IDENTIFICADOR TK_IDENTIFICADOR                     {
                                                                        $$ = NULL;
                                                                        /* Missing: check $1 is user type name */
                                                                        setIdType($2, USER_TYPE_ID);
                                                                };

var_dec_mods: TK_PR_STATIC
             | TK_PR_CONST
             | TK_PR_STATIC TK_PR_CONST;

init_var: TK_OC_LE literal      { $$ = $2; }
         | TK_OC_LE id          { $$ = $2; checkIdNodeDeclared($2); };

literal:  int                   { $$ = $1; }
        | '+' int               { $$ = $2; }
        | int_neg               { $$ = $1; }
        | float                 { $$ = $1; }
        | '+' float             { $$ = $2; }
        | '-' float             { $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2); }
        | false                 { $$ = $1; }
        | true                  { $$ = $1; }
        | char                  { $$ = $1; }
        | string                { $$ = $1; };

/* Shift command - command */

shift_cmd: id TK_OC_SL int              { $$ = makeASTBinaryNode(AST_SHIFT_LEFT, NULL, $1, $3); checkIdNodeDeclared($1); }
          | id TK_OC_SR int             { $$ = makeASTBinaryNode(AST_SHIFT_RIGHT, NULL, $1, $3); checkIdNodeDeclared($1); };

/* Assignment - command */

assig_cmd: id '=' exp                   { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $3); checkIdNodeDeclared($1); }
          | array '=' exp               { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $3);  }
          | id '.' id '=' exp           { $$ = makeASTTernaryNode(AST_ATRIBUICAO, NULL, $1, $3, $5); }

          /* Accept unary operator (+) => e.g.: a = +15  */
          | id '=' '+' exp              { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $4); checkIdNodeDeclared($1); }
          | array '=' '+' exp           { $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $4); }
          | id '.' id '=' '+' exp       { $$ = makeASTTernaryNode(AST_ATRIBUICAO, NULL, $1, $3, $6); };

/* Input and output - command */

io_cmd: TK_PR_INPUT exp                 { $$ = makeASTUnaryNode(AST_INPUT, NULL, $2); }
       | TK_PR_OUTPUT exps_list         { $$ = makeASTUnaryNode(AST_OUTPUT, NULL, $2); };

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

break_cmd: TK_PR_BREAK                  { $$ = makeASTNode(AST_BREAK, NULL); };

continue_cmd: TK_PR_CONTINUE            { $$ = makeASTNode(AST_CONTINUE, NULL); };

case_cmd: TK_PR_CASE int ':'            { $$ = makeASTUnaryNode(AST_CASE, NULL, $2); }
        | TK_PR_CASE '+' int ':'        { $$ = makeASTUnaryNode(AST_CASE, NULL, $3); }
        | TK_PR_CASE int_neg ':'        { $$ = makeASTUnaryNode(AST_CASE, NULL, $2); };

/* Pipes - command */

pipe_exp: func_call TK_OC_PG func_call          { $$ = makeASTBinaryNode(AST_PIPE_G, NULL, $1, $3); }
         | func_call TK_OC_PB func_call         { $$ = makeASTBinaryNode(AST_PIPE_B, NULL, $1, $3); }
         | pipe_exp TK_OC_PG func_call          { $$ = makeASTBinaryNode(AST_PIPE_G, NULL, $1, $3); }
         | pipe_exp TK_OC_PB func_call          { $$ = makeASTBinaryNode(AST_PIPE_B, NULL, $1, $3); };

/* Flow Control - Commands */

if_stm:  TK_PR_IF '(' exp ')' TK_PR_THEN block                          { $$ = makeASTBinaryNode(AST_IF_ELSE, NULL, $3, $6); }
       | TK_PR_IF '(' exp ')' TK_PR_THEN block TK_PR_ELSE block         { $$ = makeASTTernaryNode(AST_IF_ELSE, NULL, $3, $6, $8);  };

foreach: TK_PR_FOREACH '(' id ':' exps_list ')' block                   {
                                                                                $$ = makeASTTernaryNode(AST_FOREACH, NULL, $3, $5, $7);
                                                                                checkIdNodeDeclared($3);
                                                                        };

while: TK_PR_WHILE '(' exp ')' TK_PR_DO block                   { $$ = makeASTBinaryNode(AST_WHILE_DO, NULL, $3, $6); };

do_while: TK_PR_DO block TK_PR_WHILE '(' exp ')'                { $$ = makeASTBinaryNode(AST_DO_WHILE, NULL, $2, $5); };

switch: TK_PR_SWITCH '(' exp ')' block                          { $$ = makeASTBinaryNode(AST_SWITCH, NULL, $3, $5); };

for: TK_PR_FOR '(' cmd_list ':' exp ':' cmd_list ')' block      { $$ = makeASTQuaternaryNode(AST_FOR, NULL, $3, $5, $7, $9); };

cmd_list: cmd                           { $$ = $1; }
         | cmd ',' cmd_list             {
                                                tree_set_list_next_node($1, $3);
                                                $$ = $1;  
                                        };

cmd:      var_dec                       { $$ = $1; }
        | shift_cmd                     { $$ = $1; }
        | assig_cmd                     { $$ = $1; }
        | block                         { $$ = $1; }
        | func_call                     { $$ = $1; }
        | return_cmd                    { $$ = $1; }
        | break_cmd                     { $$ = $1; }
        | continue_cmd                  { $$ = $1; }
        | pipe_exp                      { $$ = $1; }
        | if_stm                        { $$ = $1; }
        | foreach                       { $$ = $1; }
        | while                         { $$ = $1; }
        | do_while                      { $$ = $1; }
        | switch                        { $$ = $1; }
        | for                           { $$ = $1; };

/* Expressions */

exp:  id                        { $$ = $1; checkIdNodeDeclared($1); }         
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
    | pipe_exp                  { $$ = $1; }
    | exp '%' exp               { $$ = makeASTBinaryNode(AST_ARIM_MOD, NULL, $1, $3); }
    | '!' exp                   { $$ = makeASTUnaryNode(AST_LOGICO_COMP_NEGACAO, NULL, $2); }
    | '.'                       { $$ = makeASTNode(AST_DOT_PARAM, NULL); }
    | '-' exp                   { $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2); }
    | int                       { $$ = $1; }
    | float                     { $$ = $1; }
    | string                    { $$ = $1; }
    | char                      { $$ = $1; }
    | true                      { $$ = $1; }
    | false                     { $$ = $1; };

exps_list: exp                  { $$ = $1; }
          | exp ',' exps_list   {
                                        tree_set_list_next_node($1, $3);
                                        $$ = $1;
                                };

%%
