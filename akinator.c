#include "TREE.h"
#include "STK_forTREE.h"
//dot -Tpng TREE_dump.dot -o TREE_dump.png  
//gcc -g -fsanitize=address,undefined akinator.c stk_forTREE.c -o prog
//git add .
//git commit -m "changes"
//git push

int main()
{
    TREE tr = {0};
    TREE_init(&tr);
    //printf("\n\n%s::%d\n\n", __FILE__, __LINE__);
    TREE_read(&tr, Ftxt_TREE_preorder, &TREE_preorder_parse);
    //printf("\n\n%s::%d\n\n", __FILE__, __LINE__);
    AKINATOR_menu(&tr);
    //printf("\n\n%s::%d\n\n", __FILE__, __LINE__);
    TREE_dump(&tr, Fdot_TREE_dump);
    TREE_save_to_file(&tr, Ftxt_TREE_preorder, &TREE_preorder_save);
    TREE_destroy(&tr);
}

void TREE_init(TREE *tree)
{
    assert(tree != NULL);

    tree->root = (NODE *)calloc(1, sizeof(NODE));
    assert(tree->root != NULL);

    tree->size = 1;

    tree->root[0].elem = (TREE_elem)calloc(256, sizeof(char));
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
            
            free(curr_node->elem);
            free(curr_node);
            curr_node = prev_node;
        }
    }

    free(tree->root->elem);
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
    StackInit(&stk, 16);
    StackPush(&stk, tree->root);

    while(stk.size > 0)
    {
        NODE *curr = StackPop(&stk);

        fprintf(file, "    node_%p [label=\"ptr: %p\\nelem: %s\\nleft: %p\\nright: %p\\nprev: %p\"];\n", 
                (void*)curr, (void*)curr, curr->elem, (void*)curr->left, (void*)curr->right, (void*)curr->prev);

        if(curr->left != NULL)
            fprintf(file, "    node_%p -> node_%p;\n", (void*)curr, (void*)curr->left);
        if(curr->right != NULL)
            fprintf(file, "    node_%p -> node_%p;\n", (void*)curr, (void*)curr->right);

        if(curr->right != NULL)
            StackPush(&stk, curr->right);

        if(curr->left != NULL)
            StackPush(&stk, curr->left);
    }
    fprintf(file, "}\n");

    StackDestroy(&stk);
    fclose(file);

    int ret = system("dot -Tpng " Fdot_TREE_dump " -o " Fpng_TREE_dump);
    if(ret != 0)
        fprintf(stderr, "Ошибка при выполнении команды:"
            " dot -Tpng " Fdot_TREE_dump " -o " Fpng_TREE_dump "(код %d)\n", ret);
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

    int offset = 0;

    if(sscanf(++curr_symbol, "%255[^()]%n", tree->root->elem, &offset) != 1) exit(EXIT_FAILURE);
    curr_symbol += offset;
    StackPush(&stk, tree->root);

    while(*curr_symbol != '\0')
        switch (*curr_symbol)
        {
            case '(':
            {
                NODE *new_node = NEW_NODE;
                new_node->elem = (TREE_elem)calloc(256, sizeof(char));

                new_node->prev = StackPop(&stk);
                StackPush(&stk, new_node->prev);
                StackPush(&stk, new_node);
               
                if(sscanf(++curr_symbol, "%255[^()]%n", new_node->elem, &offset) != 1) exit(EXIT_FAILURE);
                
                new_node->left = NULL;
                new_node->right = NULL;
                
                if(new_node->prev->left == NULL)
                    new_node->prev->left = new_node;
                else
                    new_node->prev->right = new_node;

                ++tree->size;
                curr_symbol += offset;
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

int AKINATOR_menu(TREE *tree)
{
    NODE *curr_node = tree->root;
    while(curr_node != NULL)
    {
        char user_input[32] = {0};

        if(curr_node->right != NULL || curr_node->left != NULL)
            fprintf(Fuser_output, "%s?\n", curr_node->elem);
        else
                fprintf(Fuser_output, "ВЫ ЗАГАДАЛИ %s?\n", curr_node->elem);

        fscanf(Fuser_input, "%s", user_input);

        for(int i = 0; user_input[i] != '\0'; ++i)
            user_input[i] = tolower(user_input[i]);

        if(!strcmp("да", user_input))
        {
            if(curr_node->right != NULL)
                curr_node = curr_node->right;
            else
                return 0; 
        }
        else if(!strcmp("нет", user_input))
        {
            if(curr_node->left != NULL)
                curr_node = curr_node->left;
            else
            {
                fprintf(Fuser_output, "\nВИДИМО Я НЕ ЗНАЮ ТАКОГО\n");
                TREE_create_user_node(tree, curr_node);
                fprintf(Fuser_output, "\nДАВАЙТЕ ЗАНОВО\n");
                curr_node = tree->root;
            } 
        }
        else
            fprintf(Fuser_output, "ОТВЕТЬТЕ ДА/НЕТ\n");
    }

    return 0;
}

void TREE_create_user_node(TREE *tree, NODE *curr_node)
{
    assert(tree != NULL);
    assert(curr_node != NULL);

    char *user_input_answer = (TREE_elem)calloc(256, sizeof(char));
    char *user_input_qwestion = (TREE_elem)calloc(256, sizeof(char));

    fprintf(Fuser_output, "КОГО ВЫ ЗАГАДАЛИ?\n");
    fscanf(Fuser_input, "%s", user_input_answer);
    fprintf(Fuser_output, "ЧЕМ %s ОТЛИЧАЕТСЯ ОТ %s?\n", user_input_answer, curr_node->elem);
    while(getchar() != '\n');
    fscanf(Fuser_input, "%255[^\n]", user_input_qwestion);

    NODE *qwestion_node = NEW_NODE;
    NODE *answer_node = NEW_NODE;

    qwestion_node->prev = curr_node->prev;
    curr_node->prev = qwestion_node;

    qwestion_node->elem = user_input_qwestion;
    answer_node->prev = qwestion_node;
    answer_node->elem = user_input_answer;

    qwestion_node->right = answer_node;
    qwestion_node->left = curr_node;
    answer_node->right = NULL;
    answer_node->left = NULL;
    curr_node->right = NULL;
    curr_node->left = NULL;

    if(qwestion_node->prev->right == curr_node)
        qwestion_node->prev->right = qwestion_node;
    else
        qwestion_node->prev->left = qwestion_node;

    tree->size += 2;
}

void TREE_save_to_file(TREE *tree, const char *filename, void (*TREE_save)(TREE *tree, struct stack_t *stk))
{
    assert(tree != NULL);
    assert(filename != NULL);
    assert(tree->root != NULL);

    FILE *file = fopen(filename, "w");
    assert(file != NULL);

    struct stack_t stk = {0};
    StackInit(&stk, 16);

    TREE_save(tree, &stk);

    while (stk.size > 0)
    {
        NODE *curr_node = StackPop(&stk);

        if (curr_node == NULL)
        {
            fprintf(file, ")");
            continue;
        }

        fprintf(file, "(%s", curr_node->elem);
    }

    StackDestroy(&stk);
    fclose(file);
}

void TREE_preorder_save(TREE *tree, struct stack_t *stk_preorder)
{
    assert(tree != NULL);
    assert(stk_preorder != NULL);
    assert(tree->root != NULL);

    struct stack_t work_stk = {0};
    StackInit(&work_stk, 16);
    StackPush(&work_stk, tree->root);

    struct stack_t rev_stk = {0};
    StackInit(&rev_stk, 16);

    while (work_stk.size > 0)
    {
        NODE *curr_node = StackPop(&work_stk);
        StackPush(&rev_stk, curr_node);

        if (curr_node != NULL)
        {
            StackPush(&work_stk, NULL);

            if (curr_node->right != NULL)
                StackPush(&work_stk, curr_node->right);

            if (curr_node->left != NULL)
                StackPush(&work_stk, curr_node->left);
        }
    }

    while (rev_stk.size > 0)
        StackPush(stk_preorder, StackPop(&rev_stk));

    StackDestroy(&work_stk);
    StackDestroy(&rev_stk);
}
