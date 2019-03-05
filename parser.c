#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"

struct ParseTree {
    struct Token* data;
    struct ParseTree* child;
    struct ParseTree* sibling;
};

struct ParseTree* new_ParseTree(struct Token* c) {
    struct ParseTree* tree;
    tree = malloc(sizeof(struct ParseTree));
    if (tree == NULL)
        return NULL;
    tree->data = c;
    tree->child = NULL;
    tree->sibling = NULL;
    return tree;
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


struct ParseTree* build_ParseTree (struct TokenList* head) {
    // Do not modify head of token list
    struct TokenList* current = head;
}


int main() {

    char c1[4] = {'h', 'i', '!', '\0'};
    struct Token* t1 = new_Token(c1, UNK);
    struct ParseTree* p1 = new_ParseTree(t1);

    char c2[5] = {'h', 'i', '!', '!', '\0'};
    struct Token* t2 = new_Token(c2, UNK);
    struct ParseTree* p2 = new_ParseTree(t2);

    char c3[3] = {'h', 'i', '\0'};
    struct Token* t3 = new_Token(c3, UNK);
    struct ParseTree* p3 = new_ParseTree(t3);

    p1->child = p2;
    p1->sibling = p3;
    free_ParseTree(p1);
}
