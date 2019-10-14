#include <assert.h>
#include <stdio.h>

#include "../parser.h"

// gcc test_11.c ../parser.c ../lexer.c -o test_11.out


int assert_SimplePred(struct ParseTree *subtree, enum TokenType terminal);
int assert_SimpleExpr(struct ParseTree *subtree, enum TokenType terminal);


int main() {
    struct ParseTree *tree, *walk, *tmp;
    int status;

    char const* const fileName = "./test_code_11";

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
    walk = walk->child;
    assert(walk->data->type == Assign);

    walk = walk->child;
    assert(walk->data->type == Var);
    walk = walk->sibling;
    assert(walk->data->type == Equal);
    walk = walk->sibling;
    assert(walk->data->type == Expr);
    assert(walk->sibling == NULL);

    walk = walk->child;
    tmp = walk;
    assert(walk->data->type == Pred);
    assert_SimplePred(walk, Var);
    walk = walk->child;
    assert(walk->sibling->data->type == Plus);
    walk = walk->sibling->sibling;
    assert_SimplePred(walk, Var);

    walk = tmp->sibling;
    assert(walk->data->type == EqEq);
    assert(walk->child == NULL);
    walk = walk->sibling;
    assert_SimpleExpr(walk, Num);
    

    printf("---------------\n");
    printf("--- TEST OK ---\n");
    printf("---------------\n");

    free_ParseTree(tree);
    return status;
}


int assert_SimplePred(struct ParseTree *subtree, enum TokenType terminal) {
    assert(subtree->data->type == Pred);
    assert(subtree->child->data->type == Term);
    assert(subtree->child->child->data->type == BaseExpr);
    assert(subtree->child->child->child->data->type == Obj);
    assert(subtree->child->child->child->child->data->type == terminal);
    if (terminal != Num){
        assert(subtree->child->child->child->child->child == NULL);
        assert(subtree->child->child->child->child->sibling == NULL);
    }
    return 0;
}


int assert_SimpleExpr(struct ParseTree *subtree, enum TokenType terminal) {
    assert(subtree->data->type == Expr);
    assert(subtree->child->data->type == Pred);
    assert(subtree->child->child->data->type == Term);
    assert(subtree->child->child->child->data->type == BaseExpr);
    assert(subtree->child->child->child->child->data->type == Obj);
    assert(subtree->child->child->child->child->child->data->type == terminal);
    if (terminal != Num){
        assert(subtree->child->child->child->child->child->child == NULL);
        assert(subtree->child->child->child->child->child->sibling == NULL);
    }
    return 0;
}
