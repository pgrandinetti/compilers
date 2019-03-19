#include <assert.h>
#include <stdio.h>

#include "../parser.h"

// gcc test_4.c ../parser.c ../lexer.c -o test_4.out


int main() {
    struct ParseTree *tree, *walk, *lev1, *line1, *line2;
    struct ParseTree *ifcond, *ifbody, *ifline1, *ifline2;
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
    ifcond = ifcond->sibling;
    assert(ifcond->data->type == Obj);
    assert(ifcond->child->data->type == Var);
    assert(ifcond->child->child == NULL);
    assert(ifcond->child->sibling == NULL);
    ifcond = ifcond->sibling;
    assert(ifcond->data->type == EqEq);
    assert(ifcond->child == NULL);
    ifcond = ifcond->sibling;
    assert(ifcond->data->type == Obj);
    assert(ifcond->sibling->data->type == Rpar);
    assert(ifcond->sibling->child == NULL);
    assert(ifcond->sibling->sibling == NULL);
    ifcond = ifcond->child;
    assert(ifcond->data->type == Num);
    assert(ifcond->sibling == NULL);
    ifcond = ifcond->child;
    assert(ifcond->data->type == Float);
    assert(ifcond->sibling == NULL);
    ifcond = ifcond->child;
    assert(ifcond->data->type == Int);
    assert(ifcond->sibling == NULL);
    assert(ifcond->child == NULL);

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
