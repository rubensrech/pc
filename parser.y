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
#include "cc_cod.h"

extern comp_tree_t *ast;
extern comp_dict_t *funcTable;
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
%type <valor_lexico>func_header
%type <valor_lexico>func_id
%type <ast>block
%type <ast>commands
%type <ast>command
%type <ast>if_stm
%type <ast>if_exp
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
%type <ast>compExp
%type <ast>arimExp
%type <ast>logicExp
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

%type <ast>params_dec
%type <ast>params_dec_list
%type <ast>param_dec

%type <valor_lexico>type_def_id
%type <ast>type_def_campos
%type <ast>type_def_campo

%type <ast>pipe_pg_step0
%type <ast>pipe_pb_step0
%type <ast>pipe_pg_step1
%type <ast>pipe_pb_step1

%%
/* Regras (e ações) da gramática */

programa: /* empty */   { $$ = makeASTNode(AST_PROGRAMA, NULL); ast = $$; }
         | code         { 
                                // > AST
                                if ($1 != NULL) $$ = makeASTUnaryNode(AST_PROGRAMA, NULL, $1);
                                else $$ = makeASTNode(AST_PROGRAMA, NULL);
                                ast = $$;
                                // > Code
                                inheritCodeList($$, $1);
                                printNodeCodeList($$);
                        };

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
array: id '[' exp ']'           {
                                        $$ = makeASTBinaryNode(AST_VETOR_INDEXADO, NULL, $1, $3);
                                        checkIdNodeDeclared($1);
                                        checkIdNodeUsedAs(ARRAY_ID, $1);
                                        setNodeDataType($$, getASTNodeTokenDataType($1));
                                };

int: TK_LIT_INT                 {
                                        // > AST
                                        $$ = makeASTNode(AST_LITERAL, $1);
                                        // > Semantic
                                        setNodeDataType($$, DATATYPE_INT);
                                        // > Code
                                        generateCode($$);
                                };
int_neg: '-' int                {
                                        // > AST
                                        $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2);
                                        // > Semantic
                                        setNodeDataType($$, DATATYPE_INT);
                                        // > Code
                                        generateCode($$);
                                };
float: TK_LIT_FLOAT             { $$ = makeASTNode(AST_LITERAL, $1); setNodeDataType($$, DATATYPE_FLOAT); };
false: TK_LIT_FALSE             { $$ = makeASTNode(AST_LITERAL, $1); setNodeDataType($$, DATATYPE_BOOL); };
true: TK_LIT_TRUE               { $$ = makeASTNode(AST_LITERAL, $1); setNodeDataType($$, DATATYPE_BOOL); };
char: TK_LIT_CHAR               { $$ = makeASTNode(AST_LITERAL, $1); setNodeDataType($$, DATATYPE_CHAR); };
string: TK_LIT_STRING           { $$ = makeASTNode(AST_LITERAL, $1); setNodeDataType($$, DATATYPE_STRING); };

native_type:  TK_PR_INT         { $$ = $1; }
            | TK_PR_FLOAT       { $$ = $1; }
            | TK_PR_CHAR        { $$ = $1; }
            | TK_PR_BOOL        { $$ = $1; }
            | TK_PR_STRING      { $$ = $1; };

/* New Type Declaration */
        
type_def: type_def_id '[' type_def_campos ']'   {
                                                        insertUserTypeTable($1, $3);
                                                        setCurrentScopeToGlobalScope();
                                                };

type_def_id: TK_PR_CLASS TK_IDENTIFICADOR       {
                                                        $$ = $2;
                                                        createNewScope($2->lexeme);    
                                                };

type_def_campos: type_def_campo                                 { $$ = $1; }
                | type_def_campo ':' type_def_campos            { $$ = $1; tree_set_list_next_node($1, $3); };
              
type_def_campo: type_def_camp_enc native_type TK_IDENTIFICADOR  {
                                                                        $$ = makeASTNode(LIST_NODE_USER_TYPE_NAME, $3);
                                                                        setIdTokenDataType($3, $2);
                                                                        setIdType($3, USER_TYPE_ID_FIELD);
                                                                };

type_def_camp_enc:  TK_PR_PROTECTED
                  | TK_PR_PRIVATE
                  | TK_PR_PUBLIC;

/* Global Variables Declaration */

global_def: TK_PR_STATIC global_var
           | global_var
           | TK_PR_STATIC global_arr
           | global_arr;

