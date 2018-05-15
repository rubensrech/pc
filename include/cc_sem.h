#ifndef __CC_SEM_H
#define __CC_SEM_H

#include "cc_table.h"
#include "cc_tree.h"
#include "cc_ast.h"
#include "cc_misc.h"
#include "cc_dict.h"

#define MAX_ERROR_MSG_SIZE          100

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

/* Semantic errors */
#define IKS_SUCCESS                 0 //caso não houver nenhum tipo de erro
/* Verificação de declarações */
#define IKS_ERROR_UNDECLARED        1 //identificador não declarado
#define IKS_ERROR_DECLARED          2 //identificador já declarado
/* Uso correto de identificadores */
#define IKS_ERROR_VARIABLE          3 //identificador deve ser utilizado como variável
#define IKS_ERROR_VECTOR            4 //identificador deve ser utilizado como vetor
#define IKS_ERROR_FUNCTION          5 //identificador deve ser utilizado como função
#define IKS_ERROR_INCOMP_TYPES      12 //identificador utilizado com tipo incompatível
#define IKS_ERROR_USER_TYPE         13 //identificador deve ser utilizado como tipo de usuário
/* Argumentos e parâmetros */
#define IKS_ERROR_MISSING_ARGS      9  //faltam argumentos 
#define IKS_ERROR_EXCESS_ARGS       10 //sobram argumentos 
#define IKS_ERROR_WRONG_TYPE_ARGS   11 //argumentos incompatíveis

typedef struct funcDesc {
    char *id;
    int returnDataType;
    comp_tree_t *params;
} FuncDesc;

/* Data type */
void setIdTokenDataType(TokenInfo *id, int dataType);
void setIdNodeTokenDataType(comp_tree_t *node, int dataType);
int getASTNodeTokenDataType(comp_tree_t *node);
int getASTNodeDataType(comp_tree_t *node);
void checkDataTypeMatching(int idDataType, int litDataType);
void setNodeDataType(comp_tree_t *node, int dataType);

/* ID: Declaration and Use */
void checkIdDeclared(TokenInfo *id);
void checkIdNodeDeclared(comp_tree_t *node);
void setIdType(TokenInfo *id, int idType);
void setIdNodeIdType(comp_tree_t *node, int idType);
void checkIdUsedAs(int usedAs, TokenInfo *id);
void checkIdNodeUsedAs(int usedAs, comp_tree_t *node);
TokenInfo *searchIdInGlobalScope(char *id);

/* Functions */
void initFuncTable();
void freeFuncTable();
void freeFuncDescriptor(FuncDesc *descriptor);
void freeParamsList(comp_tree_t *list);
void printFuncTable();
void insertFuncTable(TokenInfo *idInfo, comp_tree_t *params);
int countFuncParameters(comp_tree_t *params);
void checkFuncCall(comp_tree_t *funcAST);

/* Auxiliary */
void throwSemanticError(char *errorMsg, int errorCode);

#endif