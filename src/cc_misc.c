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
  return getLineNumber();
}

void yyerror (char const *mensagem)
{
  fprintf (stderr, "%s - line %d\n", mensagem, getLineNumber());
  exit(1);
}

void main_init (int argc, char **argv)
{
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

  // Hash already contais same key => update line
  oldInfo = dict_get(symbolsTable, key);
  if (oldInfo != NULL) {
    oldInfo->line = num_lines;
    free(token);
    free(info);
    return oldInfo;
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