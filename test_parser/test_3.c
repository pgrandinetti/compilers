#include <assert.h>
#include <stdio.h>

#include "../parser.h"

// gcc test_3.c ../parser.c ../lexer.c -o test_3.out


int main() {
    struct ParseTree *tree, *walk;
    int status;

    char const* const fileName = "./test_code_3";

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);

    assert(status == PARSING_ERROR);

    printf("---------------\n");
    printf("--- TEST OK ---\n");
    printf("---------------\n");

    free_ParseTree(tree);
    return status;
}
