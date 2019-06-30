#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cgen.h"


char* cgen_Str (struct ParseTree* tree);
char* cgen_List (struct ParseTree* tree);
char* cgen_ListElem (struct ParseTree* tree);
char* cgen_Expr (struct ParseTree* tree);
char* cgen_Pred (struct ParseTree* tree);
char* cgen_Term (struct ParseTree* tree);
char* cgen_BaseExpr (struct ParseTree* tree);
char* cgen_IfLine (struct ParseTree* tree, int indent);
char* cgen_IfBody (struct ParseTree* tree, int indent);


int str_insert (char** s1, char* s2, int pos) {
    /*
        Insert a sequence of characters into a string, starting from a given index.
        The old string is moved to a new memory location, and the old memory is freed.

        - **s1: The string to modify.
        - *s2: The sequence of characters to insert into s1.
        - pos: The starting index.
    */
    if (! s2)
        return -1;

    char* new;
    int l1, l2;

    l1 = strlen(*s1);
    if (pos > l1 || pos < 0)
        return -1;

    l2 = strlen(s2);
    if (l2 <= 0)
        return 0;

    new = calloc(l1 + l2 + 1, sizeof(char));
    if (! new)
        return -1;

    memcpy(new + pos, s2, l2 * sizeof(char));
    memcpy(new + pos + l2, *s1 + pos, (l1 - pos) * sizeof(char));
    memcpy(new, *s1, pos * sizeof(char));

    free(*s1);
    *s1 = new;
    return 0;
}


char* cgen_Int (struct ParseTree* tree) {
    if (! tree|| tree->data->type != Int)
        return NULL;

    char* integer;
    int len;

    len = strlen(tree->data->lexeme);
    integer = calloc(len + 1, sizeof(char));
    memcpy(integer, tree->data->lexeme, len);
    return integer;
}


char* cgen_Pow (struct ParseTree* tree) {
    if (! tree|| tree->data->type != Pow)
        return NULL;

    char *result, *integer;
    char prefix[3];
    int l_int;

    tree = tree->child; // must be the '^'
    tree = tree->sibling; // either Int or Plus/Minus

    if (tree->data->type == Minus)
        memcpy(prefix, "e-\0", 3*sizeof(char));
    else
        memcpy(prefix, "e+\0", 3*sizeof(char));

    if (tree->data->type != Int)
        tree = tree->sibling;
    
    integer = cgen_Int(tree);
    l_int = strlen(integer);

    result = calloc(2 + l_int + 1, sizeof(char));
    memcpy(result, prefix, 2 * sizeof(char));
    memcpy(result + 2, integer, l_int * sizeof(char));

    free(integer);
    return result;
}


char* cgen_Frac (struct ParseTree* tree) {
    if (! tree|| tree->data->type != Frac)
        return NULL;

    char *integer, *result;
    int l_int;

    tree = tree->child->sibling; // skip the Dot
    
    integer = cgen_Int(tree);
    l_int = strlen(integer);

    result = calloc(1 + l_int + 1, sizeof(char));
    memcpy(result, ".", 1 * sizeof(char));
    memcpy(result + 1, integer, l_int * sizeof(char));

    free(integer);
    return result;
}


char* cgen_Float (struct ParseTree* tree) {
    if (! tree || tree->data->type != Float)
        return NULL;

    char *integer, *frac, *pow, *result;
    int l_int, l_frac, l_pow;

    integer = frac = pow = NULL;
    l_int = l_frac = l_pow = 0;
    tree = tree->child;

    if (tree->data->type == Int) {
        integer = cgen_Int(tree);
        tree = tree->sibling;
    }
    if (tree != NULL && tree->data->type == Frac) {
        frac = cgen_Frac(tree);
        tree = tree->sibling;
    }
    if (tree != NULL && tree->data->type == Pow)
        pow = cgen_Pow(tree);

    if (integer)
        l_int = strlen(integer);
    if (frac)
        l_frac = strlen(frac);
    if (pow)
        l_pow = strlen(pow); // pow contains the 'e' too

    result = calloc(l_int + l_frac + l_pow + 1, sizeof(char));

    if (integer)
        memcpy(result, integer, l_int * sizeof(char));
    if (frac)
        memcpy(result + l_int, frac, l_frac * sizeof(char));
    if (pow)
        memcpy(result + l_int + l_frac, pow, l_pow * sizeof(char));

    free(integer);
    free(frac);
    free(pow);
    return result;
}


