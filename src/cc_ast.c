#include <stdlib.h>
#include <stdio.h>
#include "cc_ast.h"
#include "cc_gv.h"
#include "cc_sem.h"

comp_tree_t *ast;

void initAST() {
    ast = tree_new();
    gv_init(GV_OUT_FILE);
}

void freeAST() {
    gv_close();
    freeASTSubTree(ast);
    tree_free(ast);
}

void freeASTSubTree(comp_tree_t *tree) {
    comp_tree_t *ptr = tree;
	do {
		if (ptr->first != NULL)
			freeASTSubTree(ptr->first);
        if (ptr->list_next != NULL)
            freeASTSubTree(ptr->list_next);
		ptr = ptr->next;
        free(tree->value);
		tree = ptr;
	} while(ptr != NULL);
}

comp_tree_t *makeASTNode(int type, TokenInfo *token) {
    AstNodeInfo *nodeInfo = malloc(sizeof(struct astNodeInfo));
    nodeInfo->type = type;
    nodeInfo->tokenInfo = token;
    nodeInfo->dataType = DATATYPE_UNDEF;
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

comp_tree_t *makeASTQuaternaryNode(int type, TokenInfo *token, comp_tree_t *node1, comp_tree_t *node2, comp_tree_t *node3, comp_tree_t *node4) {
    comp_tree_t *newnode = makeASTNode(type, token);
    tree_insert_node(newnode, node1);
    tree_insert_node(newnode, node2);
    tree_insert_node(newnode, node3);
    tree_insert_node(newnode, node4);
	return newnode;
}

TokenInfo *getASTNodeTokenInfo(comp_tree_t *node) {
    return ((AstNodeInfo *)node->value)->tokenInfo;
}