#include <assert.h>
#include <stdio.h>

#include "../parser.h"

// gcc test_4.c ../parser.c ../lexer.c -o test_4.out


int assert_SimplePred(struct ParseTree *subtree, enum TokenType terminal);
int assert_SimpleExpr(struct ParseTree *subtree, enum TokenType terminal);


int main() {
    struct ParseTree *tree, *walk, *lev1, *line1, *line2;
    struct ParseTree *ifcond, *ifbody, *expr1;
    int status;

    char const* const fileName = "./test_code_4";

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);

    assert(status == SUBTREE_OK);

    walk = tree;
    assert(walk->data->type == Program);
    assert(walk->sibling == NULL);

    walk = walk->child;

    // Check there are 2 Lines
    lev1 = walk;
    assert(lev1->data->type == Line);
    line1 = lev1->child;
    lev1 = lev1->sibling;
    assert(lev1->data->type == Endline);
    assert(lev1->child == NULL);
    lev1 = lev1->sibling;
    assert(lev1->data->type == Line);
    line2 = lev1->child;
    lev1 = lev1->sibling;
    assert(lev1->data->type == Endline);
    assert(lev1->child == NULL);
    assert(lev1->sibling == NULL);

    // Check 1st line
    assert(line1->data->type == Assign);
    line1 = line1->child;
    assert(line1->data->type == Var);
    assert(line1->child == NULL);
    line1 = line1->sibling;
    assert(line1->data->type == Equal);
    assert(line1->child == NULL);
    line1 = line1->sibling;
    assert(line1->data->type == Expr);
    assert(line1->sibling == NULL);
    line1 = line1->child;
    assert(line1->data->type == Pred);
    assert(line1->sibling == NULL);
    line1 = line1->child;
    assert(line1->data->type == Term);
    assert(line1->sibling == NULL);
    line1 = line1->child;
    assert(line1->data->type == BaseExpr);
    assert(line1->sibling == NULL);
    line1 = line1->child;
    assert(line1->data->type == Obj);
    assert(line1->sibling == NULL);
    line1 = line1->child;
    assert(line1->data->type == Str);
    assert(line1->sibling == NULL);
    line1 = line1->child;
    // Check there are 2 quotedstr
    assert(line1->data->type == QuotedStr);
    assert(line1->child != NULL);
    line1 = line1->sibling;
    assert(line1->data->type == Plus);
    assert(line1->child == NULL);
    line1 = line1->sibling;
    assert(line1->data->type == QuotedStr);
    assert(line1->child != NULL);
    assert(line1->sibling == NULL);

    // Check 2nd line
    assert(line2->data->type == IfLine);
    line2 = line2->child;
    assert(line2->data->type == If);
    assert(line2->child == NULL);
    line2 = line2->sibling;
    assert(line2->data->type == IfCond);
    ifcond = line2->child;
    line2 = line2->sibling;
    assert(line2->data->type == IfBody);
    ifbody = line2->child;
    assert(line2->sibling == NULL);

    // Check ifcondition
    assert(ifcond->data->type == Lpar);
    assert(ifcond->child == NULL);
    expr1 = ifcond->sibling;
    assert(expr1->data->type == Expr);
    ifcond = expr1->sibling;
    assert(ifcond->data->type == Rpar);
    assert(ifcond->child == NULL);
    assert(ifcond->sibling == NULL);

    expr1 = expr1->child;
    assert_SimplePred(expr1, Var);
    expr1 = expr1->sibling;
    assert(expr1->data->type = EqEq);
    expr1 = expr1->sibling;
    assert_SimpleExpr(expr1, Num);

    // Check ifbody
    assert(ifbody->data->type == Line);
    assert(ifbody->child->data->type == Input);
    assert(ifbody->child->sibling == NULL);
    ifbody = ifbody->sibling;
    assert(ifbody->data->type == Endline);
    assert(ifbody->child == NULL);
    ifbody = ifbody->sibling;
    assert(ifbody->data->type == Line);
    assert(ifbody->child->data->type == Assign);
    assert(ifbody->child->sibling == NULL);
    ifbody = ifbody->sibling;
    assert(ifbody->data->type == Endline);
    assert(ifbody->child == NULL);
    assert(ifbody->sibling == NULL);

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
