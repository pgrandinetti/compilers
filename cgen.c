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
    char* new;
    int l1, l2;

    l1 = strlen(*s1);

    if (pos > l1 || pos < 0)
        return -1;

    l2 = strlen(s2);

    new = calloc(l1 + l2 + 1, sizeof(char));
    if (!new)
        return -1;

    memcpy(new + pos, s2, l2 * sizeof(char));
    memcpy(new + pos + l2, *s1 + pos, (l1 - pos) * sizeof(char));
    memcpy(new, *s1, pos * sizeof(char));

    free(*s1);
    *s1 = new;
    return 0;
}


char* cgen_Int (struct ParseTree* tree) {

    char* num = calloc(4, sizeof(char));
    if (!num)
        return NULL;
    memcpy(num, (char[4]){'5', '4', '3', '\0'}, 4*sizeof(char));
    return num;
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


void main() {
/*
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
*/
    cgen_Program(NULL);
}
