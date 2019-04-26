#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "parser.h"

/* Preliminary Definitions

 * Each function `is_<TokenType>` is defined as

 * int is_<TokenType> (struct TokenList** tok, struct ParseTree** new)

 * - **tok points to the current token in the sequence. The function moves it ahead CHANGING the pointer location.
 * - **new points to a ALREADY-ALLOCATED ParseTree. It will contain the resulting subtree, or stays empty.

 * Therefore:
 * - The caller must alloc the ParseTree** new
 * - The called must assign *new->data, child and sibling
 * - The caller is responsible for freeing memory

 * Return a integer flag with the operation final status (see parser.h).
*/

int is_Obj (struct TokenList** tok, struct ParseTree** tree);

int is_List (struct TokenList** tok, struct ParseTree** tree);

int is_ListExpr (struct TokenList** tok, struct ParseTree** tree);

int is_Line(struct TokenList** tok, struct ParseTree** tree);

int is_Program(struct TokenList** head, struct ParseTree** tree);

int is_Expr(struct TokenList** head, struct ParseTree** tree);

int is_BaseExpr(struct TokenList** head, struct ParseTree** tree);

int is_Term(struct TokenList** head, struct ParseTree** tree);

/* */


struct ParseTree* alloc_ParseTree() {
    struct ParseTree* tree;
    tree = malloc(sizeof(struct ParseTree));
    if (tree == NULL)
        return NULL;
    tree->data = NULL;
    tree->child = NULL;
    tree->sibling = NULL;
    return tree;
}


struct ParseTree* new_ParseTree(struct Token* c) {
    struct ParseTree* tree;
    tree = alloc_ParseTree();
    if (tree == NULL)
        return NULL;
    tree->data = new_Token(c->lexeme, c->type);
    if (tree->data == NULL) {
        free(tree);
        return NULL;
    }
    tree->child = NULL;
    tree->sibling = NULL;
    return tree;
}


void print_PT(struct ParseTree* tree, int indent) {
    if (tree == NULL)
        return;
    // This is a Depth-First print
    if (indent > 0) {
        char space[indent + 1];
        memset(space, ' ', indent * sizeof(char));
        memset(space+indent, '\0', sizeof(char));
        printf("%s", space);
    }
    print_Token(tree->data);
    struct ParseTree* sibling;
    struct ParseTree* child;
    sibling = tree->sibling;
    child = tree->child;
    print_PT(child, indent + 2);
    print_PT(sibling, indent);
}


void print_ParseTree(struct ParseTree* tree) {
    print_PT(tree, 0);
}


void free_ParseTree(struct ParseTree* tree) {
    struct ParseTree* sibling;
    struct ParseTree* child;
    sibling = tree->sibling;
    child = tree->child;
    free_Token(tree->data);
    free(tree);
    if (sibling != NULL)
        free_ParseTree(sibling);
    if (child != NULL)
        free_ParseTree(child);
}

/*
 * Template function for easy-to-check Tokens
 *
 * Match the current Token in the TokenList** tok, with the given TokenType type.
 * If char* lexeme != NULL, then also checks that the current Token's lexeme matches it.
 *
 * First, all WS are skipped. Then, if the check fails *tok unchanged. Otherwise it's advanced.
 *
 * It ASSUMES WS are already removed
*/
int _single_Token_template(struct TokenList** tok, struct ParseTree** new, enum TokenType type, char* lexeme) {
    if (*tok == NULL)
        return PARSING_ERROR;
    struct TokenList* current = *tok;
    if (current->token->type != type) {
        printf("Expecting <%s>, Found <%s>\n", type2char(type), type2char(current->token->type));
        return PARSING_ERROR;
    }
    if (lexeme != NULL)
        if(strcmp(current->token->lexeme, lexeme) != 0) {
            printf("Expecting %s, Found %s\n", lexeme, current->token->lexeme);
            return PARSING_ERROR;
        }
    // As by definition above, new is already allocated
    struct Token* newTok = new_Token(current->token->lexeme, current->token->type);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;
    (*new)->child = NULL;
    (*new)->sibling = NULL;
    *tok = current->next;
    return SUBTREE_OK;
}


int is_Endline (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Endline, NULL);
}


int is_Int (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Int, NULL);
}


int is_Dot (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Dot, NULL);
}


int is_Var (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Var, NULL);
}


int is_readIn (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, ReadIn, "readIn");
}


int is_writeOut (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, WriteOut, "writeOut");
}


int is_If (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, If, "if");
}


int is_Else (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Else, "else");
}


int is_While (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, While, "while");
}


int is_Break (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Break, "break");
}


int is_Continue (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Continue, "continue");
}