char* cgen_Num (struct ParseTree* tree) {
    if (! tree || tree->data->type != Num)
        return NULL;

    char *result, *aFloat;
    int sign, l_float;

    tree = tree->child; // either Float or Plus/Minus
    if (tree->data->type == Minus) {
        sign = -1;
        tree = tree->sibling;
    }
    else {
        sign = 1;
        if (tree->data->type == Plus)
            tree = tree->sibling;
    }
    aFloat = cgen_Float(tree);
    l_float = strlen(aFloat);

    result = calloc(1 + l_float + 1, sizeof(char));
    if (sign > 0)
        memcpy(result, "+", sizeof(char));
    else
        memcpy(result, "-", sizeof(char));
    memcpy(result + 1, aFloat, l_float * sizeof(char));

    free(aFloat);
    return result;
}


char* cgen_Bool (struct ParseTree* tree) {
    if (! tree || tree->data->type != Bool)
        return NULL;

    char* result;
    int l_bool;

    l_bool = strlen(tree->data->lexeme);
    result = calloc(l_bool + 1, sizeof(char));
    if (! result)
        return NULL;

    if (l_bool == 4)
        memcpy(result, "True", 4 * sizeof(char));
    else
        memcpy(result, "False", 5 * sizeof(char));

    return result;
}


char* cgen_Null (struct ParseTree* tree) {
    if (! tree || tree->data->type != Null)
        return NULL;

    char* result;
    result = calloc(5, sizeof(char));
    if (! result)
        return NULL;
    memcpy(result, "None", 4 * sizeof(char));
    return result;
}


char* cgen_Var (struct ParseTree* tree) {
    if (! tree || tree->data->type != Var)
        return NULL;

    char* result;
    int l_var;

    l_var = strlen(tree->data->lexeme);
    result = calloc(l_var + 1, sizeof(char));
    if (! result)
        return NULL;
    memcpy(result, tree->data->lexeme, l_var * sizeof(char));
    return result;
}


char* cgen_Obj (struct ParseTree* tree) {
    if (! tree || tree->data->type != Obj)
        return NULL;

    if (tree->child->data->type == Var)
        return cgen_Var(tree->child);
    if (tree->child->data->type == Num)
        return cgen_Num(tree->child);
    if (tree->child->data->type == Str)
        return cgen_Str(tree->child);
    if (tree->child->data->type == Bool)
        return cgen_Bool(tree->child);
    if (tree->child->data->type == List)
       return cgen_List(tree->child);
    if (tree->child->data->type == ListElem)
        return cgen_ListElem(tree->child);
    return NULL;
}


char* _bail_out_Str (char* vec[], int count) {
    for (int i=0; i<count; i++)
        free(vec[i]);
    return NULL;
}


char* cgen_QuotedStr (struct ParseTree* tree) {
    if (! tree || tree->data->type != QuotedStr)
        return NULL;

    char *result, *obj;
    int total, last, count, l_str;
    struct ParseTree *tmp;

    tmp = tree->child; // actual quoted string node

    total = strlen(tmp->data->lexeme);
    last = 0;
    count = 1; // counting the first quoted string too
    obj = NULL;

    // Count total Obj
    while (tmp->sibling != NULL) {
        count ++;
        tmp = tmp->sibling->sibling; // skip Comma
    }
    if (count > 1)
        total += 4; // the padding " %(" ... ")"


    // To avoid triple pass store computed Obj(s)
    char* objs[count];
    count = 0;

    tmp = tree->child; // start over

    l_str = strlen(tmp->data->lexeme);
    objs[count] = calloc(l_str + 1, sizeof(char));
    if (objs[count] == NULL)
        return NULL;
    memcpy(objs[count], tmp->data->lexeme, l_str * sizeof(char));
    count ++;

    while (tmp->sibling != NULL) {
        tmp = tmp->sibling->sibling;
        obj = cgen_Obj(tmp);
        if (obj == NULL)
            return _bail_out_Str(objs, count);
        objs[count++] = obj;
        total += strlen(obj) + 1; // + 1 for the ","
    }

    result = calloc(total + 1, sizeof(char));
    if (! result)
        return _bail_out_Str(objs, count);

    for (int i=0; i<count; i++) {
        l_str = strlen(objs[i]);
        memcpy(result + last, objs[i], l_str * sizeof(char));
        last += l_str;
        free(objs[i]);
        if (i == 0 && count > 1) {
            result[last++] = ' ';
            result[last++] = '%';
            result[last++] = '(';
        }
        if (i > 0 && i < count - 1)
            result[last++] = ',';
    }
    if (count > 1)
        result[last] = ')';
    return result;
}


