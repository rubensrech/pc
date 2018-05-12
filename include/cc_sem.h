#include "cc_table.h"
#include "cc_tree.h"
#include "cc_ast.h"
#include "cc_misc.h"

void setIdDataType(TokenInfo *id, int dataType);
void setIdNodeDataType(comp_tree_t *node, int dataType);

int getASTNodeTokenDataType(comp_tree_t *node);

int checkDataTypeMatching(int idDataType, int litDataType);