int is_Lbrack (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Lbrack, NULL);
}


int is_Rbrack (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Rbrack, NULL);
}


int is_Lpar (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Lpar, NULL);
}


int is_Rpar (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Rpar, NULL);
}


int is_Comma (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Comma, NULL);
}


int is_EqEq (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, EqEq, NULL);
}


int is_Equal (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Equal, NULL);
}


int is_Plus (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Plus, NULL);
}


int is_Minus (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Minus, NULL);
}


int is_NotEq (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, NotEq, NULL);
}


int is_Greater (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Greater, NULL);
}


int is_GreaterEq (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, GreaterEq, NULL);
}


int is_Lesser (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Lesser, NULL);
}


int is_LesserEq (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, LesserEq, NULL);
}


int is_Star (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Star, NULL);
}


int is_Div (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Div, NULL);
}


int is_FloatDiv (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, FloatDiv, NULL);
}


int is_Percent (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Percent, NULL);
}


int is_Or (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Or, NULL);
}


int is_And (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, And, NULL);
}


int is_Pow (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Pow, NULL);
}


int is_Null (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Null, NULL);
}


int is_Bool (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Bool, NULL);
}


int is_CharSeq (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, QuotedStr, NULL);
}


int is_QuotedStr (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *charseq, *comma, *obj, *last;
    int status;
    struct Token *newTok;

    status = SUBTREE_OK;

    newTok = new_Token((char[1]){'\0'}, QuotedStr);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    charseq = alloc_ParseTree();
    if (charseq == NULL)
        return MEMORY_ERROR;
    status = is_CharSeq(tok, &charseq);
    if (status != SUBTREE_OK) {
        free_ParseTree(charseq);
        return status;
    }
    (*new)->child = charseq;
    last = charseq;

    while ( (*tok)->token->type == Comma) {
        comma = alloc_ParseTree();
        if (comma == NULL)
            return MEMORY_ERROR;
        status = is_Comma(tok, &comma);
        if (status != SUBTREE_OK) {
            free_ParseTree(comma);
            break;
        }
        last->sibling = comma;
        last = comma;
        // Now there must be a Obj
        obj = alloc_ParseTree();
        if (obj == NULL)
            return MEMORY_ERROR;
        status = is_Obj(tok, &obj);
        if (status != SUBTREE_OK) {
            free_ParseTree(obj);
            break;
        }
        last->sibling = obj;
        last = obj;
    }

    return status;
}


int is_CondOp (struct TokenList** tok, struct ParseTree** new) {
    enum TokenType type;

    type = (*tok)->token->type;
    if (type == And)
        return is_And(tok, new);
    else if (type == Or)
        return is_Or(tok, new);
    else if (type == EqEq)
        return is_EqEq(tok, new);
    else if (type == NotEq)
        return is_NotEq(tok, new);
    else if (type == Greater)
        return is_Greater(tok, new);
    else if (type == GreaterEq)
        return is_GreaterEq(tok, new);
    else if (type == Lesser)
        return is_Lesser(tok, new);
    else if (type == LesserEq)
        return is_LesserEq(tok, new);
    else
        return PARSING_ERROR;
}


int is_Operator (struct TokenList** tok, struct ParseTree** new) {
    enum TokenType type;

    type = (*tok)->token->type;
    if (type == Plus)
        return is_Plus(tok, new);
    else if (type == Minus)
        return is_Minus(tok, new);
    else if (type == Star)
        return is_Star(tok, new);
    else if (type == Div)
        return is_Div(tok, new);
    else if (type == FloatDiv)
        return is_FloatDiv(tok, new);
    else if (type == Percent)
        return is_Percent(tok, new);
    else
        return is_CondOp(tok, new);
}


int match_CondOp_type (enum TokenType type) {
    return (type == EqEq ||
            type == NotEq ||
            type == Greater ||
            type == GreaterEq ||
            type == Lesser ||
            type == LesserEq ||
            type == And ||
            type == Or);
}


int match_AritmOp_type (enum TokenType type) {
    return (type == Plus ||
            type == Minus ||
            type == Star ||
            type == Div ||
            type == FloatDiv ||
            type == Percent);
}


int match_TermOp_type (enum TokenType type) {
    return (type == Star ||
            type == Div ||
            type == FloatDiv ||
            type == Percent);
}


int match_operator_type (enum TokenType type) {
    return (match_AritmOp_type(type) ||
            match_CondOp_type(type));
}


