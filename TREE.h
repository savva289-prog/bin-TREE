#ifndef TREEH

#define TREEH

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <assert.h>

#define NEW_NODE (NODE *)calloc(1, sizeof(NODE))
#define Fdot_TREE_dump "TREE_dump.dot"
#define Ftxt_TREE_preorder "TREE_pr.txt"

struct stack_t;

typedef int TREE_elem;

typedef struct NODE
{
    TREE_elem elem;
    struct NODE *left;
    struct NODE *right;
    struct NODE *prev;
} NODE;

typedef struct TREE
{
    size_t size;
    NODE *root;
} TREE;

void TREE_init           (TREE *tree);
void TREE_add            (TREE *tree, TREE_elem elem);
NODE *TREE_find_prev_node(TREE *tree, TREE_elem elem);
void TREE_dump           (TREE *tree, const char *filename);
void TREE_read           (TREE *tree, const char *filename, void (*TREE_parse)(TREE *tree, char *curr_symbol));
void TREE_preorder_parse (TREE *tree, char *curr_symbol);
void TREE_destroy        (TREE *tree);
size_t wrapper_stat      (const char *filename);

#endif