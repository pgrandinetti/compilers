#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "parser.h"


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
    print_ParseTree(child, indent + 2);
    print_ParseTree(sibling, indent);
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


void skip_WS(struct TokenList** list) {
    struct TokenList* current = *list;
    while (current != NULL && current->token->type == WS)
        current = current->next;
    *list = current;
}


struct ParseTree* is_endline(struct TokenList** tok) {
    if (*tok == NULL)
        return NULL;
    struct TokenList* current = *tok;
    if (current->token->type != Endline)
        return NULL;
    struct ParseTree* new = new_ParseTree(current->token);
    current = current->next;
    *tok = current;
    return new;
}


struct ParseTree* is_Int(struct TokenList** tok) {
    if (*tok == NULL)
        return NULL;
    struct TokenList* current = *tok;
    if (current->token->type != Int)
        return NULL;
    struct ParseTree* new = new_ParseTree(current->token);
    current = current->next;
    *tok = current;
    return new;
}

struct ParseTree* is_Dot(struct TokenList** tok) {
    if (*tok == NULL)
        return NULL;
    struct TokenList* current = *tok;
    if (current->token->type != Dot)
        return NULL;
    struct ParseTree* new = new_ParseTree(current->token);
    current = current->next;
    *tok = current;
    return new;
}


struct ParseTree* is_OptFrac(struct TokenList** tok) {
    if (*tok == NULL)
        return NULL;
    struct TokenList *state = *tok;
    struct ParseTree* new, *integer;
    new = is_Dot(tok);
    if (new != NULL) {
        integer = is_Int(tok);
        if (integer == NULL) {
            free_ParseTree(new);
            return NULL; // error
        }
        else
            new->sibling = integer;
    }
    else
        *tok = state;
    return new;
}


struct ParseTree* is_Float(struct TokenList** tok) {
    if (*tok == NULL)
        return NULL;
    struct Token* newTok = new_Token((char[1]){'\0'}, Float);
    if (newTok == NULL)
        return NULL;
    struct ParseTree* new = new_ParseTree(newTok);
    free_Token(newTok);
    if (new == NULL)
        return NULL;
    struct TokenList* state_before_Int, *state_before_Frac;
    struct ParseTree* subtree, *optFrac, *optExp;
    state_before_Int = state_before_Frac = *tok;
    subtree = is_Int(tok);
    if (subtree != NULL) {
        new->child = subtree;
        state_before_Frac = *tok;
        optFrac = is_OptFrac(tok);
        if (optFrac != NULL) {
            subtree->sibling = optFrac;
            optExp = isOptExp(tok);
            optFrac->sibling = optExp; // can be NULL
            if (optExp == NULL)
                *tok = state_before_Frac;
        }
        else {
            *tok = state_before_Frac;
            optExp = isOptExp(tok);
            subtree->sibling = optExp; // can be NULL
            if (optExp == NULL)
                *tok = state_before_Frac;
        }
    }
    else {
        *tok = state_before_Int;
        optFrac = is_OptFrac(tok);
        if (optFrac != NULL) {
            new->child = optFrac;
            optExp = isOptExp(tok);
            optFrac->sibling = optExp; // can be NULL
            if (optExp == NULL)
                *tok = state_before_Frac;
        }
        else {
            *tok = state_before_Frac;
            optExp = isOptExp(tok);
            new->child = optExp; // can be NULL
            if (optExp == NULL)
                *tok = state_before_Frac;
        }
    }
    if (new == NULL)
        free_ParseTree(new);
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


struct ParseTree* is_writeOut(struct TokenList** tok) {
    struct TokenList* current = *tok;
    skip_WS(&current);
    if(current->token->type != Keyword ||
       strcmp(current->token->lexeme, "writeOut") != 0)
        return NULL;
    struct ParseTree* new = new_ParseTree(current->token);
    *tok = current->next;
    return new;
}


struct ParseTree* is_Obj(struct TokenList** tok) {
    return NULL;
/*
    // Create root of the subtree
    struct Token* newTok = new_Token((char[1]){'\0'}, Obj);
    if (newTok == NULL)
        return NULL;
    struct ParseTree* new = new_ParseTree(newTok);
    free_Token(newTok);
    if (new == NULL)
        return NULL;
    // Create subtree
    struct TokenList* current, *state;
    current = *tok;
    state = current;
    struct ParseTree* subtree;
    if ( (subtree = is_Base(tok)) != NULL )
        new->child = subtree;
    else {
        *tok = state;
        if ( (subtree = is_Var(tok)) != NULL )
            new->child = subtree;
        else {
            *tok = state;
            if ( (subtree = is_List(tok)) != NULL )
            new->child = subtree;
            else {
                *tok = state;
                if ( (subtree = is_ListElem(tok)) != NULL )
                    new->child = subtree;
            }
        }
    }
    if (new->child == NULL) {
        // Did not manage to find it
        free_ParseTree(new);
        return NULL;
    }
    return new;
*/
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
    free_Token(newTok);
    if (new == NULL)
        return NULL;
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
    // Create root of the subtree
    struct Token* newTok = new_Token((char[1]){'\0'}, Output);
    if (newTok == NULL)
        return NULL;
    struct ParseTree* new = new_ParseTree(newTok);
    free_Token(newTok);
    if (new == NULL)
        return NULL;
    // Create subtree
    struct ParseTree* writeOut = is_writeOut(tok);
    if (writeOut == NULL) {
        free_ParseTree(new);
        return NULL;
    }
    struct ParseTree* obj = is_Obj(tok);
    if (obj == NULL) {
        free_ParseTree(new);
        free_ParseTree(writeOut);
        return NULL;
    }
    // Link it
    writeOut->sibling = obj;
    new->child = writeOut;
    return new;
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
    free_Token(tok);
    if (tree == NULL)
        return NULL;

    ret = tree; // save pointer to root

    /*
     * Nested call to identify grammar tokens return pointer to subtree(s)
     * or NULL is parsing/memory error happened.
    */
    
    // Start by skipping all white spaces
    skip_WS(&head);

    int child = 1; // true - the first Line is always a child of Program

    // A program is a (possibly empty) sequence of 'line' 'endline'
    while (head != NULL) {
        // Match Line
        subtree = is_line(&head);
        if (subtree == NULL) // TODO can be NULL for good exit or for error
            break;
        if (child) {
            tree->child = subtree;
            child = !child;
        }
        else
            tree->sibling = subtree;
        tree = subtree;

        skip_WS(&head);

        // Match Endline
        subtree = is_endline(&head);
        if (subtree == NULL) {
            printf("Missing ENDLINE\n");
            free_ParseTree(ret);
            return NULL;
        }
        tree->sibling = subtree;
        tree = subtree;

        skip_WS(&head);
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
    free_TokenList(list);
    if (tree != NULL) {
        print_ParseTree(tree, 0);
        free_ParseTree(tree);
    }
    else
        printf("PARSING ERROR\n");
}