/* > VAR DECLARATION */
global_var: native_type TK_IDENTIFICADOR                {
                                                                // > Semantic
                                                                setIdTokenDataType($2, $1);
                                                                setIdType($2, VAR_ID);
                                                                // > Code
                                                                setTokenGlobalVarOffset($2);
                                                                allocNewGlobalVar($1);
                                                        }
          | TK_IDENTIFICADOR TK_IDENTIFICADOR           {
                                                                // > Semantic
                                                                setIdTokenDataType($2, DATATYPE_USER_TYPE);
                                                                setIdType($2, USER_TYPE_ID);
                                                                checkUserTypeWasDeclared($1);
                                                                setIdTokenUserDataType($2, $1);
                                                        };

global_arr: native_type TK_IDENTIFICADOR '[' TK_LIT_INT ']'     {
                                                                        // > Semantic 
                                                                        setIdTokenDataType($2, $1);
                                                                        setIdType($2, ARRAY_ID);
                                                                        // > Code
                                                                        setTokenGlobalVarOffset($2);
                                                                        allocNewGlobalArray($1, $4->value.intVal);
                                                                }
        | TK_IDENTIFICADOR TK_IDENTIFICADOR '[' TK_LIT_INT ']'  {
                                                                        setIdTokenDataType($2, DATATYPE_USER_TYPE);
                                                                        setIdType($2, ARRAY_ID);
                                                                        // USER TYPE SEMANTIC CHECK
                                                                        // check $1 is really an user declared type (based on user declared types list)
                                                                        // set $2->userDataType = $1;
                                                                        checkUserTypeWasDeclared($1);
                                                                        setIdTokenUserDataType($2, $1);
                                                                };
/* Function Declaration */

func_dec: func_header block                     {
                                                        // > AST
                                                        $$ = makeASTUnaryNode(AST_FUNCAO, $1, $2);
                                                        // > Semantic
                                                        // Scope ended -> back to global scope
                                                        setCurrentScopeToGlobalScope();
                                                        checkFuncHasReturnCmd($$);
                                                        // > Code
                                                        inheritCodeList($$, $2);
                                                }
        | TK_PR_STATIC func_header block        {       $$ = makeASTUnaryNode(AST_FUNCAO, $2, $3);
                                                        // Scope ended -> back to global scope
                                                        setCurrentScopeToGlobalScope();
                                                        checkFuncHasReturnCmd($$);
                                                };

func_header: func_id '(' params_dec ')'         {       $$ = $1;
                                                        insertFuncTable($1, $3);
                                                };

func_id: native_type TK_IDENTIFICADOR           {       $$ = $2;
                                                        createNewScope($2->lexeme);
                                                        setIdType($2, FUNC_ID);
                                                        setIdTokenDataType($2, $1);
                                                }
        | TK_IDENTIFICADOR TK_IDENTIFICADOR     {
                                                        $$ = $2;
                                                        createNewScope($2->lexeme);
                                                        setIdType($2, FUNC_ID);
                                                        setIdTokenDataType($2, DATATYPE_USER_TYPE);
                                                        // USER TYPE SEMANTIC CHECK
                                                        // check $1 is really an user declared type (based on user declared types list)
                                                        // set $2->userDataType = $1;
                                                        checkUserTypeWasDeclared($1);
                                                        setIdTokenUserDataType($2, $1);
                                                };


params_dec: /* empty */                         { $$ = NULL; }
           | params_dec_list                    { $$ = $1; };

params_dec_list: param_dec                      { $$ = $1; }
                | param_dec ',' params_dec_list { $$ = $1; tree_set_list_next_node($1, $3); };

param_dec: param_dec_mods native_type TK_IDENTIFICADOR          {
                                                                        $$ = makeASTNode(LIST_NODE_PARAM_ID, $3);
                                                                        setIdTokenDataType($3, $2);
                                                                        setIdType($3, VAR_ID);
                                                                }
        | param_dec_mods TK_IDENTIFICADOR TK_IDENTIFICADOR      {
                                                                        $$ = makeASTNode(LIST_NODE_PARAM_ID, $3);
                                                                        setIdTokenDataType($3, DATATYPE_USER_TYPE);
                                                                        setIdType($3, USER_TYPE_ID);
                                                                        // USER TYPE SEMANTIC CHECK
                                                                        // check $2 is really an user declared type (based on user declared types list)
                                                                        // set $3->userDataType = $2;
                                                                        checkUserTypeWasDeclared($2);
                                                                        setIdTokenUserDataType($3, $2);
                                                                };