int is_BaseExpr (struct TokenList** tok, struct ParseTree** new) {
    int status;
    struct ParseTree *lpar, *rpar, *subexpr, *obj;
    struct Token *newTok;

    status = SUBTREE_OK;

    newTok = new_Token((char[1]){'\0'}, BaseExpr);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    if ((*tok)->token->type == Lpar) {
        // is a sub expression
        lpar = alloc_ParseTree();
        if (lpar == NULL)
            return MEMORY_ERROR;
        status = is_Lpar(tok, &lpar);
        if (status != SUBTREE_OK){
            free_ParseTree(lpar);
            return status;
        }
        (*new)->child = lpar;

        subexpr = alloc_ParseTree();
        if (subexpr == NULL)
            return MEMORY_ERROR;
        status = is_Expr(tok, &subexpr);
        if (status != SUBTREE_OK){
            free_ParseTree(subexpr);
            return status;
        }
        lpar->sibling = subexpr;

        rpar = alloc_ParseTree();
        if (rpar == NULL)
            return MEMORY_ERROR;
        status = is_Rpar(tok, &rpar);
        if (status != SUBTREE_OK){
            free_ParseTree(rpar);
            return status;
        }
        subexpr->sibling = rpar;
    }
    else {
        // just a Obj
        obj = alloc_ParseTree();
        if (obj == NULL)
            return MEMORY_ERROR;
        status = is_Obj(tok, &obj);
        if (status != SUBTREE_OK) {
            free_ParseTree(obj);
            return status;
        }
        (*new)->child = obj;
    }
    return status;
}

int is_Term (struct TokenList** tok, struct ParseTree** new) {
    int status;
    enum TokenType type;
    struct ParseTree *base, *op, *term;
    struct Token *newTok;

    status = SUBTREE_OK;

    newTok = new_Token((char[1]){'\0'}, Term);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    base = alloc_ParseTree();
    if (base == NULL)
        return MEMORY_ERROR;
    status = is_BaseExpr(tok, &base);
    if (status != SUBTREE_OK){
        free_ParseTree(base);
        return status;
    }
    (*new)->child = base;

    // The remainder of a Term is optional
    type = (*tok)->token->type;
    if (match_TermOp_type(type)){
        op = alloc_ParseTree();
        if (op == NULL)
            return MEMORY_ERROR;
        status = is_Operator(tok, &op);
        if (status != SUBTREE_OK){
            free_ParseTree(op);
            return status;
        }
        base->sibling = op;

        term = alloc_ParseTree();
        if (term == NULL)
            return MEMORY_ERROR;
        status = is_Term(tok, &term);
        if (status != SUBTREE_OK){
            free_ParseTree(term);
            return status;
        }
        op->sibling = term;
    }
    return status;
}

int is_Pred (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *term, *op, *pred;
    struct Token *newTok;
    enum TokenType type;
    int status;

    status = SUBTREE_OK;
    newTok = new_Token((char[1]){'\0'}, Pred);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    term = alloc_ParseTree();
    if (term == NULL)
        return MEMORY_ERROR;
    status = is_Term(tok, &term);
    if (status != SUBTREE_OK){
        free_ParseTree(term);
        return status;
    }
    (*new)->child = term;

    // The remainder of Pred is optional
    // (only if there is '+' | '-')
    type = (*tok)->token->type;
    if (match_AritmOp_type(type) &&
        !match_TermOp_type(type)){
        op = alloc_ParseTree();
        if (op == NULL)
            return MEMORY_ERROR;
        status = is_Operator(tok, &op);
        if (status != SUBTREE_OK){
            free_ParseTree(op);
            return status;
        }
        term->sibling = op;

        pred = alloc_ParseTree();
        if (pred == NULL)
            return MEMORY_ERROR;
        status = is_Pred(tok, &pred);
        if (status != SUBTREE_OK){
            free_ParseTree(pred);
            return status;
        }
        op->sibling = pred;
    }
    return status;
}

int is_Expr (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *pred, *op, *expr;
    struct Token *newTok;
    enum TokenType type;
    int status;

    status = SUBTREE_OK;
    newTok = new_Token((char[1]){'\0'}, Expr);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    pred = alloc_ParseTree();
    if (pred == NULL)
        return MEMORY_ERROR;
    status = is_Pred(tok, &pred);
    if (status != SUBTREE_OK){
        free_ParseTree(pred);
        return status;
    }
    (*new)->child = pred;

    // The remainder of Expr is optional
    // (only if there is a conditional operator)
    type = (*tok)->token->type;
    if (match_CondOp_type(type)){
        op = alloc_ParseTree();
        if (op == NULL)
            return MEMORY_ERROR;
        status = is_Operator(tok, &op);
        if (status != SUBTREE_OK){
            free_ParseTree(op);
            return status;
        }
        pred->sibling = op;

        expr = alloc_ParseTree();
        if (expr == NULL)
            return MEMORY_ERROR;
        status = is_Expr(tok, &expr);
        if (status != SUBTREE_OK){
            free_ParseTree(expr);
            return status;
        }
        op->sibling = expr;
    }
    return status;
}


