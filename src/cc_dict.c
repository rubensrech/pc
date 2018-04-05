// Copyright (c) 2016 Lucas Nodari 
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cc_dict.h"

#define ERRO(MENSAGEM) { fprintf (stderr, "[cc_dict, %s] %s.\n", __FUNCTION__, MENSAGEM); abort(); }


// one-at-a-time-hash, Jenkins, 2006
static int generate_hash(char *key, int limit)
{
  unsigned int hash;
  for (hash = 0; *key != '\0'; ++key) {
    hash += *key;
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash % limit;
}

static comp_dict_item_t *dict_item_new()
{
  comp_dict_item_t *item = malloc(sizeof(comp_dict_item_t));
  item->key = NULL;
  item->value = NULL;
  item->next = NULL;
  return item;
}

static void dict_item_free_item(comp_dict_item_t * item)
{
  free(item->key);
  free(item);
}

static void dict_item_free(comp_dict_item_t * item)
{
  if (item == NULL)
    return;

  comp_dict_item_t *ptr = item;
  while (ptr != NULL) {
    ptr = item->next;
    dict_item_free_item(item);
    item = ptr;
  }
}

static void *dict_item_remove(comp_dict_item_t * target, char *key)
{
  comp_dict_item_t *prev = target;
  while (target != NULL && target->key != NULL) {
    if (strcmp(target->key, key) == 0)
      break;
    prev = target;
    target = target->next;
  }
  if (target == NULL)
    return NULL;                // key não existe
  prev->next = target->next;

  void *data = target->value;
  dict_item_free_item(target);
  return data;
}

static int dict_item_insert(comp_dict_item_t * target,
                            comp_dict_item_t * new)
{
  if (target == NULL)
    return 0;
  while (target->next != NULL)
    target = target->next;
  target->next = new;

  return 1;
}

static comp_dict_item_t *dict_item_get(comp_dict_item_t * first, char *key)
{
  while (first != NULL && first->key != NULL) {
    if (strcmp(first->key, key) == 0)
      return first;
    first = first->next;
  }
  return NULL;
}

static int dict_item_list_print(comp_dict_item_t * item)
{
  int qtd = 0;
  while (item != NULL) {
    ++qtd;
    printf(" %s %p\n", item->key, item->value);
    item = item->next;
  }
  return qtd;
}

void dict_debug_print(comp_dict_t * dict)
{
  int i, l;
  int qtd = 0;
  printf("Dict [%d/%d]\n", dict->occupation, dict->size);
  for (i = 0, l = dict->size; i < l; ++i) {
    if (dict->data[i]) {
      printf("%d: %s %p\n", i, dict->data[i]->key, dict->data[i]->value);
      ++qtd;
      if (dict->data[i]->next)
        qtd += dict_item_list_print(dict->data[i]->next);
    }
  }
  printf("Number of entries: %d\n", qtd);
}

comp_dict_t *dict_new(void)
{
  comp_dict_t *dict = malloc(sizeof(comp_dict_t));
  if (!dict) {
    ERRO("Cannot alocate memory for dict");
    return NULL;
  }
  dict->size = DICT_SIZE;
  dict->occupation = 0;
  dict->data = malloc(sizeof(comp_dict_item_t *) * dict->size);
  if (!dict->data) {
    free(dict);
    ERRO("Cannot alocate memory for dict data");
    return NULL;
  }

  int i, l = dict->size;
  for (i = 0; i < l; ++i)
    dict->data[i] = NULL;

  return dict;
}

void dict_free(comp_dict_t * dict)
{
  if (dict->occupation > 0) {
    ERRO("Cannot free non empty dict");
  }

  int i, l;
  for (i = 0, l = dict->size; i < l; ++i) {
    if (dict->data[i]) {
      dict_item_free(dict->data[i]);
    }
  }

  free(dict->data);
  free(dict);
}

void *dict_put(comp_dict_t * dict, char *key, void *value)
{
  if (dict == NULL || dict->data == NULL || key == NULL) {
    ERRO("At least one parameter is NULL");
  }


  int hash = generate_hash(key, dict->size);

  comp_dict_item_t *newitem = dict_item_new();
  newitem->key = strdup(key);
  newitem->value = value;

  if (dict->data[hash] == NULL) {       // caso 1: entrada não existe na tabela, é inserido imediatamente
    dict->data[hash] = newitem;
    ++dict->occupation;
  } else {                      // caso 2: entrada existe na tabela, inserir no encadeamento
    comp_dict_item_t *exists = dict_item_get(dict->data[hash], key);
    if (!exists) {
      dict_item_insert(dict->data[hash], newitem);
    } else {
      exists->value = value;
      dict_item_free_item(newitem);
      return value;
      /*dict_item_free_item(newitem);
      return exists->value;*/
    }
  }
  return newitem->value;
}

void *dict_get(comp_dict_t * dict, char *key)
{
  if (dict == NULL || dict->data == NULL || key == NULL) {
    ERRO("At least one parameter is NULL");
  }


  int hash = generate_hash(key, dict->size);
  comp_dict_item_t *item = NULL;

  if (dict->data[hash])
    item = dict_item_get(dict->data[hash], key);

  return item->value;
}

void *dict_remove(comp_dict_t * dict, char *key)
{
  if (dict == NULL || dict->data == NULL || key == NULL) {
    ERRO("At least one parameter is NULL");
  }


  int hash = generate_hash(key, dict->size);
  comp_dict_item_t *item = NULL;
  void *data = NULL;

  if (dict->data[hash]) {
    if (strcmp(dict->data[hash]->key, key) == 0) {      // chave é primeiro elemento
      item = dict->data[hash];
      data = item->value;
      dict->data[hash] = item->next;
      dict_item_free_item(item);

      if (dict->data[hash] == NULL)     // chave foi ultimo elemento da entrada
        --dict->occupation;
    } else {                    // chave não é primeiro elemento ou não existe
      data = dict_item_remove(dict->data[hash], key);
    }
  }

  return data;
}