char* cgen_Str (struct ParseTree* tree) {
    if (! tree || tree->data->type != Str)
        return NULL;

    char *result, *quoted;
    int total, last, count, l_str;
    struct ParseTree *tmp;

    total = last = l_str = 0;
    count = 1; // At the least 1 QuotedStr will be there
    tmp = tree->child; // The first QuotedStr

    // Count total quoted strings
    while (tmp->sibling != NULL) {
        count ++;
        tmp = tmp->sibling->sibling;
    }

    // To avoid triple pass, store strings while calculating the length
    char* strs[count];
    count = 0;
    tmp = tree->child; // start over

    quoted = cgen_QuotedStr(tmp);
    strs[count++] = quoted;
    total += strlen(quoted);

    while (tmp->sibling != NULL) {
        total += 3; // the padding " + "
        tmp = tmp->sibling->sibling;
        quoted = cgen_QuotedStr(tmp);
        if (quoted == NULL)
            return _bail_out_Str(strs, count);
        strs[count++] = quoted;
        total += strlen(quoted);
    }

    result = calloc(total + 1, sizeof(char));
    if (! result)
        return _bail_out_Str(strs, count);

    for (int i=0; i<count; i++) {
        l_str = strlen(strs[i]);
        memcpy(result + last, strs[i], l_str * sizeof(char));
        last += l_str;
        free(strs[i]);
        if (i < count - 1) {
            result[last++] = ' ';
            result[last++] = '+';
            result[last++] = ' ';
        }
    }
    return result;
}


char* cgen_ListExpr (struct ParseTree* tree) {
    if (! tree || tree->data->type != ListExpr)
        return NULL;

    char *obj, *result;
    int l_obj, total, count, last;;
    struct ParseTree *tmp;

    total = 0;
    count = 1;
    tmp = tree->child;

    while (tmp->sibling != NULL) {
        tmp = tmp->sibling->sibling;
        count++;
    }

    char* objs[count];
    count = last = 0;
    tmp = tree->child;
    while (1) {
        obj = cgen_Obj(tmp);
        if (obj == NULL)
            return _bail_out_Str(objs, count);
        objs[count++] = obj;
        total += strlen(obj);
        if (tmp->sibling != NULL) {
            tmp = tmp->sibling->sibling;
            total += 1; // for the ','
        }
        else
            break;
    }

    result = calloc(total + 1, sizeof(char));
    if (! result)
        return _bail_out_Str(objs, count);
    for (int i=0; i<count; i++) {
        l_obj = strlen(objs[i]);
        memcpy(result + last, objs[i], l_obj * sizeof(char));
        free(objs[i]);
        last += l_obj;
        if (i < count - 1)
            result[last++] = ',';
    }
    return result;
}


char* cgen_List (struct ParseTree* tree) {
    if (! tree || tree->data->type != List)
        return NULL;

    char *lexpr, *result;
    int l_expr, total;

    lexpr = cgen_ListExpr(tree->child->sibling);
    if (! lexpr)
        return NULL;
    l_expr = strlen(lexpr);
    total = l_expr + 2; //

    result = calloc(total + 1, sizeof(char));
    if (! result) {
        free(lexpr);
        return NULL;
    }

    result[0] = '[';
    memcpy(result + 1, lexpr, l_expr * sizeof(char));
    result[l_expr + 1] = ']';
    free(lexpr);
    return result;
}