int is_List (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *open, *listexpr, *close;
    int status, has_expr;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, List);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    status = SUBTREE_OK;
    has_expr = 0;
    open = alloc_ParseTree();
    if (open == NULL)
        return MEMORY_ERROR;

    status = is_Lbrack(tok, &open);
    if (status != SUBTREE_OK) {
        free_ParseTree(open);
        return status;
    }
    (*new)->child = open;

    if ((*tok)->token->type != Rbrack) {
        listexpr = alloc_ParseTree();
        if (listexpr == NULL)
            return MEMORY_ERROR;
        status = is_ListExpr(tok, &listexpr);
        if (status != SUBTREE_OK) {
            free_ParseTree(listexpr);
            return status;
        }
        has_expr = 1;
        open->sibling = listexpr;
    }

    close = alloc_ParseTree();
    if (close == NULL)
        return MEMORY_ERROR;
    status = is_Rbrack(tok, &close);
    if (status != SUBTREE_OK) {
        free_ParseTree(close);
        return status;
    }

    if (has_expr)
        listexpr->sibling = close;
    else
        open->sibling = close;
    return status;
}


int is_ListElem(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *var, *lbrack, *idx, *rbrack;
    int status;
    struct Token *newTok;

    status = SUBTREE_OK;
    newTok = new_Token((char[1]){'\0'}, ListElem);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    var = alloc_ParseTree();
    if (var == NULL)
        return MEMORY_ERROR;
    status = is_Var(tok, &var);
    if (status != SUBTREE_OK) {
        free_ParseTree(var);
        return status;
    }
    (*new)->child = var;

    lbrack = alloc_ParseTree();
    if (lbrack == NULL)
        return MEMORY_ERROR;
    status = is_Lbrack(tok, &lbrack);
    if (status != SUBTREE_OK) {
        free_ParseTree(lbrack);
        return status;
    }
    var->sibling = lbrack;

    idx = alloc_ParseTree();
    if (idx == NULL)
        return MEMORY_ERROR;
    // only int and var are allowed as list index
    if ((*tok)->token->type == Int)
        status = is_Int(tok, &idx);
    else
        status = is_Var(tok, &idx);
    if (status != SUBTREE_OK) {
        free_ParseTree(idx);
        return status;
    }
    lbrack->sibling = idx;

    rbrack = alloc_ParseTree();
    if (rbrack == NULL)
        return MEMORY_ERROR;
    status = is_Rbrack(tok, &rbrack);
    if (status != SUBTREE_OK) {
        free_ParseTree(rbrack);
        return status;
    }
    idx->sibling = rbrack;

    return status;
}


int is_ListExpr(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *obj, *comma, *last;
    int status;
    struct Token* newTok;

    status = SUBTREE_OK;
    newTok = new_Token((char[1]){'\0'}, ListExpr);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    obj = alloc_ParseTree();
    if (obj == NULL)
        return MEMORY_ERROR;

    status = is_Obj(tok, &obj);
    if (status != SUBTREE_OK) {
        free_ParseTree(obj);
        return status;
    }
    (*new)->child = obj;
    last = obj;

    while( (*tok)->token->type == Comma) {
        comma = alloc_ParseTree();
        if (comma == NULL)
            return MEMORY_ERROR;
        status = is_Comma(tok, &comma);
        if (status != SUBTREE_OK) {
            free_ParseTree(comma);
            return status;
        }
        last->sibling = comma;
        last = comma;
        obj = alloc_ParseTree();
        if (obj == NULL)
            return MEMORY_ERROR;
        status = is_Obj(tok, &obj);
        if (status != SUBTREE_OK) {
            free_ParseTree(obj);
            return status;
        }
        last->sibling = obj;
        last = obj;
    }
    return status;

}


int is_Str (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *quotedstr, *plus, *last;
    int status;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Str);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    status = SUBTREE_OK;
    quotedstr = alloc_ParseTree();
    if (quotedstr == NULL)
        return MEMORY_ERROR;

    status = is_QuotedStr(tok, &quotedstr);
    if (status != SUBTREE_OK) {
        free_ParseTree(quotedstr);
        return status;
    }
    (*new)->child = quotedstr;
    last = quotedstr;

    while( (*tok)->token->type == Plus) {
        plus = alloc_ParseTree();
        if (plus == NULL)
            return MEMORY_ERROR;
        status = is_Plus(tok, &plus);
        if (status != SUBTREE_OK) {
            free_ParseTree(plus);
            return status;
        }
        last->sibling = plus;
        last = plus;
        if ((*tok)->token->type != QuotedStr)
            return PARSING_ERROR;
        quotedstr = alloc_ParseTree();
        if (quotedstr == NULL)
            return MEMORY_ERROR;
        status = is_QuotedStr(tok, &quotedstr);
        if (status != SUBTREE_OK) {
            free_ParseTree(quotedstr);
            return status;
        }
        last->sibling = quotedstr;
        last = quotedstr;
    }
    return status;
}


