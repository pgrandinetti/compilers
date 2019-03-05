#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "lexer.h"

struct ParseTree {
    struct Token* data;
    struct ParseTree* child;
    struct ParseTree* sibling;
};

/*
 * Create a new ParseTree from a Token* structure.
 * A copy of the input Token* is created in memory.
*/
struct ParseTree* new_ParseTree(struct Token* c) {
    struct ParseTree* tree;
    tree = malloc(sizeof(struct ParseTree));
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

void print_ParseTree(struct ParseTree* tree, int indent) {
    // This is a Depth-First print
    if (tree == NULL)
        return;
    if (indent > 0 ) {
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
    print_ParseTree(child, indent + 2);
    print_ParseTree(sibling, indent);
}

/*
 * Free memory for the ParseTree and all nested structure
 * (Token* and char* within the Token)
*/
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

void skip_WS(struct TokenList** list) {
    struct TokenList* current = *list;
    while (current->token->type == WS)
        current = current->next;
    *list = current;
}

struct ParseTree* is_endline(struct TokenList** tok) {
    struct TokenList* current = *tok;
    if (current->token->type != Endline) {
        printf(">>>>>>>>>>>>>> ");
        print_Token(current->token);
        return NULL;
    }
    // Endline is never a child
    struct ParseTree* new = new_ParseTree(current->token);
    current = current->next;
    *tok = current;
    return new;
}


struct ParseTree* is_Var(struct TokenList** tok) {
    struct TokenList* current = *tok;
    skip_WS(&current);
    if(current->token->type != Var)
        return NULL;
    struct ParseTree* new = new_ParseTree(current->token);
    *tok = current->next;
    return new;
}

struct ParseTree* is_readIn(struct TokenList** tok) {
    struct TokenList* current = *tok;
    skip_WS(&current);
    if(current->token->type != Keyword ||
       strcmp(current->token->lexeme, "readIn") != 0)
        return NULL;
    struct ParseTree* new = new_ParseTree(current->token);
    *tok = current->next;
    return new;
}

/*
int is_assign(struct TokenList* tok, struct ParseTree* tree) {
    return is_var(tok, tree) && is_equal(tok, tree) && is_expr(tok, tree);
}
*/


struct ParseTree* is_input(struct TokenList** tok) {
    // Create root of the subtree
    struct Token* newTok = new_Token((char[1]){'\0'}, Input);
    if (newTok == NULL)
        return NULL;
    struct ParseTree* new = new_ParseTree(newTok);
    if (newTok == NULL) {
        free_Token(newTok);
        return NULL;
    }
    free_Token(newTok);
    // Create subtree
    struct ParseTree* readin = is_readIn(tok);
    if (readin == NULL) {
        free_ParseTree(new);
        return NULL;
    }
    struct ParseTree* var = is_Var(tok);
    if (var == NULL) {
        free_ParseTree(new);
        free_ParseTree(readin);
        return NULL;
    }
    // Link it
    readin->sibling = var;
    new->child = readin;
    return new;
}


struct ParseTree* is_output(struct TokenList** tok) {
    struct TokenList* curr = *tok;
    curr = curr->next->next;
    *tok = curr;
    return NULL;
}


struct ParseTree* is_line(struct TokenList** tok) {
    // Create root of the subtree
    struct Token* newTok = new_Token((char[1]){'\0'}, Line);
    if (newTok == NULL)
        return NULL;
    struct ParseTree* new = new_ParseTree(newTok);
    free_Token(newTok);
    if (new == NULL)
        return NULL;
    // Create subtree
    struct TokenList* state = *tok;
    struct ParseTree* subtree;
    if ((subtree = is_output(tok)) == NULL) {
        *tok = state;
        subtree = is_input(tok);
    }
    if (subtree != NULL)
        new->child = subtree;
    else {
        free_ParseTree(new);
        new = NULL;
    }
    return new;
}


struct ParseTree* is_program(struct TokenList* head) {
    // Entry point of any valid tokens sequence for this grammar.
    int status; // boolean
    struct ParseTree* tree; // Used in nested function
    struct ParseTree* ret; // return value
    struct ParseTree* subtree; // incremental subtree

    struct Token* tok; // the root token (program)
    tok = new_Token((char[1]){'\0'}, Program);
    if (tok == NULL)
        return NULL;
    tree = new_ParseTree(tok);
    if (tree == NULL) {
        free(tok);
        return NULL;
    }
    free_Token(tok);
    ret = tree; // save pointer to root
    /*
     * Nested call to identify grammar tokens return boolean value
     * true (1) if the token was matched
     * false (0) otherwise, that is a parsing error
    */
    // A program is a (possibly empty) sequence of 'line' 'endline'
    int child = 1; // the first line is always a child of program
    while (head != NULL) {
        // Match Line
        printf("PROGR >>>>>>>>>>>> %p\n", head);
    print_Token(head->token);
        subtree = is_line(&head);
        if (subtree == NULL)
            break;
        if (child) {
            tree->child = subtree;
            child = !child;
        }
        else
            tree->sibling = subtree;
        tree = subtree;

        // Match Endline
        printf("PROGR >>>>>>>>>>>> %p\n", head);
    print_Token(head->token);
        subtree = is_endline(&head);
        if (subtree == NULL) {
            printf("Missing ENDLINE\n");
            return NULL;
        }
        tree->sibling = subtree;
        tree = subtree;
    }
    return ret;
}


struct ParseTree* build_ParseTree (struct TokenList* head) {
    // Always match the start symbol of this grammar: program.
    return is_program(head);
}

int main(int argc, char* argv[]) {
    /*
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

    char c4[3] = {'h', 'i', '\0'};
    struct Token* t4 = new_Token(c4, UNK);
    struct ParseTree* p4 = new_ParseTree(t4);

    p2->child = p4;

    print_ParseTree(p1, 0);

    free_ParseTree(p1);
    */
    if (argc < 2) {
        printf("Expecting exactly 1 argument: file path.\n");
        return 1;
    }
    char const* const fileName = argv[1];

    /*
     * Load the file in memory and return a char* to it
    */
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Cannot read the given file path.\n");
        return -1;
    }
    struct stat buffer;
    int status = stat(fileName, &buffer);
    char vec[buffer.st_size + 1];
    size_t read = fread(vec, sizeof(char), buffer.st_size, file);
    fclose(file);
    vec[buffer.st_size] = '\0';
    const char* fp = vec;

    /*
     * Build Token list
    */
    // Will also print total count
    struct TokenList* list = build_TokenList(fp);
    print_TokenList(list);

    struct ParseTree* tree = build_ParseTree(list);
    print_ParseTree(tree, 0);
    free_ParseTree(tree);
    free_TokenList(list);
}
