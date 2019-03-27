#include <assert.h>
#include <stdio.h>

#include "../parser.h"

// gcc test_5.c ../parser.c ../lexer.c -o test_5.out


int main() {
    struct ParseTree *tree, *walk, *ifcond, *basexpr;
    int status;

    char const* const fileName = "./test_code_5";

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);

    assert(status == SUBTREE_OK);

    print_ParseTree(tree);

    walk = tree;
    assert(walk->data->type == Program);
    assert(walk->sibling == NULL);

    walk = walk->child;

    assert(walk->data->type == Line);
    assert(walk->sibling->data->type == Endline);
    assert(walk->sibling->sibling == NULL);
    assert(walk->sibling->child == NULL);
    walk = walk->child;

    assert(walk->data->type == LoopLine);
    assert(walk->sibling == NULL);
    walk = walk->child;

    assert(walk->data->type == While);
    assert(walk->child == NULL);
    walk = walk->sibling;

    assert(walk->data->type == IfCond);
    assert(walk->child != NULL);
    ifcond = walk; // to check later
    walk = walk->sibling;

    assert(walk->data->type == Program);
    assert(walk->sibling == NULL);
    walk = walk->child;

    // Check the loop condition
    ifcond = ifcond->child;
    assert(ifcond->data->type == Lpar);
    assert(ifcond->child == NULL);
    ifcond = ifcond->sibling;
    assert(ifcond->data->type == Expr);
    assert(ifcond->child->data->type == Pred);
    assert(ifcond->child->child->data->type == Term);
    assert(ifcond->child->child->child->data->type == BaseExpr);
    basexpr = ifcond->child->child->child;
    assert(basexpr->child->data->type == Obj);
    assert(basexpr->child->child->data->type == Var);
    ifcond = ifcond->child->sibling;
    assert(ifcond->data->type == Lesser);
    ifcond = ifcond->sibling;
    assert(ifcond->data->type == Expr);
    assert(ifcond->child->data->type == Pred);
    assert(ifcond->child->child->data->type == Term);
    assert(ifcond->child->child->child->data->type == BaseExpr);
    basexpr = ifcond->child->child->child;
    assert(basexpr->child->data->type == Obj);
    assert(basexpr->child->child->data->type == Num);

    // Check that there are 3 Lines and 3 Endlines
    assert(walk->data->type == Line);
    assert(walk->child->data->type == Assign);
    assert(walk->child->sibling == NULL);

    walk = walk->sibling;
    assert(walk->data->type == Endline);
    assert(walk->child == NULL);

    walk = walk->sibling;
    assert(walk->data->type == Line);
    assert(walk->child->data->type == Output);
    assert(walk->child->sibling == NULL);

    walk = walk->sibling;
    assert(walk->data->type == Endline);
    assert(walk->child == NULL);

    walk = walk->sibling;
    assert(walk->data->type == Line);
    assert(walk->child->data->type == Break);
    assert(walk->child->child == NULL);
    assert(walk->child->sibling == NULL);

    walk = walk->sibling;
    assert(walk->data->type == Endline);
    assert(walk->child == NULL);
    assert(walk->sibling == NULL);

    printf("---------------\n");
    printf("--- TEST OK ---\n");
    printf("---------------\n");

    free_ParseTree(tree);
    return status;
}