int is_Frac (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *dot, *integer;
    int status;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Frac);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    status = SUBTREE_OK;
    dot = alloc_ParseTree();
    if (dot == NULL)
        return MEMORY_ERROR;

    status = is_Dot(tok, &dot);
    if (status != SUBTREE_OK) {
        free_ParseTree(dot);
        return status;
    }
    (*new)->child = dot;

    integer = alloc_ParseTree();
    if (integer == NULL)
        return MEMORY_ERROR;

    status = is_Int(tok, &integer);
    if (status != SUBTREE_OK) {
        free_ParseTree(integer);
        return status;
    }
    dot->sibling = integer;
    return status;
}


int is_Exp (struct TokenList** tok, struct ParseTree** new) {
    int status, has_sign;
    struct ParseTree *pow, *sign, *integer;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Pow);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    status = SUBTREE_OK;
    has_sign = 0;
    pow = alloc_ParseTree();

    if (pow == NULL)
        return MEMORY_ERROR;

    status = is_Pow(tok, &pow);
    if (status != SUBTREE_OK) {
        free_ParseTree(pow);
        return status;
    }
    (*new)->child = pow;

    sign = alloc_ParseTree();
    if (sign == NULL)
        return MEMORY_ERROR;

    if ((*tok)->token->type == Plus) {
        status = is_Plus(tok, &sign);
        has_sign = 1;
    }
    else if ((*tok)->token->type == Minus) {
        status = is_Minus(tok, &sign);
        has_sign = 1;
    }

    if (has_sign && (status != SUBTREE_OK)) {
        free_ParseTree(sign);
        return status;
    }
    if (has_sign)
        (*new)->child->sibling = sign;

    integer = alloc_ParseTree();
    if (integer == NULL)
        return MEMORY_ERROR;

    status = is_Int(tok, &integer);
    if (status != SUBTREE_OK) {
        free_ParseTree(integer);
        return status;
    }

    if (has_sign)
        sign->sibling = integer;
    else
        (*new)->child->sibling = integer;
    return status;
}


int is_Float(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *integer, *frac, *pow, *last;
    int status;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Float);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    status = SUBTREE_OK;

    if ((*tok)->token->type == Dot) {
        frac = alloc_ParseTree();
        if (frac == NULL)
            return MEMORY_ERROR;
        status = is_Frac(tok, &frac);
        if (status != SUBTREE_OK) {
            free_ParseTree(frac);
            return status;
        }
        (*new)->child = frac;
        last = frac;
        if ((*tok)->token->type == Pow) {
            pow = alloc_ParseTree();
            if (pow == NULL)
                return MEMORY_ERROR;
            status = is_Exp(tok, &pow);
            if (status != SUBTREE_OK) {
                free_ParseTree(pow);
                return status;
            }
            last->sibling = pow;
            last = pow;
        }
    }
    else if ((*tok)->token->type == Int) {
        integer = alloc_ParseTree();
        if (integer == NULL)
            return MEMORY_ERROR;
        status = is_Int(tok, &integer);
        if (status != SUBTREE_OK) {
            free_ParseTree(integer);
            return status;
        }
        (*new)->child = integer;
        last = integer;
        if ((*tok)->token->type == Dot) {
            frac = alloc_ParseTree();
            if (frac == NULL)
                return MEMORY_ERROR;
            status = is_Frac(tok, &frac);
            if (status != SUBTREE_OK) {
                free_ParseTree(frac);
                return status;
            }
            last->sibling = frac;
            last = frac;
        }
        if ((*tok)->token->type == Pow) {
            pow = alloc_ParseTree();
            if (pow == NULL)
                return MEMORY_ERROR;
            status = is_Exp(tok, &pow);
            if (status != SUBTREE_OK) {
                free_ParseTree(pow);
                return status;
            }
            last->sibling = pow;
            last = pow;
        }
    }
    else
        status = PARSING_ERROR;

    return status;
}


