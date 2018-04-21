#include <string.h>
#include <stdlib.h>
#include "cc_misc.h"
#include "cc_dict.h"
#include "cc_ast.h"
#include "main.h"

extern int num_lines;
extern char *yytext;

comp_dict_t *symbolsTable;
comp_tree_t *ast;

int getLineNumber(void) {
  return num_lines;
}

int comp_get_line_number (void)
{
  //implemente esta função
  return getLineNumber();
}

void yyerror (char const *mensagem)
{
  // fprintf (stderr, "%s\n", mensagem); //altere para que apareça a linha
  fprintf (stderr, "%s - line %d\n", mensagem, getLineNumber());
  exit(1);
}

void teste() {
  AstNodeInfo *sum = malloc(sizeof(struct astNodeInfo));
  AstNodeInfo *child = malloc(sizeof(struct astNodeInfo));
  TokenInfo *childInfo = malloc(sizeof(struct tokenInfo));
  char val[2] = "5";

  sum->type = AST_ARIM_SOMA;
  sum->tokenInfo = NULL;

  childInfo->line = 10;
  childInfo->type = POA_LIT_INT;
  childInfo->lexeme = val;
  childInfo->value.intVal = 5;

  child->type = AST_LITERAL;
  child->tokenInfo = childInfo;

  ast = tree_make_binary_node(sum, 
              tree_make_node(child), 
              tree_make_node(child));

}

void main_init (int argc, char **argv)
{
  //implemente esta função com rotinas de inicialização, se necessário

  // Initialize symbols table
  symbolsTable = dict_new();
  ast = tree_new();
}

void main_finalize (void)
{

  // comp_print_table();
  gv_close();

  freeSymbolsTable();
}

void comp_print_table (void)
{
  //para cada entrada na tabela de símbolos
  //Etapa 1: chame a função cc_dict_etapa_1_print_entrada
  //implemente esta função

  int i;
  struct comp_dict_item *entry;
  TokenInfo *info;
  char token[MAX_HASH_KEY_SIZE];

  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        info = entry->value;

        sscanf(entry->key, "%s $$ %*d", token);
        cc_dict_etapa_2_print_entrada(token, info->line, info->type);
        
        entry = entry->next;
      }
  }
}

// MARK: - Auxiliary functions

// > Symbols Table

void freeSymbolsTable() {
  int i;
  struct comp_dict_item *entry, *next;
  TokenInfo *info;

  for (i = 0; i < symbolsTable->size; i++) {
    entry = symbolsTable->data[i];
      while (entry != NULL) {
        next = entry->next;
        info = dict_remove(symbolsTable, entry->key);
        freeTokenInfo(info);
        entry = next;
      }
  }

  dict_free(symbolsTable);
}

void freeTokenInfo(TokenInfo *info) {
  free(info->lexeme);
  free(info);
}

void removeQuotes(char *token) {
  int i;
  if (token[0] == '\'' || token[0] == '"') {
    token[strlen(token)-1] = '\0';
    for (i = 0; i < strlen(token); i++) {
      token[i] = token[i+1];
    }
  }
}

TokenInfo *addSymbolsTable(int tokenType) {
  TokenInfo *info = malloc(sizeof(struct tokenInfo));
  TokenInfo *oldInfo;
  char key[MAX_HASH_KEY_SIZE+1];
  char *token;
  
  token = strdup(yytext);
  
  info->line = num_lines;
  info->type = tokenType;

  // Get token real value
  switch (tokenType) {
    case POA_LIT_STRING:
      removeQuotes(token);
      info->value.strVal = token;
      break;
    case POA_LIT_CHAR:
      removeQuotes(token);
      info->value.charVal = token[0];
      break;
    case POA_IDENT:
      info->value.strVal = token;
      break;
    case POA_LIT_BOOL:
      info->value.boolVal = !strcmp(token, "true");
      break;
    case POA_LIT_FLOAT:
      info->value.floatVal = atof(token);
      break;
    case POA_LIT_INT:
      info->value.intVal = atoi(token);
      break;
  }

  // Generate hash table key (TOKEN $$ TYPE)
  snprintf(key, MAX_HASH_KEY_SIZE, "%s $$ %d", token, tokenType);

  // info->lexeke must be freed later
  info->lexeme = token;

  // Hash already contais same key => remove old entry
  if (dict_get(symbolsTable, key) != NULL) {
    oldInfo = dict_remove(symbolsTable, key);
    // Update AST pointer !!!
    freeTokenInfo(oldInfo);
  }

  // Key is duplicated (strdup) inside dict_put function
  return (TokenInfo*)dict_put(symbolsTable, key, info);
}

// > AST

comp_tree_t *makeASTNode(int type, TokenInfo *token) {
  AstNodeInfo *nodeInfo = malloc(sizeof(struct astNodeInfo));
  nodeInfo->type = type;
  nodeInfo->tokenInfo = token;
  return tree_make_node(nodeInfo);
}

comp_tree_t *makeASTUnaryNode(int type, TokenInfo *token, comp_tree_t *node1) {
  comp_tree_t *newnode = makeASTNode(type, token);
	tree_insert_node(newnode, node1);
	return newnode;
}

comp_tree_t *makeASTBinaryNode(int type, TokenInfo *token, comp_tree_t *node1, comp_tree_t *node2) {
  comp_tree_t *newnode = makeASTNode(type, token);
	tree_insert_node(newnode, node1);
  tree_insert_node(newnode, node2);
	return newnode;
}

comp_tree_t *makeASTTernaryNode(int type, TokenInfo *token, comp_tree_t *node1, comp_tree_t *node2, comp_tree_t *node3) {
  comp_tree_t *newnode = makeASTNode(type, token);
	tree_insert_node(newnode, node1);
  tree_insert_node(newnode, node2);
  tree_insert_node(newnode, node3);
	return newnode;
}