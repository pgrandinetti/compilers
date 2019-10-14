#include <assert.h>
#include <stdio.h>

#include "../parser.h"


int main() {
    struct ParseTree *tree, *walk;
    int status;

    char const* const fileName = "./test_code_2";

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);

    assert(status == SUBTREE_OK);

    walk = tree;
    assert(walk->data->type == Program);
    assert(walk->sibling == NULL);

    walk = walk->child;
    assert(walk->data->type == Line);

    assert(walk->sibling->data->type == Endline);
    assert(walk->sibling->child == NULL);
    assert(walk->sibling->sibling == NULL);

    walk = walk->child;
    assert(walk->data->type == Output);
    assert(walk->sibling == NULL);

    walk = walk->child;
    assert(walk->data->type == WriteOut);
    assert(walk->child == NULL);

    walk = walk->sibling;
    assert(walk->data->type == Obj);
    assert(walk->sibling == NULL);

    walk = walk->child;
    assert(walk->data->type == Num);
    assert(walk->sibling == NULL);

    walk = walk->child;
    assert(walk->data->type == Float);
    assert(walk->sibling == NULL);

    walk = walk->child;
    assert(walk->data->type == Int);
    assert(walk->sibling == NULL);
    assert(walk->child == NULL);

    printf("---------------\n");
    printf("--- TEST OK ---\n");
    printf("---------------\n");

    free_ParseTree(tree);
    return status;
}