param_dec_mods: /* empty */
                | TK_PR_CONST;

block:  '{' '}'                 { $$ = makeASTNode(AST_BLOCO, NULL); }
      | '{' commands '}'        {       
                                        if ($2 == NULL) $$ = makeASTNode(AST_BLOCO, NULL);
                                        else $$ = $2;
                                };

commands: command                  { $$ = $1; }
         | command commands        { 
                                        if ($1 != NULL && $2 != NULL) {
                                                // > AST
                                                tree_set_list_next_node($1, $2);
                                                $$ = $1;
                                                // > Code
                                                cmdsCodeListConcat($1, $2);
                                        } else if ($1 == NULL) $$ = $2;
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
/* > VAR DECLARATION */
var_dec:
        /* Declarations with init value (only native types) */
        var_dec_mods native_type id init_var    {       
                                                        // > AST
                                                        $$ = makeASTBinaryNode(AST_INICIALIZACAO, NULL, $3, $4);
                                                        // > Semantic
                                                        setIdNodeIdType($3, VAR_ID); // also checks redeclaration
                                                        setIdNodeTokenDataType($3, $2);
                                                        checkDataTypeMatching($2, getASTNodeTokenDataType($4), 1);
                                                        // > Code
                                                        setNodeLocalVarOffset($3);
                                                        allocNewLocalVar($2);
                                                        generateCode($$);
                                                }
        | native_type id init_var               {       
                                                        // > AST
                                                        $$ = makeASTBinaryNode(AST_INICIALIZACAO, NULL, $2, $3);
                                                        // > Semantic
                                                        setIdNodeIdType($2, VAR_ID); // also checks redeclaration
                                                        setIdNodeTokenDataType($2, $1);
                                                        checkDataTypeMatching($1, getASTNodeTokenDataType($3), 1);
                                                        // > Code
                                                        setNodeLocalVarOffset($2);
                                                        allocNewLocalVar($1);
                                                        generateCode($$);
                                                }

        /* Native type declarations with no init value */
        | var_dec_mods native_type TK_IDENTIFICADOR     {       
                                                                // > AST
                                                                $$ = NULL;
                                                                // > Semantic
                                                                setIdType($3, VAR_ID); // also checks redeclaration
                                                                setIdTokenDataType($3, $2);
                                                                // > Code
                                                                setTokenLocalVarOffset($3);
                                                                allocNewLocalVar($2);
                                                        }
        | native_type TK_IDENTIFICADOR                  {       
                                                                // > AST
                                                                $$ = NULL;
                                                                // > Semantic
                                                                setIdType($2, VAR_ID); // also checks redeclaration
                                                                setIdTokenDataType($2, $1);
                                                                // > Code
                                                                setTokenLocalVarOffset($2);
                                                                allocNewLocalVar($1);
                                                        }
        
        /* Cannot initialize user type variables */
        | var_dec_mods TK_IDENTIFICADOR TK_IDENTIFICADOR        {       $$ = NULL;
                                                                        setIdType($3, USER_TYPE_ID);
                                                                        setIdTokenDataType($3, DATATYPE_USER_TYPE);
                                                                        // USER TYPE SEMANTIC CHECK
                                                                        // check $2 is really an user declared type (based on user declared types list)
                                                                        // set $3->userDataType = $2;
                                                                        checkUserTypeWasDeclared($2);
                                                                        setIdTokenUserDataType($3, $2);
                                                                }
        | TK_IDENTIFICADOR TK_IDENTIFICADOR                     {       $$ = NULL;
                                                                        setIdType($2, USER_TYPE_ID);
                                                                        setIdTokenDataType($2, DATATYPE_USER_TYPE);
                                                                        // USER TYPE SEMANTIC CHECK
                                                                        // check $1 is really an user declared type (based on user declared types list)
                                                                        // set $2->userDataType = $1;
                                                                        checkUserTypeWasDeclared($1);
                                                                        setIdTokenUserDataType($2, $1);
                                                                };

var_dec_mods: TK_PR_STATIC
             | TK_PR_CONST
             | TK_PR_STATIC TK_PR_CONST;

init_var: TK_OC_LE literal      { $$ = $2; }
         | TK_OC_LE id          {
                                        $$ = $2;
                                        checkIdNodeDeclared($2);
                                        checkIdNodeUsedAs(VAR_ID, $2);
                                };

literal:  int                   { $$ = $1; }
        | '+' int               { $$ = $2; }
        | int_neg               { $$ = $1; }
        | float                 { $$ = $1; }
        | '+' float             { $$ = $2; }
        | '-' float             { $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2); setNodeDataType($$, DATATYPE_FLOAT); }
        | false                 { $$ = $1; }
        | true                  { $$ = $1; }
        | char                  { $$ = $1; }
        | string                { $$ = $1; };

