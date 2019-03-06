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
 * Each function `is_<TokenType>` is defined as

 * int is_<TokenType> (struct TokenList** tok, struct ParseTree** new)

 * - tok points to the current token in the sequence. The function moves it ahead CHANGING the pointer location.
 * - new points to a already-ALLOCATED ParseTree. It will contain the resulting subtree, or stay empty.

 * Therefore:
 * - The caller must alloc the ParseTree** new
 * - The called must assign *new->data, child and sibling

 * Return a integer flag with the operation final status (see #define below).
*/

#define PARSING_ERROR -1
#define SUBTREE_OK 0
#define MEMORY_ERROR 1


void skip_WS(struct TokenList** list) {
    struct TokenList* current = *list;
    while (current != NULL && current->token->type == WS)
        current = current->next;
    *list = current;
}


/*
 * Template function for easy-to-check Tokens
 *
 * Match the current Token in the TokenList** tok, with the given TokenType type.
 * If char* lexeme != NULL, then also checks that the current Token's lexeme matches it.
*/
int _single_Token_template(struct TokenList** tok, struct ParseTree** new, enum TokenType type, char* lexeme) {
    if (*tok == NULL)
        return PARSING_ERROR;

    skip_WS(tok);
    struct TokenList* current = *tok;
    if (current->token->type != type)
        return PARSING_ERROR;
    if (lexeme != NULL)
        if(strcmp(current->token->lexeme, lexeme) != 0)
            return PARSING_ERROR;
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


int is_endline(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Endline, NULL);
}


int is_Int(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Int, NULL);
}


int is_Dot(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Dot, NULL);
}


int is_QuotedStr(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, QuotedStr, NULL);
}


int is_Var(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Var, NULL);
}


int is_readIn(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Keyword, "readIn");
}


int is_writeOut(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Keyword, "writeOut");
}


int is_If(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Keyword, "if");
}


int is_While(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Keyword, "while");
}


int is_Break(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Keyword, "break");
}


int is_Continue(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Keyword, "Continue");
}


int is_Lbrack(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Lbrack, NULL);
}


int is_Rbrack(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Rbrack, NULL);
}


int is_Lpar(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Lpar, NULL);
}


int is_Rpar(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Rpar, NULL);
}


int is_Comma(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Comma, NULL);
}


int is_EqEq(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, EqEq, NULL);
}


int is_Equal(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Equal, NULL);
}


int is_Plus(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Plus, NULL);
}


int is_Minus(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Minus, NULL);
}


int is_NotEq(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, NotEq, NULL);
}


int is_Greater(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Greater, NULL);
}


int is_GreaterEq(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, GreaterEq, NULL);
}


int is_Lesser(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Lesser, NULL);
}


int is_LesserEq(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, LesserEq, NULL);
}


int is_Star(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Star, NULL);
}


int is_Div(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Div, NULL);
}


int is_FloatDiv(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, FloatDiv, NULL);
}


int is_Percent(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Percent, NULL);
}


int is_Or(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Or, NULL);
}


int is_And(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, And, NULL);
}


int is_Pow(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Pow, NULL);
}


int is_Null(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Null, NULL);
}


int is_Bool(struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Bool, NULL);
}


/*
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
*/

int is_Obj(struct TokenList** tok, struct ParseTree** new) {
    return -1;
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


int is_input(struct TokenList** tok, struct ParseTree** new) {
    // Create subtree
    struct ParseTree* readin, *var;
    int status;
    readin = malloc(sizeof(struct ParseTree));
    var = malloc(sizeof(struct ParseTree));
    if (readin == NULL || var == NULL)
        return MEMORY_ERROR;
    status = is_readIn(tok, &readin);
    if (status != SUBTREE_OK)
        return status;
    status = is_Var(tok, &var);
    if (status != SUBTREE_OK)
        return status;
    readin->sibling = var;
    struct Token* newTok;
    newTok = new_Token((char[1]){'\0'}, Input);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*new)->data = newTok;
    (*new)->child = readin;
    (*new)->sibling = NULL;
    return SUBTREE_OK;
}