char* cgen_ListElem (struct ParseTree* tree) {
    if (! tree || tree->data->type != ListElem)
        return NULL;

    char *result, *var, *idx;
    int total, l_var, l_idx;

    total = 0;

    var = cgen_Var(tree->child);
    if (var == NULL)
        return NULL;
    l_var = strlen(var);
    total += l_var;

    tree = tree->child->sibling->sibling;

    if (tree->data->type == Int)
        idx = cgen_Int(tree);
    else
        idx = cgen_Var(tree);
    if (idx == NULL) {
        free(var);
        return NULL;
    }
    l_idx = strlen(idx);
    total += l_idx;

    result = calloc(total + 3, sizeof(char));
    if (! result) {
        free(var);
        free(idx);
        return NULL;
    }
    memcpy(result, var, l_var * sizeof(char));
    result[l_var] = '[';
    memcpy(result + l_var + 1, idx, l_idx * sizeof(char));
    result[l_var + 1 + l_idx] = ']';

    free(var);
    free(idx);
    return result;
}


char* cgen_Op (struct ParseTree* tree) {
    char *result;
    if (tree->data->type == Plus){
        result = calloc(2, sizeof(char));
        result[0] = '+';
        return result;
    }
    if (tree->data->type == Minus){
        result = calloc(2, sizeof(char));
        result[0] = '-';
        return result;
    }
    if (tree->data->type == Star){
        result = calloc(2, sizeof(char));
        result[0] = '*';
        return result;
    }
    if (tree->data->type == Div || tree->data->type == FloatDiv){
        result = calloc(2, sizeof(char));
        result[0] = '/';
        return result;
    }
    if (tree->data->type == Percent){
        result = calloc(2, sizeof(char));
        result[0] = '%';
        return result;
    }
    if (tree->data->type == And){
        result = calloc(4, sizeof(char));
        memcpy(result, "and", 3 * sizeof(char));
        return result;
    }
    if (tree->data->type == Or){
        result = calloc(3, sizeof(char));
        memcpy(result, "or", 2 * sizeof(char));
        return result;
    }
    if (tree->data->type == NotEq){
        result = calloc(3, sizeof(char));
        memcpy(result, "!=", 2 * sizeof(char));
        return result;
    }
    if (tree->data->type == EqEq){
        result = calloc(3, sizeof(char));
        memcpy(result, "==", 2 * sizeof(char));
        return result;
    }
    if (tree->data->type == LesserEq){
        result = calloc(3, sizeof(char));
        memcpy(result, "<=", 2 * sizeof(char));
        return result;
    }
    if (tree->data->type == GreaterEq){
        result = calloc(3, sizeof(char));
        memcpy(result, ">=", 2 * sizeof(char));
        return result;
    }
    if (tree->data->type == Greater){
        result = calloc(2, sizeof(char));
        result[0] = '>';
        return result;
    }
    if (tree->data->type == Lesser){
        result = calloc(2, sizeof(char));
        result[0] = '<';
        return result;
    }
    return NULL;
}


char* cgen_BaseExpr (struct ParseTree* tree) {
    if (! tree || tree->data->type != BaseExpr)
        return NULL;

    char* result, *subexpr;
    int l_sub;

    result = subexpr = NULL;

    if (tree->child->data->type == Obj)
        result = cgen_Obj(tree->child);
    else {
        // must be ( Expr )
        subexpr = cgen_Expr(tree->child->sibling);
        if (subexpr == NULL)
            return NULL;
        l_sub = strlen(subexpr);
        result = calloc(l_sub + 3, sizeof(char));
        if (! result) {
            free(subexpr);
            return NULL;
        }
        result[0] = '(';
        memcpy(result + 1, subexpr, l_sub * sizeof(char));
        result[1 + l_sub] = ')';
    }
    free(subexpr);
    return result;
}