/* Shift command - command */

shift_cmd: id TK_OC_SL int              {       $$ = makeASTBinaryNode(AST_SHIFT_LEFT, NULL, $1, $3);
                                                checkIdNodeDeclared($1);
                                                checkIdNodeUsedAs(VAR_ID, $1);
                                        }
          | id TK_OC_SR int             {       $$ = makeASTBinaryNode(AST_SHIFT_RIGHT, NULL, $1, $3);
                                                checkIdNodeDeclared($1);
                                                checkIdNodeUsedAs(VAR_ID, $1);
                                        };

/* Assignment - command */

assig_cmd: id '=' unary_plus exp                {
                                                        // > AST
                                                        $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $4);
                                                        // > Semantic
                                                        checkIdNodeDeclared($1);
                                                        checkIdNodeUsedAsMultiple(VAR_ID, USER_TYPE_ID, $1);
                                                        checkUserDataTypeMatching($1, $4);
                                                        // > Code
                                                        generateCode($$);
                                                }
          | array '=' unary_plus exp            {       
                                                        // > AST
                                                        $$ = makeASTBinaryNode(AST_ATRIBUICAO, NULL, $1, $4);
                                                        // > Semantic
                                                        // Declaration check, id use as array check, set node dataType => already done in 'array' rule
                                                        checkUserDataTypeMatching($1->first, $4);
                                                        // > Code
                                                        generateCode($$);
                                                }
          | id '.' id '=' unary_plus exp        {       $$ = makeASTTernaryNode(AST_ATRIBUICAO, NULL, $1, $3, $6);
                                                        checkIdNodeDeclared($1);
                                                        checkIdNodeUsedAs(USER_TYPE_ID, $1);
                                                        // USER TYPE SEMANTIC CHECK
                                                        // set $3 dataType (based on user declared types list)
                                                        // checkDataTypeMatching(getASTNodeDataType($3), getASTNodeDataType($6), 1);
                                                        setUserTypeFieldDataType($1, $3); // also check field($3) is valid
                                                        checkDataTypeMatching(getASTNodeTokenDataType($3), getASTNodeDataType($6), 1);
                                                };

unary_plus: /* empty */
           | '+';

/* Input and output - command */

io_cmd: TK_PR_INPUT exp                 { $$ = makeASTUnaryNode(AST_INPUT, NULL, $2); }
       | TK_PR_OUTPUT exps_list         { $$ = makeASTUnaryNode(AST_OUTPUT, NULL, $2); };

/* Function call - command */

