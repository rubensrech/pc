#include "cc_table.h"
#include "cc_tree.h"
#include "cc_ast.h"
#include "cc_misc.h"

#define IKS_SUCCESS                 0 //caso não houver nenhum tipo de erro

/* Verificação de declarações */
#define IKS_ERROR_UNDECLARED        1 //identificador não declarado
#define IKS_ERROR_DECLARED          2 //identificador já declarado

/* Uso correto de identificadores */
#define IKS_ERROR_VARIABLE          3 //identificador deve ser utilizado como variável
#define IKS_ERROR_VECTOR            4 //identificador deve ser utilizado como vetor
#define IKS_ERROR_FUNCTION          5 //identificador deve ser utilizado como função
#define IKS_ERROR_INCOMP_TYPES      12 //identificador utilizado com tipo incompatível    

/* Argumentos e parâmetros */
#define IKS_ERROR_MISSING_ARGS      9  //faltam argumentos 
#define IKS_ERROR_EXCESS_ARGS       10 //sobram argumentos 
#define IKS_ERROR_WRONG_TYPE_ARGS   11 //argumentos incompatívei

void setIdDataType(TokenInfo *id, int dataType);
void setIdNodeDataType(comp_tree_t *node, int dataType);

int getASTNodeTokenDataType(comp_tree_t *node);

void checkDataTypeMatching(int idDataType, int litDataType);