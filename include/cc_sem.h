#ifndef __CC_SEM_H
#define __CC_SEM_H

#include "cc_table.h"
#include "cc_tree.h"
#include "cc_ast.h"
#include "cc_misc.h"
#include "cc_dict.h"

#define MAX_ERROR_MSG_SIZE          100
#define ENABLE_RETURN_CHECK         0

#define LIST_NODE_PARAM_ID             -1
#define LIST_NODE_USER_TYPE_NAME       -2

/* Data types */
#define DATATYPE_USER_TYPE          -1  // used for user type name identifier
#define DATATYPE_UNDEF              0
#define DATATYPE_FLOAT              1
#define DATATYPE_INT                2
#define DATATYPE_BOOL               3
#define DATATYPE_STRING             4
#define DATATYPE_CHAR               5

/* Identifiers types */
#define ID_TYPE_UNDEF               0 // still not defined
#define VAR_ID                      1 // int ID;
#define ARRAY_ID                    2 // int ID[exp];
#define USER_TYPE_ID                4 // Pessoa ID;
#define USER_TYPE_ID_FIELD          5 // pessoa.ID = exp;
#define FUNC_ID                     6 // ID();
#define USER_TYPE_DEF               7 // ID used for user defined types

/* Semantic errors */
#define IKS_SUCCESS                 0 //caso não houver nenhum tipo de erro
/* Verificação de declarações */
#define IKS_ERROR_UNDECLARED        1 //identificador não declarado
#define IKS_ERROR_DECLARED          2 //identificador já declarado
/* Uso correto de identificadores */
#define IKS_ERROR_VARIABLE          3 //identificador deve ser utilizado como variável
#define IKS_ERROR_VECTOR            4 //identificador deve ser utilizado como vetor
#define IKS_ERROR_FUNCTION          5 //identificador deve ser utilizado como função
#define IKS_ERROR_WRONG_TYPE        12 //tipo incompatível
#define IKS_ERROR_USER_TYPE         13 //identificador deve ser utilizado como tipo de usuário
/* Argumentos e parâmetros */
#define IKS_ERROR_MISSING_ARGS      9  //faltam argumentos 
#define IKS_ERROR_EXCESS_ARGS       10 //sobram argumentos 
#define IKS_ERROR_WRONG_TYPE_ARGS   11 //argumentos incompatíveis
#define IKS_ERROR_WRONG_DOT_PARAM   14 //param '.' usado fora do context de pipe expression
#define IKS_ERROR_DOT_PARAM_TYPE    15 //param '.' recebe retorno de função com tipo incompatível
#define IKS_ERROR_DOT_PARAM_MISS    20 //param '.' não encontrado em chamada de func dentro de pipe
/* Retorno de funções */
#define IKS_ERROR_RETURN_TYPE       16 //função retorna expressão de tipo incompatível com sua declaração
#define IKS_ERROR_NO_RETURN         17 //função sem return
/* User types */
#define IKS_ERROR_UNDECLARED_TYPE   18 //uso de tipo nao declarado
#define IKS_ERROR_UNKNOWN_TYPE_FIELD 19 //uso de campo desconhecido em variavel de tipo declarado
/* Break / continue */
#define IKS_ERROR_INVALID_BREAK     21 // uso de break fora de loop ou switch
#define IKS_ERROR_INVALID_CONTINUE  22 // uso de continue fora de loop



typedef struct funcDesc {
    char *id;
    int returnDataType;
    char *returnUserDataType;
    comp_tree_t *params;
} FuncDesc;

typedef struct userTypeDesc {
    char *typeName;
    comp_tree_t *fields;
} UserTypeDesc;

typedef struct pipeExpParseInfo {
    int isParsingPipeExp;
    int lastFuncCallRetType;
} PipeExpParseInfo;

#define GLOBAL_SCOPE_ID     "#GLOBAL#"

typedef struct scopeInfo {
    int isCurrScopeGlobal;
    char *currScopeId;
} ScopeInfo;

/* Data type */
void setIdTokenDataType(TokenInfo *id, int dataType);
void setIdNodeTokenDataType(comp_tree_t *node, int dataType);
int getASTNodeTokenDataType(comp_tree_t *node);
int getASTNodeDataType(comp_tree_t *node);
int checkDataTypeMatching(int dataType1, int dataType2, int shouldThrow);
int checkArimExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2);
int checkLogicExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2);
int checkCompExpDataTypeMatching(comp_tree_t *exp1, comp_tree_t *exp2);
void checkExpNodeDataTypeIsBool(comp_tree_t *exp);
void checkExpNodeDataTypeIsInt(comp_tree_t *exp);
void setNodeDataType(comp_tree_t *node, int dataType);

/* Scope control */
void setCurrentScopeToGlobalScope();
void createNewScope(char *id);
char *getCurrentScope();

/* ID: Declaration and Use */
void checkIdDeclared(TokenInfo *id);
void checkIdNodeDeclared(comp_tree_t *node);
void setIdType(TokenInfo *id, int idType);
void setIdNodeIdType(comp_tree_t *node, int idType);
void checkIdUsedAs(int usedAs, TokenInfo *id);
void checkIdNodeUsedAs(int usedAs, comp_tree_t *node);
void checkIdUsedAsMultiple(int usedAs1, int usedAs2, TokenInfo *id);
void checkIdNodeUsedAsMultiple(int usedAs1, int usedAs2, comp_tree_t *node);
TokenInfo *searchIdInGlobalScope(char *id);

/* Functions */
void initFuncTable();
void freeFuncTable();
void freeFuncDescriptor(FuncDesc *descriptor);
void printFuncTable();
void insertFuncTable(TokenInfo *idInfo, comp_tree_t *params);
int countFuncParameters(comp_tree_t *params);
void checkFuncCall(comp_tree_t *funcAST);
void checkFuncHasReturnCmd(comp_tree_t *funcNode);
void checkFuncReturnDataType(comp_tree_t *returnNode);

/* Pipe Expressions */
void setCurrParsingPipeExp(int lastFuncCallRetType);
void endParsingPipeExp();
int setPipeExpDotParamDataType(comp_tree_t *dotParamNode);

/* User types */
void initUserTypesTable();
void freeUserTypesTable();
void freeUserTypeDescriptor(UserTypeDesc *descriptor);
void printUserTypesTable();
int countUserTypeFields(comp_tree_t *fields);
void insertUserTypeTable(TokenInfo *typeNameIdInfo, comp_tree_t *fields);
void checkUserTypeWasDeclared(TokenInfo *typeNameIdInfo);
void setIdTokenUserDataType(TokenInfo *id, TokenInfo *typeNameId);
void setUserTypeFieldDataType(comp_tree_t *varNode, comp_tree_t *fieldNode);
TokenInfo *lookUpFieldInUserTypeFields(char *fieldName, comp_tree_t *fields);
void setNodeUserDataType(comp_tree_t *node, char *userDataType);
void checkUserDataTypeMatching(comp_tree_t *node1, comp_tree_t *node2);

/* Auxiliary */
void throwSemanticError(char *errorMsg, int errorCode);
int inArray(int array[], int size, int val);
TokenInfo *getTokenInfoFromIdNode(comp_tree_t *node);

/* Break / Continue */
void enteredLoop();
void leftLoop();
void enteredSwitch();
void leftSwitch();
void checkBreakIsValid();
void checkContinueIsValid();

#endif