char* cgen_Term (struct ParseTree* tree) {
    if (! tree || tree->data->type != Term)
        return NULL;

    char *basexpr, *op, *term, *result;
    int l_base, l_op, l_term;

    basexpr = op = term = result = NULL;
    l_op = l_term = 0;
    tree = tree->child; // must be BaseExpr

    basexpr = cgen_BaseExpr(tree);
    if (basexpr == NULL)
        return NULL;
    l_base = strlen(basexpr);

    if (tree->sibling != NULL) {
        tree = tree->sibling; // the Op
        op = cgen_Op(tree);
        if (op == NULL) {
            free(basexpr);
            return NULL;
        }
        l_op = strlen(op);
        tree = tree->sibling; // the other Term
        term = cgen_Term(tree);
        if (term == NULL) {
            free(op);
            free(basexpr);
            return NULL;
        }
        l_term = strlen(term);
    }

    if (l_op > 0)
        result = calloc(l_base + 2 + l_op + l_term + 1, sizeof(char));
    else
        result = calloc(l_base + l_op + l_term + 1, sizeof(char));
    if (! result) {
        free(basexpr);
        free(op);
        free(term);
        return NULL;
    }
    memcpy(result, basexpr, l_base * sizeof(char));
    if (l_op > 0) {
        result[l_base] = ' ';
        memcpy(result + l_base + 1, op, l_op * sizeof(char));
        result[l_base + 1 + l_op] = ' ';
        memcpy(result + l_base + l_op + 2, term, l_term * sizeof(char));
    }
    free(basexpr);
    free(op);
    free(term);
    return result;
}


char* cgen_Pred (struct ParseTree* tree) {
    if (! tree || tree->data->type != Pred)
        return NULL;

    char *term, *op, *pred, *result;
    int l_term, l_op, l_pred;

    term = op = pred = result = NULL;
    l_op = l_pred = 0;
    tree = tree->child; // must be Term

    term = cgen_Term(tree);
    if (term == NULL)
        return NULL;
    l_term = strlen(term);

    if (tree->sibling != NULL) {
        tree = tree->sibling; // the Op
        op = cgen_Op(tree);
        if (op == NULL) {
            free(term);
            return NULL;
        }
        l_op = strlen(op);
        tree = tree->sibling; // the other Pred
        pred = cgen_Pred(tree);
        if (pred == NULL) {
            free(op);
            free(term);
            return NULL;
        }
        l_pred = strlen(pred);
    }

    if (l_op > 0)
        result = calloc(l_term + 2 + l_op + l_pred + 1, sizeof(char));
    else
        result = calloc(l_term + l_op + l_pred + 1, sizeof(char));
    if (! result) {
        free(term);
        free(op);
        free(pred);
        return NULL;
    }
    memcpy(result, term, l_term * sizeof(char));
    if (l_op > 0) {
        result[l_term] = ' ';
        memcpy(result + l_term + 1, op, l_op * sizeof(char));
        result[l_term + 1 + l_op] = ' ';
        memcpy(result + l_term + l_op + 2, pred, l_pred * sizeof(char));
    }
    free(term);
    free(op);
    free(pred);
    return result;
}


char* cgen_Expr (struct ParseTree* tree) {
    if (! tree || tree->data->type != Expr)
        return NULL;

    char *pred, *op, *expr, *result;
    int l_pred, l_op, l_expr;

    pred = op = expr = result = NULL;
    l_pred = l_op = l_expr = 0;
    tree = tree->child; // must be Pred

    pred = cgen_Pred(tree);
    if (pred == NULL)
        return NULL;
    l_pred = strlen(pred);

    if (tree->sibling != NULL) {
        tree = tree->sibling; // the Op
        op = cgen_Op(tree);
        if (op == NULL) {
            free(pred);
            return NULL;
        }
        l_op = strlen(op);
        tree = tree->sibling; // the other Expr
        expr = cgen_Expr(tree);
        if (expr == NULL) {
            free(op);
            free(pred);
            return NULL;
        }
        l_expr = strlen(expr);
    }

    if (l_op > 0)
        result = calloc(l_pred + 2 + l_op + l_expr + 1, sizeof(char));
    else
        result = calloc(l_pred + l_op + l_expr + 1, sizeof(char));
    if (! result) {
        free(pred);
        free(op);
        free(expr);
        return NULL;
    }
    memcpy(result, pred, l_pred * sizeof(char));
    if (l_op > 0) {
        result[l_pred] = ' ';
        memcpy(result + l_pred + 1, op, l_op * sizeof(char));
        result[l_pred + 1 + l_op] = ' ';
        memcpy(result + l_pred + l_op + 2, expr, l_expr * sizeof(char));
    }
    free(pred);
    free(op);
    free(expr);
    return result;
}