int is_Num(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *sign, *numeric;
    int status, has_sign;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Num);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    status = SUBTREE_OK;
    has_sign = 1;
    sign = alloc_ParseTree();

    if (sign == NULL)
        return MEMORY_ERROR;

    if ((*tok)->token->type == Plus)
        status = is_Plus(tok, &sign);
    else if ((*tok)->token->type == Minus)
        status = is_Minus(tok, &sign);
    else
        has_sign = 0;

    if (status != SUBTREE_OK) {
        free_ParseTree(sign);
        return status;
    }
    if (has_sign)
        (*new)->child = sign;
    else
        free_ParseTree(sign);

    numeric = alloc_ParseTree();
    if (numeric == NULL)
        return MEMORY_ERROR;

    status = is_Float(tok, &numeric);

    if (status != SUBTREE_OK)
        free_ParseTree(numeric);
    else {
        if (has_sign)
            sign->sibling = numeric;
        else
            (*new)->child = numeric;
    }
    return status;
}


int is_Obj(struct TokenList** tok, struct ParseTree** new) {
    struct Token* newTok;
    struct ParseTree* subtree;
    int status;

    newTok = new_Token((char[1]){'\0'}, Obj);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    subtree = alloc_ParseTree();
    if (subtree == NULL)
        return MEMORY_ERROR;

    // use LL(1) FOLLOW Sets
    struct TokenList* curr;
    curr = *tok;
    if (curr->token->type == Var) {
        if (curr->next->token->type == Lbrack)
            status = is_ListElem(tok, &subtree);
        else
            status = is_Var(tok, &subtree);
    }
    else if (curr->token->type == Null)
        status = is_Null(tok, &subtree);
    else if (curr->token->type == Lbrack)
        status = is_List(tok, &subtree);
    else if (curr->token->type == QuotedStr)
        status = is_Str(tok, &subtree);
    else if (curr->token->type == Bool)
        status = is_Bool(tok, &subtree);
    else if (curr->token->type == Int ||
             curr->token->type == Dot ||
             curr->token->type == Plus ||
             curr->token->type == Minus)
        status = is_Num(tok, &subtree);
    else
        status = PARSING_ERROR;

    if (status == SUBTREE_OK)
        (*new)->child = subtree;
    else
        free_ParseTree(subtree);
    return status;
}



int is_Assign(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *var1, *eq, *var2;
    int status;
    struct Token *newTok;

    newTok = new_Token((char[1]) {'\0'}, Assign);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    var1 = alloc_ParseTree();
    if (var1 == NULL)
        return MEMORY_ERROR;
    status = is_Var(tok, &var1);
    if (status != SUBTREE_OK) {
        free_ParseTree(var1);
        return PARSING_ERROR;
    }
    (*new)->child = var1;

    eq = alloc_ParseTree();
    if (eq == NULL)
        return MEMORY_ERROR;
    status = is_Equal(tok, &eq);
    if (status != SUBTREE_OK) {
        free_ParseTree(eq);
        return PARSING_ERROR;
    }
    var1->sibling = eq;

    var2 = alloc_ParseTree();
    if (var2 == NULL)
        return MEMORY_ERROR;

    status = is_Expr(tok, &var2);
    if (status != SUBTREE_OK) {
        free_ParseTree(var2);
        return PARSING_ERROR;
    }
    eq->sibling = var2;
    return SUBTREE_OK;
}


int is_Input(struct TokenList** tok, struct ParseTree** new) {
    // Create subtree
    struct ParseTree *readin, *var;
    int status;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Input);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    readin = alloc_ParseTree();
    if (readin == NULL)
        return MEMORY_ERROR;
    status = is_readIn(tok, &readin);
    if (status != SUBTREE_OK) {
        free_ParseTree(readin);
        return status;
    }
    (*new)->child = readin;

    var = alloc_ParseTree();
    if (var == NULL)
        return MEMORY_ERROR;
    status = is_Var(tok, &var);
    if (status != SUBTREE_OK) {
        free_ParseTree(var);
        return status;
    }
    readin->sibling = var;

    return SUBTREE_OK;
}


int is_Output(struct TokenList** tok, struct ParseTree** new) {
    // Create subtree
    struct ParseTree *writeOut, *obj;
    int status;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Output);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    writeOut = alloc_ParseTree();
    if (writeOut == NULL)
        return MEMORY_ERROR;
    status = is_writeOut(tok, &writeOut);
    if (status != SUBTREE_OK) {
        free_ParseTree(writeOut);
        return status;
    }
    (*new)->child = writeOut;

    obj = alloc_ParseTree();
    if (obj == NULL)
        return MEMORY_ERROR;
    status = is_Obj(tok, &obj);
    if (status != SUBTREE_OK) {
        free_ParseTree(obj);
        return status;
    }
    writeOut->sibling = obj;

    return status;
}


