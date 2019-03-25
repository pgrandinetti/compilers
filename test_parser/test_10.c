#include <assert.h>
#include <stdio.h>

#include "../parser.h"

// gcc test_10.c ../parser.c ../lexer.c -o test_10.out


int assert_SimpleTerm(struct ParseTree *subtree, enum TokenType terminal);
int assert_SimpleExpr(struct ParseTree *subtree, enum TokenType terminal);
int assert_SimpleBase(struct ParseTree *subtree, enum TokenType terminal);
int assert_ComplExpr(struct ParseTree *subtree);

int main() {
    struct ParseTree *tree, *walk, *tmp;
    int status;

    char const* const fileName = "./test_code_10";

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);

    print_ParseTree(tree);

    assert(status == SUBTREE_OK);

    assert(tree->data->type == Program);
    walk = tree->child;
    assert(walk->data->type == Line);
    assert(walk->sibling->data->type == Endline);
    assert(walk->sibling->child == NULL);

    walk = walk->child;
    assert(walk->data->type == Assign);
    walk = walk->child;
    assert(walk->data->type == Var);
    walk = walk->sibling;
    assert(walk->data->type == Equal);
    walk = walk->sibling;
    assert(walk->data->type = Expr);

    // Now check the Expr
    tmp = walk->child;
    assert_SimpleTerm(tmp, Var);
    tmp = tmp->child->sibling;
    assert(tmp->data->type == FloatDiv);
    tmp = tmp->sibling;
    assert_SimpleTerm(tmp, Var);

    tmp = walk->child->sibling;
    assert(tmp->data->type == Plus);
    assert(tmp->child == NULL);

    tmp = tmp->sibling; // big branch

    assert(tmp->data->type == Expr);
    walk = tmp;

    tmp = tmp->child;
    assert_SimpleTerm(tmp, Var);
    tmp = tmp->child; // base expr
    tmp = tmp->sibling;
    assert(tmp->data->type == Star);
    tmp = tmp->sibling;
    assert(tmp->data->type == Term);
    assert(tmp->sibling == NULL);
    tmp = tmp->child;
    assert_ComplExpr(tmp);
    tmp = tmp->child->sibling; // the expr
    tmp = tmp->child;
    assert_SimpleTerm(tmp, Var);
    tmp = tmp->sibling;
    assert(tmp->data->type == Plus);
    tmp = tmp->sibling;
    assert_SimpleExpr(tmp, Var);

    tmp = walk->child->sibling;
    assert(tmp->data->type == Minus);
    tmp = tmp->sibling;
    assert(tmp->data->type == Expr);
    tmp = tmp->child;
    assert(tmp->data->type == Term);
    tmp = tmp->child;
    assert_SimpleBase(tmp, Var);
    tmp = tmp->sibling;
    assert(tmp->data->type == Star);
    tmp = tmp->sibling;
    assert(tmp->data->type == Term);
    tmp = tmp->child;
    assert_ComplExpr(tmp);
    // ...

    printf("---------------\n");
    printf("--- TEST OK ---\n");
    printf("---------------\n");

    free_ParseTree(tree);
    return status;
}

int assert_SimpleExpr(struct ParseTree *subtree, enum TokenType terminal) {
    assert(subtree->data->type == Expr);
    assert(assert_SimpleTerm(subtree->child, terminal) == 0);
}

int assert_ComplExpr(struct ParseTree *subtree) {
    assert(subtree->data->type == BaseExpr);
    assert(subtree->child->data->type == Lpar);
    assert(subtree->child->child == NULL);
    assert(subtree->child->sibling->data->type == Expr);
    assert(subtree->child->sibling->sibling->data->type == Rpar);
    assert(subtree->child->sibling->sibling->child == NULL);
}

int assert_SimpleTerm(struct ParseTree *subtree, enum TokenType terminal) {
    assert(subtree->data->type == Term);
    assert(subtree->child->data->type == BaseExpr);
    assert(subtree->child->child->data->type == Obj);
    assert(subtree->child->child->child->data->type == terminal);
    return 0;
}

int assert_SimpleBase(struct ParseTree *subtree, enum TokenType terminal) {
    assert(subtree->data->type == BaseExpr);
    assert(subtree->child->data->type == Obj);
    assert(subtree->child->child->data->type == terminal);
    return 0;
}