func_call: id '(' params ')'    {       if ($3 != NULL) $$ = makeASTBinaryNode(AST_CHAMADA_DE_FUNCAO, NULL, $1, $3);
                                        else $$ = makeASTUnaryNode(AST_CHAMADA_DE_FUNCAO, NULL, $1);
                                        checkIdNodeDeclared($1);
                                        checkIdNodeUsedAs(FUNC_ID, $1);
                                        setNodeDataType($$, getASTNodeTokenDataType($1));
                                        checkFuncCall($$);
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

return_cmd: TK_PR_RETURN exp            {       $$ = makeASTUnaryNode(AST_RETURN, NULL, $2);
                                                checkFuncReturnDataType($$);
                                        };

break_cmd: TK_PR_BREAK                  { $$ = makeASTNode(AST_BREAK, NULL); };

continue_cmd: TK_PR_CONTINUE            { $$ = makeASTNode(AST_CONTINUE, NULL); };

case_cmd: TK_PR_CASE int ':'            { $$ = makeASTUnaryNode(AST_CASE, NULL, $2); }
        | TK_PR_CASE '+' int ':'        { $$ = makeASTUnaryNode(AST_CASE, NULL, $3); }
        | TK_PR_CASE int_neg ':'        { $$ = makeASTUnaryNode(AST_CASE, NULL, $2); };

/* Pipes - command */

pipe_exp:  pipe_pg_step0 func_call              { $$ = makeASTBinaryNode(AST_PIPE_G, NULL, $1, $2); endParsingPipeExp(); }
         | pipe_pb_step0 func_call              { $$ = makeASTBinaryNode(AST_PIPE_B, NULL, $1, $2); endParsingPipeExp(); }
         | pipe_pg_step1 func_call              { $$ = makeASTBinaryNode(AST_PIPE_G, NULL, $1, $2); endParsingPipeExp(); }
         | pipe_pb_step1 func_call              { $$ = makeASTBinaryNode(AST_PIPE_B, NULL, $1, $2); endParsingPipeExp(); };

pipe_pg_step0: func_call TK_OC_PG               { $$ = $1; setCurrParsingPipeExp(getASTNodeDataType($1)); };
pipe_pb_step0: func_call TK_OC_PB               { $$ = $1; setCurrParsingPipeExp(getASTNodeDataType($1)); };
pipe_pg_step1: pipe_exp TK_OC_PG                { $$ = $1; setCurrParsingPipeExp(getASTNodeDataType($1->last)); };
pipe_pb_step1: pipe_exp TK_OC_PB                { $$ = $1; setCurrParsingPipeExp(getASTNodeDataType($1->last)); };


/* Flow Control - Commands */

if_stm:  if_exp TK_PR_THEN block                        {
                                                                // > AST
                                                                $$ = makeASTBinaryNode(AST_IF_ELSE, NULL, $1, $3);
                                                                // > Code
                                                                generateCode($$);
                                                        }
       | if_exp TK_PR_THEN block TK_PR_ELSE block       {       
                                                                // > AST
                                                                $$ = makeASTTernaryNode(AST_IF_ELSE, NULL, $1, $3, $5);
                                                                // > Code
                                                                generateCode($$);
                                                        };

if_exp: TK_PR_IF '(' exp ')'                            {       
                                                                $$ = $3;
                                                                checkExpNodeDataTypeIsBool($3);
                                                        };

foreach: TK_PR_FOREACH '(' id ':' exps_list ')' block   {       
                                                                $$ = makeASTTernaryNode(AST_FOREACH, NULL, $3, $5, $7);
                                                                checkIdNodeDeclared($3);
                                                                checkIdNodeUsedAs(VAR_ID, $3);
                                                        };

while: TK_PR_WHILE '(' exp ')' TK_PR_DO block           {
                                                                // > AST
                                                                $$ = makeASTBinaryNode(AST_WHILE_DO, NULL, $3, $6);
                                                                // > Semantic
                                                                checkExpNodeDataTypeIsBool($3);
                                                                // > Code
                                                                generateCode($$);
                                                        };

do_while: TK_PR_DO block TK_PR_WHILE '(' exp ')'        {
                                                                // > AST
                                                                $$ = makeASTBinaryNode(AST_DO_WHILE, NULL, $2, $5);
                                                                // > Semantic
                                                                checkExpNodeDataTypeIsBool($5);
                                                                // > Code
                                                                generateCode($$);
                                                        };

switch: TK_PR_SWITCH '(' exp ')' block                          {       $$ = makeASTBinaryNode(AST_SWITCH, NULL, $3, $5);
                                                                        checkExpNodeDataTypeIsInt($3);
                                                                };

for: TK_PR_FOR '(' cmd_list ':' exp ':' cmd_list ')' block      {       
                                                                        // > AST
                                                                        $$ = makeASTQuaternaryNode(AST_FOR, NULL, $3, $5, $7, $9);
                                                                        // > Semantic
                                                                        checkExpNodeDataTypeIsBool($5);
                                                                        // > Code
                                                                        generateCode($$);
                                                                };

cmd_list: cmd                           { $$ = $1; }
         | cmd ',' cmd_list             {
                                                // > AST
                                                tree_set_list_next_node($1, $3);
                                                $$ = $1;  
                                                // > Code
                                                cmdsCodeListConcat($1, $3);
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

exp:  array                     {
                                        // > AST
                                        $$ = $1;
                                        // > Code
                                        generateCode($$);
                                }
    | func_call                 { $$ = $1; }
    | int                       { $$ = $1; }
    | float                     { $$ = $1; }
    | string                    { $$ = $1; }
    | char                      { $$ = $1; }
    | true                      { $$ = $1; }
    | false                     { $$ = $1; }
    | '(' exp ')'               { $$ = $2; } 
    | id                        {       
                                        // > AST
                                        $$ = $1;
                                        // > Semantic
                                        checkIdNodeDeclared($1);
                                        checkIdNodeUsedAsMultiple(VAR_ID, USER_TYPE_ID, $1);
                                        setNodeDataType($$, getASTNodeTokenDataType($1));
                                        setNodeUserDataType($$, getTokenInfoFromIdNode($1)->userDataType);
                                        // > Code
                                        generateCode($$);
                                }         
    | logicExp                  {       
                                        // > AST
                                        $$ = $1;
                                        // > Semantic
                                        int resultDataType = checkLogicExpDataTypeMatching($$->first, $$->last);
                                        setNodeDataType($$, resultDataType);
                                        // > Code
                                        generateCode($$);
                                }
    | arimExp                   {       
                                        // > AST
                                        $$ = $1;
                                        // > Semantic
                                        int resultDataType = checkArimExpDataTypeMatching($$->first, $$->last);
                                        setNodeDataType($$, resultDataType);
                                        // > Code
                                        generateCode($$);
                                }
    | compExp                   {       
                                        // > AST
                                        $$ = $1;
                                        // > Semantic
                                        int resultDataType = checkCompExpDataTypeMatching($$->first, $$->last);
                                        setNodeDataType($$, resultDataType);
                                        // > Code
                                        generateCode($$);
                                }
    | '-' exp                   {       
                                        // > AST
                                        $$ = makeASTUnaryNode(AST_ARIM_INVERSAO, NULL, $2);
                                        // > Semantic
                                        int resultDataType = checkArimExpDataTypeMatching($2, NULL);
                                        setNodeDataType($$, resultDataType);
                                        // > Code
                                        generateCode($$);
                                }
    | '!' exp                   {       $$ = makeASTUnaryNode(AST_LOGICO_COMP_NEGACAO, NULL, $2);
                                        int resultDataType = checkLogicExpDataTypeMatching($2, NULL);
                                        setNodeDataType($$, resultDataType);
                                }
    | pipe_exp                  {       $$ = $1;
                                        int resultDataType = getASTNodeDataType($1->last);
                                        setNodeDataType($$, resultDataType);
                                }
    | '.'                       { $$ = makeASTNode(AST_DOT_PARAM, NULL); /* Keep dataType = DATATYPE_UNDEF */ };

compExp:  exp TK_OC_EQ exp      { $$ = makeASTBinaryNode(AST_LOGICO_COMP_IGUAL, NULL, $1, $3); }
        | exp TK_OC_NE exp      { $$ = makeASTBinaryNode(AST_LOGICO_COMP_DIF, NULL, $1, $3); }
        | exp TK_OC_GE exp      { $$ = makeASTBinaryNode(AST_LOGICO_COMP_GE, NULL, $1, $3); }
        | exp TK_OC_LE exp      { $$ = makeASTBinaryNode(AST_LOGICO_COMP_LE, NULL, $1, $3); }
        | exp '>' exp           { $$ = makeASTBinaryNode(AST_LOGICO_COMP_G, NULL, $1, $3); }
        | exp '<' exp           { $$ = makeASTBinaryNode(AST_LOGICO_COMP_L, NULL, $1, $3); };

arimExp:  exp '+' exp           { $$ = makeASTBinaryNode(AST_ARIM_SOMA, NULL, $1, $3); }
        | exp '-' exp           { $$ = makeASTBinaryNode(AST_ARIM_SUBTRACAO, NULL, $1, $3); }
        | exp '/' exp           { $$ = makeASTBinaryNode(AST_ARIM_DIVISAO, NULL, $1, $3); }
        | exp '*' exp           { $$ = makeASTBinaryNode(AST_ARIM_MULTIPLICACAO, NULL, $1, $3); }
        | exp '%' exp           { $$ = makeASTBinaryNode(AST_ARIM_MOD, NULL, $1, $3); };

logicExp:  exp TK_OC_AND exp    { $$ = makeASTBinaryNode(AST_LOGICO_E, NULL, $1, $3); }
         | exp TK_OC_OR exp     { $$ = makeASTBinaryNode(AST_LOGICO_OU, NULL, $1, $3); };

exps_list:  exp                 { $$ = $1; }
          | exp ',' exps_list   {
                                        tree_set_list_next_node($1, $3);
                                        $$ = $1;
                                };

%%
