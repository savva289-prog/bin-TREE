#include "TREE.h"
#include "STK_forTREE.h"
//dot -Tpng TREE_dump.dot -o TREE_dump.png

int main()
{
    TREE tr = {0};
    TREE_init(&tr);

    TREE_read(&tr, Ftxt_TREE_preorder, &TREE_preorder_parse);
    //TREE_add(&tr, 20);
    //TREE_add(&tr, 30);
    //TREE_add(&tr, 40);
    //TREE_add(&tr, 45);
    //TREE_add(&tr, 55);
    //TREE_add(&tr, 35);

    TREE_dump(&tr, Fdot_TREE_dump);
    TREE_destroy(&tr);
}

void TREE_init(TREE *tree)
{
    assert(tree != NULL);

    tree->root = (NODE *)calloc(1, sizeof(NODE));
    assert(tree->root != NULL);

    tree->size = 1;

    tree->root[0].elem = 0;
    tree->root[0].prev = NULL;
    tree->root[0].left = NULL;
    tree->root[0].right = NULL;
}

void TREE_add(TREE *tree, TREE_elem elem)
{
    assert(tree != NULL);

    NODE *temp = NEW_NODE;
    NODE *prev_node = TREE_find_prev_node(tree, elem);

    if(elem < prev_node->elem)
        prev_node->left = temp;
    else
        prev_node->right = temp;

    temp->elem = elem;
    temp->left = NULL;
    temp->right = NULL;
    temp->prev = prev_node;

    ++tree->size;
}

NODE *TREE_find_prev_node(TREE *tree, TREE_elem elem)
{
    assert(tree != NULL);

    NODE *temp = &tree->root[0];
    while(true)
    {
        if(elem >= temp->elem)
        {
            if(temp->right == NULL)
                return temp;

            temp = temp->right;
        }
        else
        {
            if(temp->left == NULL)
                return temp;

            temp = temp->left;
        }
    }
}

void TREE_destroy(TREE *tree)
{
    assert(tree != NULL);
    assert(tree->root != NULL);

    NODE *curr_node = tree->root;
    while(curr_node != NULL)
    {
        if(curr_node->left != NULL)
            curr_node = curr_node->left;
        else if(curr_node->right != NULL)
            curr_node = curr_node->right;
        else
        {
            NODE *prev_node = curr_node->prev;

            if(curr_node == tree->root)
                break;
            else if(prev_node->left == curr_node)
                prev_node->left = NULL;
            else
                prev_node->right = NULL;
            
            free(curr_node);
            curr_node = prev_node;
        }
    }

    free(tree->root);
    tree->root = NULL;
    tree->size = 0;
}

void TREE_dump(TREE *tree, const char *filename)
{
    assert(tree != NULL);
    assert(tree->root != NULL);

    FILE *file = fopen(filename, "w");
    assert(file != NULL);

    fprintf(file, "digraph BST {\n");
    fprintf(file, "    rankdir=TB;\n");
    fprintf(file, "    node [shape=box, style=\"filled, rounded\", fillcolor=\"#e1d5e7\", fontname=\"Courier\", fontsize=10];\n");
    fprintf(file, "    edge [color=\"#6c8ebf\", penwidth=1.5];\n\n");

    struct stack_t stk = {0};
    StackInit(&stk, tree->size);
    size_t top = 0;

    StackPush(&stk, tree->root);
    ++top;

    while(top > 0)
    {
        NODE *curr = StackPop(&stk);
        --top;

        fprintf(file, "    node_%p [label=\"ptr: %p\\nelem: %d\\nleft: %p\\nright: %p\\nprev: %p\"];\n", 
                (void*)curr, (void*)curr, curr->elem, (void*)curr->left, (void*)curr->right, (void*)curr->prev);

        if(curr->left != NULL)
            fprintf(file, "    node_%p -> node_%p;\n", (void*)curr, (void*)curr->left);
        if(curr->right != NULL)
            fprintf(file, "    node_%p -> node_%p;\n", (void*)curr, (void*)curr->right);

        if(curr->right != NULL)
        {
            StackPush(&stk, curr->right);
            ++top;
        }

        if(curr->left != NULL)
        {
            StackPush(&stk, curr->left);
            ++top;
        }
    }

    StackDestroy(&stk);
    fprintf(file, "}\n");
    fclose(file);
}

void TREE_read(TREE *tree, const char *filename, void (*TREE_parse)(TREE *tree, char *curr_symbol))
{
    assert(tree != NULL);
    assert(tree->root != NULL);
    assert(filename != NULL);

    FILE *file = fopen(filename, "r");
    size_t size = wrapper_stat(filename);

    char *tree_str = (char *)calloc(size + 1, sizeof(char));
    char *cpy_tree_str = tree_str;
    fread((void *)tree_str, size, size / sizeof(char), file);
    tree_str[size] = '\0';

    TREE_parse(tree, cpy_tree_str);

    free(tree_str);

    fclose(file);
}

void TREE_preorder_parse(TREE *tree, char *curr_symbol)
{
    assert(tree != NULL);
    assert(curr_symbol != NULL);

    struct stack_t stk = {0};
    StackInit(&stk, 16);

    tree->root->elem = (int)strtol(curr_symbol + 1, &curr_symbol, 10);
    StackPush(&stk, tree->root);

    while(*curr_symbol != '\0')
        switch (*curr_symbol)
        {
            case '(':
            {
                NODE *new_node = NEW_NODE;

                new_node->prev = StackPop(&stk);
                StackPush(&stk, new_node->prev);
                StackPush(&stk, new_node);
                        
                new_node->elem = (int)strtol(curr_symbol + 1, &curr_symbol, 10);
                new_node->left = NULL;
                new_node->right = NULL;
                        
                if(new_node->prev->left == NULL)
                    new_node->prev->left = new_node;
                else
                    new_node->prev->right = new_node;

                ++tree->size;
                break;
            }
        
            case ')':
                StackPop(&stk);
                ++curr_symbol;
                break;
            
            case ' ':  ++curr_symbol; break;
            case '\n': ++curr_symbol; break;
            case '\t': ++curr_symbol; break;

            default:
                printf("\nОШИБКА В СОСТАВЛЕНИИ СТРОКИ ДЛЯ ЧТЕНИЯ\n"
                    "НЕИЗВЕСТНЫЙ СИМВОЛ <%c>\n"
                    "%s::%d\n\n", *curr_symbol, __FILE__, __LINE__);
                exit(EXIT_FAILURE);
                break;
        }

    StackDestroy(&stk);
}

size_t wrapper_stat(const char *filename)
{
    assert(filename != NULL);

    struct stat st = {0};
    
    int stat_flag = stat(filename, &st);
    assert(stat_flag != -1);

    return (size_t)(st.st_size);
}