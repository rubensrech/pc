/*
  cc_ast.h

  Este arquivo contém as constantes para os tipos dos nós da AST.
*/

#ifndef __CC_AST_H
#define __CC_AST_H

#define AST_PROGRAMA             0
#define AST_FUNCAO               1
//Comandos
#define AST_IF_ELSE              2
#define AST_DO_WHILE             3
#define AST_WHILE_DO             4
#define AST_INPUT                5
#define AST_OUTPUT               6
#define AST_ATRIBUICAO           7
#define AST_RETURN               8
#define AST_BLOCO                9
//Condição, Saída, Expressão
#define AST_IDENTIFICADOR       10
#define AST_LITERAL             11
#define AST_ARIM_SOMA           12
#define AST_ARIM_SUBTRACAO      13
#define AST_ARIM_MULTIPLICACAO  14
#define AST_ARIM_DIVISAO        15
#define AST_ARIM_INVERSAO       16 // - (operador unário -)
#define AST_LOGICO_E            17 // &&
#define AST_LOGICO_OU           18 // ||
#define AST_LOGICO_COMP_DIF     19 // !=
#define AST_LOGICO_COMP_IGUAL   20 // ==
#define AST_LOGICO_COMP_LE      21 // <=
#define AST_LOGICO_COMP_GE      22 // >=
#define AST_LOGICO_COMP_L       23 // <
#define AST_LOGICO_COMP_G       24 // >
#define AST_LOGICO_COMP_NEGACAO 25 // !
#define AST_VETOR_INDEXADO      26 // para var[exp] quando o índice exp é acessado no vetor var
#define AST_CHAMADA_DE_FUNCAO   27
#define AST_SHIFT_RIGHT         28
#define AST_SHIFT_LEFT          29
#define AST_BREAK               30
#define AST_CONTINUE            31
#define AST_CASE                32
#define AST_FOR                 33
#define AST_FOREACH             34
#define AST_SWITCH              35

#define AST_ARIM_MOD            36
#define AST_DOT_PARAM           37
#define AST_PIPE_G              38
#define AST_PIPE_B              39
#define AST_INICIALIZACAO       40

#define GV_OUT_FILE "saida.dot"

#include "cc_tree.h"
#include "cc_table.h"

typedef struct astNodeInfo {
  int type;
  TokenInfo *tokenInfo;
  int dataType;
  char *userDataType;
} AstNodeInfo;

void initAST();
void freeAST();
void freeASTSubTree(comp_tree_t *tree);
comp_tree_t *makeASTNode(int type, TokenInfo *token);
comp_tree_t *makeASTUnaryNode(int type, TokenInfo *token, comp_tree_t *node1);
comp_tree_t *makeASTBinaryNode(int type, TokenInfo *token, comp_tree_t *node1, comp_tree_t *node2);
comp_tree_t *makeASTTernaryNode(int type, TokenInfo *token, comp_tree_t *node1, comp_tree_t *node2, comp_tree_t *node3);
comp_tree_t *makeASTQuaternaryNode(int type, TokenInfo *token, comp_tree_t *node1, comp_tree_t *node2, comp_tree_t *node3, comp_tree_t *node4);

TokenInfo *getASTNodeTokenInfo(comp_tree_t *node);

#endif
