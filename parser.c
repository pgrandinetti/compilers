#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//#include "lexer.h"


enum ConstructType {
    // A syntactical Construct of the Grammar
    Comma,
    Lpar, // (
    Rpar, // )
    Lbrack, // [
    Rbrack, // ]
    Lcurly, // {
    Rcurly, // }
    Plus,
    Minus,
    Equal,
    EqEq, // ==
    NotEq, // !=
    Lesser, // <
    Greater, // >
    LesserEq,
    GreaterEq,
    Star, // *
    Div, // /
    FloatDiv, // /.
    Percent, // %
    Or,
    And, // &
    Int,
    Float,
    Pow,
    Dot,
    QuotedStr, // "
    Bool,
    Null,
    Var,
    Keyword,
    Endline,
/* -----=====----- */
    Program,
    Line,
    Assign,
    Input,
    Output,
    ifLine,
    loopLine,
    keywordLine,
    Expr,
    BaseExpr,
    Operator,
    CondOp,
    IfConf,
    IfBody,
    OptElse,
    LoopBody,
    Obj,
    Str,
    Num
};

struct Construct {
    char* text;
    enum ConstructType type;
};

struct ParseTree {
    struct Construct* data;
    struct ParseTree* child;
    struct ParseTree* sibling;
};

void free_ParseTree(struct ParseTree* tree) {
    struct ParseTree* sibling;
    struct ParseTree* child;
    sibling = tree->sibling;
    child = tree->child;
    free(tree->data->text);
    free(tree->data);
    free(tree);
    if (sibling != NULL)
        free_ParseTree(sibling);
    if (child != NULL)
        free_ParseTree(child);
}

/*
struct ParseTree* build_tree (struct token* tok) {
    // Do not modify head of token list
    struct token* current = tok;
}
*/

struct Construct* new_Construct(char* text, size_t len) {
    struct Construct* c;
    c = malloc(sizeof(struct Construct));
    if (c == NULL)
        return NULL;
    c->text = malloc(len * sizeof(char));
    if (c->text == NULL)
        return NULL;
    memcpy(c->text, text, len);
    return c;
}

struct ParseTree* new_ParseTree(struct Construct* c) {
    struct ParseTree* tree;
    tree = malloc(sizeof(struct ParseTree));
    if (tree == NULL)
        return NULL;
    tree->data = c;
    tree->child = NULL;
    tree->sibling = NULL;
    return tree;
}

int main() {

    struct Construct* c1 = new_Construct((char[4]) {'h', 'i', '!', '\0'}, 4);
    struct ParseTree* p1 = new_ParseTree(c1);

    struct Construct* c2 = new_Construct((char[5]) {'h', 'i', '!', '!', '\0'}, 5);
    struct ParseTree* p2 = new_ParseTree(c2);

    struct Construct* c3 = new_Construct((char[3]) {'h', 'i', '\0'}, 3);
    struct ParseTree* p3 = new_ParseTree(c3);

    p1->child = p2;
    p1->sibling = p3;
    free_ParseTree(p1);
}