int is_OptElse (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *elsetok, *line, *endline, *last;
    int status;
    struct Token* newTok;

    status = SUBTREE_OK;

    newTok = new_Token((char[1]){'\0'}, OptElse);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    elsetok = alloc_ParseTree();
    if (elsetok == NULL)
        return MEMORY_ERROR;
    status = is_Else(tok, &elsetok);
    (*new)->child = elsetok;
    last = elsetok;
    if (status != SUBTREE_OK)
        return status;
    do{
        // else cannot be empty
        line = alloc_ParseTree();
        if (line == NULL)
            return MEMORY_ERROR;
        status = is_Line(tok, &line);
        last->sibling = line;
        last = line;
        if (status != SUBTREE_OK)
            return status;
        endline = alloc_ParseTree();
        if (endline == NULL)
            return MEMORY_ERROR;
        status = is_Endline(tok, &endline);
        last->sibling = endline;
        last = endline;
        if (status != SUBTREE_OK)
            return status;
    }
    while ( (*tok) != NULL && (*tok)->token->type != Endline);
    return status;
}


int is_IfBody (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *line, *endline, *optelse, *last;
    int status, count;
    struct Token* newTok;

    status = SUBTREE_OK;
    count = 0;

    newTok = new_Token((char[1]){'\0'}, IfBody);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;
    last = NULL;

    while ( (*tok) != NULL &&
            (*tok)->token->type != Endline &&
            (*tok)->token->type != Else) {
        count++;
        line = alloc_ParseTree();
        if (line == NULL)
            return MEMORY_ERROR;
        status = is_Line(tok, &line);
        if (last == NULL)
            (*new)->child = line;
        else
            last->sibling = line;
        last = line;
        if (status != SUBTREE_OK)
            return status;

        endline = alloc_ParseTree();
        if (endline == NULL)
            return MEMORY_ERROR;
        status = is_Endline(tok, &endline);
        last->sibling = endline;
        last = endline;
        if (status != SUBTREE_OK)
            return status;
    }

    if (count == 0) {
        printf("IfBody cannot be empty\n");
        return PARSING_ERROR;
    }

    if ((*tok) != NULL &&
        (*tok)->token->type == Else) {
        // must be the Else branch
        optelse = alloc_ParseTree();
        if (optelse == NULL)
            return MEMORY_ERROR;
        status = is_OptElse(tok, &optelse);
        if (last == NULL)
            (*new)->child = optelse;
        else
            last->sibling = optelse;
    }
    return status;
}

int is_IfCond (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *lpar, *expr, *rpar;
    int status;
    struct Token* newTok;
    status = SUBTREE_OK;

    newTok = new_Token((char[1]){'\0'}, IfCond);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    lpar = alloc_ParseTree();
    if (lpar == NULL)
        return MEMORY_ERROR;
    status = is_Lpar(tok, &lpar);
    (*new)->child = lpar;
    if (status != SUBTREE_OK)
        return status;

    expr = alloc_ParseTree();
    if (expr == NULL)
        return MEMORY_ERROR;
    status = is_Expr(tok, &expr);
    lpar->sibling = expr;
    if (status != SUBTREE_OK)
        return status;

    rpar = alloc_ParseTree();
    if (rpar == NULL)
        return MEMORY_ERROR;
    status = is_Rpar(tok, &rpar);
    expr->sibling = rpar;
    if (status != SUBTREE_OK)
        return status;

    return status;
}


int is_IfLine (struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *iftok, *ifcond, *ifbody;
    int status;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, IfLine);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    status = SUBTREE_OK;

    iftok = alloc_ParseTree();
    if (iftok == NULL)
        return MEMORY_ERROR;
    status = is_If(tok, &iftok);
    (*new)->child = iftok;
    if (status != SUBTREE_OK)
        return status;

    ifcond = alloc_ParseTree();
    if (ifcond == NULL)
        return MEMORY_ERROR;
    status = is_IfCond(tok, &ifcond);
    iftok->sibling = ifcond;
    if (status != SUBTREE_OK)
        return status;

    ifbody = alloc_ParseTree();
    if (ifbody == NULL)
        return MEMORY_ERROR;
    status = is_IfBody(tok, &ifbody);
    ifcond->sibling = ifbody;

    return status;
}


