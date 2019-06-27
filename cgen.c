#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cgen.h"


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
    if (! tree|| tree->data->type != Num)
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


void cgen_Program (struct ParseTree* tree) {
    /*
        Insert into **code, from index pos, the new code-string.

        - **code: The current code template.
        - pos: The index at which to insert the new code and fill the template.
    */

    char template[12] = "print ("
                       " "
                       "+ "
                       ")\0";

    char* new = calloc(12, sizeof(char));
    memcpy(new, template, 12*sizeof(char));

    int idx1 = 7;
    int idx2 = 10;

    char *new1, *new2;

    new1 = cgen_Int(tree);
    str_insert (&new, new1, idx1);
    printf("|%s|\n", new);

    idx2 += strlen(new1);

    new2 = cgen_Int(tree);
    str_insert (&new, new2, idx2);
    printf("|%s|\n", new);

    free(new);
    free(new1);
    free(new2);
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