char* cgen_Input (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Input)
        return NULL;

    char *var, *input, *result;
    int l_var, l_input, last;

    var = tree->child->sibling->data->lexeme;
    l_var = strlen(var);

    if (strcmp(tree->child->data->lexeme, "readInt") == 0)
        input = "int(input())";
    else if (strcmp(tree->child->data->lexeme, "readFloat") == 0)
        input = "float(input())";
    else if (strcmp(tree->child->data->lexeme, "readStr") == 0)
        input = "input()";
    else
        input = "bool(input())";
    l_input = strlen(input);

    result = calloc(indent + l_var + 4 + l_input, sizeof(char));
    if (! result)
        return NULL;
    last = 0;
    if (indent > 0) {
        memset(result, ' ', indent * sizeof(char));
        last += indent;
    }
    memcpy(result + last, var, l_var * sizeof(char));
    last += l_var;
    result[last++] = ' ';
    result[last++] = '=';
    result[last++] = ' ';
    memcpy(result + last, input, l_input * sizeof(char));
    return result;
}


char* cgen_Output (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Output)
        return NULL;

    char *result, *obj;
    int l_obj, last;

    obj = cgen_Obj(tree->child->sibling);
    if (obj == NULL)
        return NULL;
    l_obj = strlen(obj);

    result = calloc(l_obj + indent + 8, sizeof(char));
    if (! result) {
        free(obj);
        return NULL;
    }
    last = 0;
    if (indent > 0){
        memset(result, ' ', indent * sizeof(char));
        last += indent;
    }
    memcpy(result + last, "print(", 6 * sizeof(char));
    last += 6;
    memcpy(result + last, obj, l_obj * sizeof(char));
    last += l_obj;
    result[last] = ')';

    free(obj);
    return result;
}


char* cgen_Assign (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Assign)
        return NULL;

    char *result, *expr, *var;
    int l_expr, l_var, last;

    var = cgen_Var(tree->child);
    if (var == NULL)
        return NULL;
    l_var = strlen(var);

    expr = cgen_Expr(tree->child->sibling->sibling);
    if (expr == NULL) {
        free(var);
        return NULL;
    }
    l_expr = strlen(expr);

    result = calloc(l_var + indent + 4 + l_expr, sizeof(char));
    if (! result) {
        free(expr);
        free(var);
        return NULL;
    }
    last = 0;
    if (indent > 0) {
        memset(result, ' ', indent * sizeof(char));
        last += indent;
    }
    memcpy(result + last, var, l_var * sizeof(char));
    last += l_var;
    result[last++] = ' ';
    result[last++] = '=';
    result[last++] = ' ';
    memcpy(result + last, expr, l_expr * sizeof(char));

    free(var);
    free(expr);
    return result;
}


char* cgen_Line (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Line)
        return NULL;

    char *line, *result;
    int l_line;
    enum TokenType type;

    type = tree->child->data->type;
    if (type == Assign)
        line = cgen_Assign(tree->child, indent);
    else if (type == Output)
        line = cgen_Output(tree->child, indent);
    else if (type == Input)
        line = cgen_Input(tree->child, indent);
    else if (type == IfLine)
        line = cgen_IfLine(tree->child, indent);
    else if (type == LoopLine)
        line = cgen_LoopLine(tree->child, indent);
    else if (type == Break)
        line = cgen_Break(tree->child, indent);
    else if (type == Continue)
        line = cgen_Continue(tree->child, indent);
    else
        line = NULL;

    if (line == NULL)
        return NULL;

    l_line = strlen(line);
    result = calloc(l_line + 2, sizeof(char));
    if (! result) {
        free(line);
        return NULL;
    }
    memcpy(result, line, l_line * sizeof(char));
    // Add the ENDLINE TOKEN
    memset(result + l_line, '\n', sizeof(char));

    free(line);
    return result;
}