int is_output(struct TokenList** tok, struct ParseTree** new) {
    return 1;
/*
    // Create root of the subtree
    struct Token* newTok = new_Token((char[1]){'\0'}, Output);
    if (newTok == NULL)
        return -1;
    struct ParseTree* new = new_ParseTree(newTok);
    free_Token(newTok);
    if (new == NULL)
        return -1;
    // Create subtree
    struct ParseTree* writeOut;
    is_writeOut(tok, &writeOut);
    if (writeOut == NULL) {
        free_ParseTree(new);
        return -1;
    }
    struct ParseTree* obj = NULL;
    is_Obj(tok, &obj);
    if (obj == NULL) {
        free_ParseTree(new);
        free_ParseTree(writeOut);
        return -1;
    }
    // Link it
    writeOut->sibling = obj;
    new->child = writeOut;
    return new;
*/
}


int is_line(struct TokenList** tok, struct ParseTree** line) {
    // Create subtree
    struct TokenList* state = *tok;
    struct ParseTree* subtree;
    int status;
    subtree = malloc(sizeof(struct ParseTree));
    if (subtree == NULL)
        return MEMORY_ERROR;
    if ((status = is_output(tok, &subtree)) != SUBTREE_OK) {
        //*tok = state;
        status = is_input(tok, &subtree);
    }
    struct Token* newTok;
    newTok = new_Token((char[1]){'\0'}, Line);
    if (newTok == NULL)
        return MEMORY_ERROR;
    (*line)->data = newTok;
    (*line)->child = subtree;
    (*line)->sibling = NULL;
    return status;
}


int is_program(struct TokenList* head, struct ParseTree** tree) {
    struct Token* tok;
    tok = new_Token((char[1]){'\0'}, Program);
    if (tok == NULL)
        return MEMORY_ERROR;
    (*tree)->data = tok;
    (*tree)->child = NULL;
    (*tree)->sibling = NULL;

    // Entry point of any valid tokens sequence for this grammar.
    struct ParseTree* current;
    struct ParseTree* line, *endline;
    line = malloc(sizeof(struct ParseTree));
    endline = malloc(sizeof(struct ParseTree));
    if (line == NULL || endline == NULL)
        return MEMORY_ERROR;

    current = *tree; // save pointer to root

    /*
     * Nested call to identify grammar tokens return pointer to subtree(s)
     * or NULL is parsing/memory error happened.
    */
    
    // Start by skipping all white spaces
    skip_WS(&head);

    int child = 1; // true - the first Line is always a child of Program
    int status;
    // A program is a (possibly empty) sequence of 'line' 'endline'
    while (head != NULL) {
        // Match Line
        status = is_line(&head, &line);
        if (status != SUBTREE_OK) {
            printf("ERROR PARSING LINE: %d\n", status);
            return status;
        }
        if (child) {
            current->child = line;
            child = !child;
        }
        else
            current->sibling = line;
        current = line;
        line = malloc(sizeof(struct ParseTree));

        skip_WS(&head);
        // Match Endline
        status = is_endline(&head, &endline);
        if (status != SUBTREE_OK) {
            printf("Missing ENDLINE\n");
            return status;
        }
        current->sibling = endline;
        current = endline;
        endline = malloc(sizeof(struct ParseTree));

        skip_WS(&head);
    }
    free(line);
    free(endline);

    return SUBTREE_OK;
}


struct ParseTree* build_ParseTree (struct TokenList* head) {
    struct ParseTree* tree;
    tree = malloc(sizeof(struct ParseTree));
    // Always match the start symbol of this grammar: program.
    int status;
    status = is_program(head, &tree);
    if (status != SUBTREE_OK) {
        free_ParseTree(tree);
        tree = NULL;
    }
    return tree;
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
    if (tree != NULL) {
        print_ParseTree(tree);
        free_ParseTree(tree);
    }
    else
        printf("PARSING ERROR\n");
    free_TokenList(list);
}
