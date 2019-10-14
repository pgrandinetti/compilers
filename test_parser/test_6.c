#include <assert.h>
#include <stdio.h>

#include "../parser.h"

// gcc test_6.c ../parser.c ../lexer.c -o test_6.out


int main() {
    struct ParseTree *tree, *walk;
    int status;

    char const* const fileName = "./test_code_6";

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);

    assert(status == SUBTREE_OK);

    print_ParseTree(tree);

    walk = tree;
    assert(walk->data->type == Program);
    assert(walk->sibling == NULL);

    // Quick walk through the tree and check main stuff

    // First main branch
    walk = walk->child;
    assert(walk->data->type ==  Line);
    walk = walk->child;
    assert(walk->data->type == LoopLine);
    walk = walk->child->sibling->sibling;
    assert(walk->data->type == Program);
    walk = walk->child;
    assert(walk->child->data->type == Assign);
    walk = walk->sibling->sibling->child;
    assert(walk->data->type == IfLine);
    walk = walk->child->sibling->sibling;
    assert(walk->data->type == IfBody);
    walk = walk->child->sibling->sibling;
    assert(walk->data->type == OptElse);

    // Second main branch
    walk = tree->child->sibling->sibling;
    assert(walk->child->data->type == IfLine);
    walk = walk->sibling;
    assert(walk->data->type == Endline);
    assert(walk->sibling == NULL);

    printf("---------------\n");
    printf("--- TEST OK ---\n");
    printf("---------------\n");

    free_ParseTree(tree);
    return status;
}
