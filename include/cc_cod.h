#include "cc_ast.h"

int getSizeOf(int type);
int generateTempReg();

void generateCode(comp_tree_t *node);
void generateLiteralCode(comp_tree_t *node);
void generateArimCode(comp_tree_t *node, const char *op);