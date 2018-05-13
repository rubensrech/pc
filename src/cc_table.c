#include <stdlib.h>
#include <string.h>
#include "cc_table.h"
#include "main.h"

extern char *yytext;
extern int num_lines;
extern int scope;

comp_dict_t *symbolsTable;

void initSymbolsTable() {
    symbolsTable = dict_new();
}

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
  info->scope = scope;
  info->idType = ID_TYPE_UNDEF;

  // Get token value
  switch (tokenType) {
    case POA_LIT_STRING:
      removeQuotes(token);
      info->value.strVal = token;
      info->dataType = DATATYPE_STRING;
      break;
    case POA_LIT_CHAR:
      removeQuotes(token);
      info->value.charVal = token[0];
      info->dataType = DATATYPE_CHAR;
      break;
    case POA_IDENT:
      info->value.strVal = token;
      info->dataType = DATATYPE_UNDEF;
      break;
    case POA_LIT_BOOL:
      info->value.boolVal = !strcmp(token, "true");
      info->dataType = DATATYPE_BOOL;
      break;
    case POA_LIT_FLOAT:
      info->value.floatVal = atof(token);
      info->dataType = DATATYPE_FLOAT;
      break;
    case POA_LIT_INT:
      info->value.intVal = atoi(token);
      info->dataType = DATATYPE_INT;
      break;
  }

  if (tokenType == POA_IDENT) {
    // Unique table entry for id in current scope
    // Generate hash table key (TOKEN $$ TYPE && SCOPE)
    snprintf(key, MAX_HASH_KEY_SIZE, "%s $$ %d $$ %d", token, tokenType, scope);
  } else {
    // Generate hash table key (TOKEN $$ TYPE)
    snprintf(key, MAX_HASH_KEY_SIZE, "%s $$ %d", token, tokenType);
  }

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

TokenInfo *lookUpForIdInSymbolsTable(char *id, int scope) {
  TokenInfo *tokenInfo;
  char key[MAX_HASH_KEY_SIZE+1];
  
  // Generate hash table key (TOKEN $$ TYPE && SCOPE)
  snprintf(key, MAX_HASH_KEY_SIZE, "%s $$ %d $$ %d", id, POA_IDENT, scope);

  tokenInfo = dict_get(symbolsTable, key);
  return tokenInfo;
}

void printSymbolsTable() {
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