char* cgen_IfLine (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != IfLine)
        return NULL;

    char *result, *ifcond, *ifbody;
    struct ParseTree *cond, *body;
    int l_cond, l_body, last;

    cond = tree->child->sibling->child->sibling;
    ifcond = cgen_Expr(cond);
    if (ifcond == NULL)
        return NULL;
    l_cond = strlen(ifcond);

    body = tree->child->sibling->sibling;
    ifbody = cgen_IfBody(body, indent + INDENT_LEV);
    if (ifbody == NULL) {
        free(ifcond);
        return NULL;
    }
    l_body = strlen(ifbody);

    // Additional spaces 'if ' ... ':\n' ... '/0'
    result = calloc(indent + l_cond + l_body + 6, sizeof(char));
    if (! result) {
        free(ifbody);
        free(ifcond);
        return NULL;
    }
    if (indent > 0)
        memset(result, ' ', indent * sizeof(char));
    last = indent;
    result[last++] = 'i';
    result[last++] = 'f';
    result[last++] = ' ';
    memcpy(result + last, ifcond, l_cond * sizeof(char));
    last += l_cond;
    result[last++] = ':';
    result[last++] = '\n';
    memcpy(result + last, ifbody, l_body * sizeof(char));

    free(ifcond);
    free(ifbody);
    return result;
}


char* cgen_IfBody (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != IfBody)
        return NULL;

    char *result, *line;
    int l_line, nLines, nElseLines, total_if, total_else, last;
    struct ParseTree *if_p, *else_p;

    line = NULL;
    l_line = total_if = total_else = 0;
    if_p = else_p = NULL;
    nLines = nElseLines = 0;

    if_p = tree->child; // 1st line
    while (if_p != NULL && if_p->data->type != OptElse) {
        nLines++;
        if_p = if_p->sibling->sibling;
    }

    char* iflines[nLines];
    nLines = 0;
    if_p = tree->child; // start over
    while (if_p != NULL && if_p->data->type != OptElse) {
        iflines[nLines] = cgen_Line(if_p, indent);
        if (iflines[nLines] == NULL)
            return _bail_out_Str(iflines, nLines);
        total_if += strlen(iflines[nLines]);
        nLines++;
        if_p = if_p->sibling->sibling;
    }

    // else part
    if (if_p!= NULL && if_p->data->type == OptElse) {
        else_p = if_p->child->sibling; // 1st else line
    }
    while (else_p != NULL) {
        nElseLines++;
        else_p = else_p->sibling->sibling;
    }

    char* elselines[nElseLines];
    if (nElseLines > 0){
        nElseLines = 0;
        else_p = if_p->child->sibling; // to start over
    }
    while (else_p != NULL) {
        elselines[nElseLines] = cgen_Line(else_p, indent);
        if (elselines[nElseLines] == NULL) {
            _bail_out_Str(elselines, nElseLines);
            return _bail_out_Str(iflines, nLines);
        }
        total_else += strlen(elselines[nElseLines]);
        nElseLines++;
        else_p = else_p->sibling->sibling;
    }

    if (nElseLines > 0)
        result = calloc(total_if + indent - INDENT_LEV + 6 + total_else + 1, sizeof(char));
    else
        result = calloc(total_if + 1, sizeof(char));

    if (! result){
        _bail_out_Str(elselines, nElseLines);
        return _bail_out_Str(iflines, nLines);
    }

    last = 0;
    for (int i=0; i<nLines; i++) {
        l_line = strlen(iflines[i]);
        memcpy(result + last, iflines[i], l_line * sizeof(char));
        free(iflines[i]);
        last += l_line;
    }
    if (nElseLines > 0) {
        memset(result + last, ' ', (indent - INDENT_LEV) * sizeof(char));
        last = last + indent - INDENT_LEV;
        memcpy(result + last, "else:\n", 6 * sizeof(char));
        last += 6;
    }
    for (int i=0; i<nElseLines; i++) {
        l_line = strlen(elselines[i]);
        memcpy(result + last, elselines[i], l_line * sizeof(char));
        free(elselines[i]);
        last += l_line;
    }
    return result;
}


