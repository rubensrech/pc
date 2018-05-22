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

#ifndef CC_TREE_H_
#define CC_TREE_H_

/*
 * Tipo: comp_tree_t, value é um tipo genérico armazenado em cada
 * nó da árvore, childnodes é a quantidade de nós filhos de um nó,
 * first e last referem-se ao primeiro e ultimo nós filhos,
 * next e prev percorrem uma lista de nós filhos de um mesmo nó.
 * Para uma árvore com 2 filhos, first->next == last e first == last->prev
 */
typedef struct comp_tree {
	void *value;
	int childnodes;
	struct comp_tree *first,*last;
	struct comp_tree *next,*prev;
	struct comp_tree *list_next;
} comp_tree_t;

/* Funções: a seguir segue a lista de funções da API cc_tree */

/*
 * Função: tree_new, cria um nó raiz para a árvore. Retorna um
 * ponteiro para o nó criado ou aborta a execução do programa
 * caso algum erro de alocação de memória tenha ocorrido.
 * Equivalente à tree_make_node(NULL).
 */
comp_tree_t* tree_new(void);

/*
 * Função: tree_free, percorre recursivamente a estrutura da
 * árvore e libera todos comp_tree_t alocados, os elementos value
 * devem ser tratados externamente, previamente ao uso desta função.
 */
void tree_free(comp_tree_t *tree);

/*
 * Função: tree_make_node, cria um novo nó e retorna o ponteiro para
 * a estrutura comp_tree_t criada. O paramêtro value é uma estrutura
 * qualquer sob responsabilidade do usuário.
 */
comp_tree_t* tree_make_node(void *value);

/*
 * Função: tree_insert_node, insere node no encadeamento de filhos de
 * tree. Aborta o programa se tree ou node forem nulos.
 */
void tree_insert_node(comp_tree_t *tree, comp_tree_t *node);

/*
 * Função: tree_has_child_nodes, retorna 1 se existir algum nó no
 * encadeamento de nós filhos de tree, 0 caso contrário.
 */
int tree_has_child_nodes(comp_tree_t *tree);

/*
 * Função: tree_make_unary_node, cria um novo nó usando value, e
 * insere node no encadeamento de filhos do novo nó.
 */
comp_tree_t* tree_make_unary_node(void *value, comp_tree_t* node);

/*
 * Função: tree_make_binary_node, cria um novo nó usando value, e
 * insere node1 e node2 no encadeamento de filhos do novo nó.
 */
comp_tree_t* tree_make_binary_node(void *value, comp_tree_t* node1, comp_tree_t* node2);

/*
 * Função: tree_make_ternary_node, cria um novo nó usando value, e
 * insere node1, node2 e node3 no encadeamento de filhos do novo nó.
 */
comp_tree_t* tree_make_ternary_node(void *value, comp_tree_t* node1, comp_tree_t* node2, comp_tree_t* node3);

/*
 * Função: tree_debug_print, percorre a árvore recursivamente e
 * exibe os ponteiros dos atributos value.
 */
void tree_debug_print(comp_tree_t *tree);



/*
 * Função: tree_set_list_next_node, define o proximo nodeo de uma
 * lista, em um contexto em que tree é um elemento de uma lista e
 * node deve ser o elemento seguinte a tree
 */
void tree_set_list_next_node(comp_tree_t *tree, comp_tree_t *node);

/*
 * Função: list_free, libera todos os nos de uma lista
 * construida a partir da estrutura da arvore, ou seja,
 * utilizando apenas o campo list_next da estrutura. Os
 * elementos value devem ser tratados externamente, 
 * previamente ao uso desta função.
 */
void list_free(comp_tree_t *list);

int count_list_items(comp_tree_t *list);


#endif //CC_TREE_H_