int is_LoopLine(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *whilekey, *ifcond, *loopbody;
    int status;
    struct Token* newTok;

    status = SUBTREE_OK;
    newTok = new_Token((char[1]){'\0'}, LoopLine);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;

    whilekey = alloc_ParseTree();
    if (whilekey == NULL)
        return MEMORY_ERROR;
    status = is_While(tok, &whilekey);
    (*new)->child = whilekey;
    if (status != SUBTREE_OK)
        return status;

    ifcond = alloc_ParseTree();
    if (ifcond == NULL)
        return MEMORY_ERROR;
    status = is_IfCond(tok, &ifcond);
    whilekey->sibling = ifcond;
    if (status != SUBTREE_OK)
        return status;

    loopbody = alloc_ParseTree();
    if (loopbody == NULL)
        return MEMORY_ERROR;
    status = is_Program(tok, &loopbody);
    if (status != SUBTREE_OK) {
        free_ParseTree(loopbody);
        return status;
    }
    ifcond->sibling = loopbody;

    return status;
}


int is_Line(struct TokenList** tok, struct ParseTree** line) {
    // Create subtree
    struct ParseTree* subtree;
    struct Token* newTok;
    int status;
    struct TokenList* curr;

    newTok = new_Token((char[1]){'\0'}, Line);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*line)->data = newTok;

    subtree = alloc_ParseTree();
    if (subtree == NULL)
        return MEMORY_ERROR;

    curr = *tok;
    if (curr->token->type == ReadIn)
        status = is_Input(tok, &subtree);
    else if (curr->token->type == WriteOut)
        status = is_Output(tok, &subtree);
    else if (curr->token->type == If)
        status = is_IfLine(tok, &subtree);
    else if (curr->token->type == While)
        status = is_LoopLine(tok, &subtree);
    else if (curr->token->type == Continue)
        status = is_Continue(tok, &subtree);
    else if (curr->token->type == Break)
        status = is_Break(tok, &subtree);
    else if (curr->token->type == Var)
        status = is_Assign(tok, &subtree);
    else
        status = PARSING_ERROR;

    (*line)->child = subtree;
    return status;
}


int is_Program(struct TokenList** head, struct ParseTree** tree) {
    struct Token *tok;
    int status, child;
    struct ParseTree *current;
    struct ParseTree *line, *endline;
    
    tok = new_Token((char[1]){'\0'}, Program);
    if (tok == NULL)
        return MEMORY_ERROR;
    (*tree)->data = tok;

    // Entry point of any valid tokens sequence for this grammar.
    line = alloc_ParseTree();
    endline = alloc_ParseTree();
    if (line == NULL || endline == NULL) {
        free(line);
        free(endline);
        return MEMORY_ERROR;
    }

    current = *tree; // save pointer to root

    /*
     * Nested call to identify grammar tokens return pointer to subtree(s)
     * or NULL is parsing/memory error happened.
    */
    
    child = 1; // true - the first Line is always a child of Program
    // A program is a (possibly empty) sequence of 'line' 'endline'
    while ((*head) != NULL &&
           (*head)->token->type != Endline) {
        // Match Line
        status = is_Line(head, &line);
        if (child) {
            current->child = line;
            child = !child;
        }
        else
            current->sibling = line;
        current = line;
        line = alloc_ParseTree();
        if (status != SUBTREE_OK) {
            printf("ERROR PARSING LINE: %d\n", status);
            break;
        }

        // Match Endline
        status = is_Endline(head, &endline);
        current->sibling = endline;
        current = endline;
        endline = alloc_ParseTree();
        if (status != SUBTREE_OK) {
            printf("Missing ENDLINE\n");
            break;
        }
    }
    free_ParseTree(line);
    free_ParseTree(endline);

    return status;
}


int build_ParseTree (struct TokenList* head, struct ParseTree** tree) {
    // Always match the start symbol of this grammar: Program.
    int status;

    status = is_Program(&head, tree);
    return status;
}


int build_ParseTree_FromFile (const char *fileName, struct ParseTree **tree) {
    FILE *file;
    char *vec;
    struct stat buffer;
    struct TokenList *list, *list2;
    int status;

    file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Cannot read the given file path.\n");
        return MEMORY_ERROR;
    }
    stat(fileName, &buffer);
    vec = malloc((buffer.st_size + 1) * sizeof(char));
    if (vec == NULL)
        return MEMORY_ERROR;
    fread(vec, sizeof(char), buffer.st_size, file);
    fclose(file);
    *(vec+buffer.st_size) = '\0';
    const char *fp = vec;

    /*
     * Build Token list
    */
    // Will also print total count
    list = build_TokenList(fp);

    if (list == NULL)
        return MEMORY_ERROR; // TODO - buildTokenList should return int too
    list2 = strip_WS(list);
    free_TokenList(list);

    status = build_ParseTree(list2, tree);

    free_TokenList(list2);
    free(vec);
    return status;
}