char* cgen_Break (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Break)
        return NULL;

    char *result;

    result = calloc(indent + 6, sizeof(char));
    if (! result)
        return NULL;

    if (indent > 0)
        memset(result, ' ', indent * sizeof(char));
    memcpy(result + indent, "break", 5 * sizeof(char));
    return result;
}


char* cgen_Continue (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Continue)
        return NULL;

    char *result;

    result = calloc(indent + 9, sizeof(char));
    if (! result)
        return NULL;

    if (indent > 0)
        memset(result, ' ', indent * sizeof(char));
    memcpy(result + indent, "continue", 8 * sizeof(char));
    return result;
}


char* cgen_Program (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != Program)
        return NULL;

    char *result;
    int nLines, total, last, l_line;
    struct ParseTree *tmp;

    result = NULL;
    nLines = last = total = l_line = 0;
    tmp = tree->child;

    while (tmp != NULL) {
        nLines++;
        tmp = tmp->sibling->sibling;
    }
    if (nLines == 0)
        return result;

    char* lines[nLines];
    nLines = 0;
    tmp = tree->child;
    while (tmp != NULL) {
        lines[nLines] = cgen_Line(tmp, indent);
        printf("line is %s\n", lines[nLines]);
        if (lines[nLines] == NULL)
            return _bail_out_Str(lines, nLines);
        total += strlen(lines[nLines]);
        nLines++;
        tmp = tmp->sibling->sibling;
    }

    result = calloc(total + 1, sizeof(char));
    if (! result)
        return _bail_out_Str(lines, nLines);
    for (int i=0; i<nLines; i++) {
        l_line = strlen(lines[i]);
        memcpy(result + last, lines[i], l_line * sizeof(char));
        last += l_line;
        free(lines[i]);
    }
    return result;
}


char* cgen_LoopLine (struct ParseTree* tree, int indent) {
    if (! tree || tree->data->type != LoopLine)
        return NULL;

    char *result, *prog, *expr;
    struct ParseTree *cond, *body;
    int l_expr, l_prog;

    l_expr = l_prog = 0;
    result = prog = expr = NULL;

    cond = tree->child->sibling->child->sibling;
    expr = cgen_Expr(cond);
    if (expr == NULL)
        return NULL;
    l_expr = strlen(expr);

    body = tree->child->sibling->sibling;
    prog = cgen_Program(body, indent + INDENT_LEV);
    if (prog == NULL) {
        free(expr);
        return NULL;
    }
    l_prog = strlen(prog);

    result = calloc(indent + 6 + l_expr + 2 + l_prog + 1, sizeof(char));
    memset(result, ' ', indent * sizeof(char));
    memcpy(result + indent, "while ", 6 * sizeof(char));
    memcpy(result + indent + 6, expr, l_expr * sizeof(char));
    result[indent+6+l_expr] = ':';
    result[indent+6+l_expr+1] = '\n';
    memcpy(result + indent + 6 + l_expr + 2, prog, l_prog * sizeof(char));

    free(expr);
    free(prog);
    return result;
}


/*
void main() {

    int res;
    int l = 10;
    size_t len;
    char *s1 = calloc(l, sizeof(char));
    char w[8] = "1234567\0";
    memcpy(s1, w, 8);
    len = strlen(s1);
    printf("Old L %d\n", l);
    printf("Old len is %zu\n", len);
    printf("Old S1 |%s|\n", s1);

    char s2[5] = "abcd\0";

    res = str_insert(&s1, s2, 10);

    len = strlen(s1);
    printf("Res %d\n", res);
    printf("New L %d\n", l);
    printf("New len is %zu\n", len);
    printf("New S1 |%s|\n", s1);
    free(s1);
    return;
  //  cgen_Program(NULL);
}
*/
