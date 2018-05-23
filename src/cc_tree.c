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

#include "cc_tree.h"
#include "cc_misc.h"
#include "cc_gv.h"
#include "cc_ast.h"

extern FILE *intfp;
void *comp_tree_last = NULL;

#define ERRO(MENSAGEM) { fprintf (stderr, "[cc_tree, %s] %s.\n", __FUNCTION__, MENSAGEM); abort(); }

comp_tree_t* tree_new(void){
	comp_tree_t *tree = tree_make_node(NULL);
	return tree;
}

void tree_free(comp_tree_t *tree){
	comp_tree_t *ptr = tree;
	do {
		if (ptr->first != NULL)
			tree_free(ptr->first);
		if (ptr->list_next != NULL)
            tree_free(ptr->list_next);
		ptr = ptr->next;
		free(tree);
		tree = ptr;
	} while(ptr != NULL);
}

void list_free(comp_tree_t *list) {
	comp_tree_t *ptr = list;
	if (list != NULL) {
		list_free(ptr->list_next);
        free(list);
    }
}

int count_list_items(comp_tree_t *list) {
	if (list != NULL) {
		return 1 + count_list_items(list->list_next);
	} else {
		return 0;
	}
}

comp_tree_t* tree_make_node(void *value) {
	comp_tree_t *node = malloc(sizeof(comp_tree_t));
	if (!node)
		ERRO("Failed to allocate memory for tree node");

	node->value = value;
	node->childnodes = 0;
	node->first = NULL;
	node->last = NULL;
	node->next = NULL;
	node->prev = NULL;
	node->list_next = NULL;

	// ----- gv -----
	if (value != NULL) {
		AstNodeInfo *info = (AstNodeInfo*)value;
		char *name = NULL;

		if (info->tokenInfo != NULL) {
			switch (info->type) {
				case AST_LITERAL:
				case AST_IDENTIFICADOR:
				case AST_FUNCAO:
					name = (info->tokenInfo)->lexeme;
					break;
				default: break;
			}
		}

		// Only declare to gv if node is an AST node (type >= 0)
		if (info->type >= 0)
			gv_declare(info->type, node, name);
	}
	// ----- -- -----

	return node;
}

void tree_set_list_next_node(comp_tree_t *tree, comp_tree_t *node) {
	if (tree == NULL)
		ERRO("Cannot insert node, tree is null");
	if (node == NULL)
		ERRO("Cannot insert node, node is null");

	tree->list_next = node;

	// ----- gv -----
	// Only connect to gv if 'tree' is an AST node (type >= 0)
	AstNodeInfo *nodeInfo = (tree->value);
	if (nodeInfo->type >= 0)
		gv_connect(tree, node);
	// ----- -- -----

	fprintf (intfp, "node_%p [label=\"\"]\n", tree);
	fprintf (intfp, "node_%p [label=\"\"]\n", node);
	fprintf (intfp, "node_%p -> node_%p\n", tree, node);
	comp_tree_last = tree;
}

void tree_insert_node(comp_tree_t *tree, comp_tree_t *node){
	if (tree == NULL)
		ERRO("Cannot insert node, tree is null");
	if (node == NULL)
		ERRO("Cannot insert node, node is null");

	if (tree_has_child_nodes(tree)){
		tree->first = node;
		tree->last = node;
	} else {
		node->prev = tree->last;
		tree->last->next = node;
		tree->last = node;
	}
	++tree->childnodes;

	// ----- gv -----
	// Only connect to gv if 'tree' is an AST node (type >= 0)
	AstNodeInfo *nodeInfo = (tree->value);
	if (nodeInfo->type >= 0)
		gv_connect(tree, node);
	// ----- -- -----

	fprintf (intfp, "node_%p [label=\"\"]\n", tree);
	fprintf (intfp, "node_%p [label=\"\"]\n", node);
	fprintf (intfp, "node_%p -> node_%p\n", tree, node);
	comp_tree_last = tree;
}

int tree_has_child_nodes(comp_tree_t *tree){
	if (tree != NULL){
		if (tree->childnodes == 0)
			return 1;
	}
	return 0;
}

comp_tree_t* tree_make_unary_node(void *value, comp_tree_t *node){
	comp_tree_t *newnode = tree_make_node(value);
	tree_insert_node(newnode,node);
	return newnode;
}

comp_tree_t* tree_make_binary_node(void *value, comp_tree_t *node1, comp_tree_t *node2){
	comp_tree_t *newnode = tree_make_node(value);
	tree_insert_node(newnode,node1);
	tree_insert_node(newnode,node2);
	return newnode;
}

comp_tree_t* tree_make_ternary_node(void *value, comp_tree_t *node1, comp_tree_t *node2, comp_tree_t *node3){
	comp_tree_t *newnode = tree_make_node(value);
	tree_insert_node(newnode,node1);
	tree_insert_node(newnode,node2);
	tree_insert_node(newnode,node3);
	return newnode;
}

static void print_spaces(int num){
	while (num-->0)
		putc(' ',stdout);
}

static void tree_debug_print_node(comp_tree_t *tree, int spacing){
	if (tree == NULL) return;
	print_spaces(spacing);
	printf("%p(%d): %p\n",tree,tree->childnodes,tree->value);
}

static void tree_debug_print_s(comp_tree_t *tree, int spacing){
	if (tree == NULL) return;

	comp_tree_t *ptr = tree;
	do {
		tree_debug_print_node(ptr,spacing);
		if (ptr->first != NULL)
			tree_debug_print_s(ptr->first,spacing+1);
		ptr = ptr->next;
	} while(ptr != NULL);
}

void tree_debug_print(comp_tree_t *tree){
	tree_debug_print_s(tree,0);
}