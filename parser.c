#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "parser.h"


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
 * Each function `is_<TokenType>` is defined as

 * int is_<TokenType> (struct TokenList** tok, struct ParseTree** new)

 * - **tok points to the current token in the sequence. The function moves it ahead CHANGING the pointer location.
 * - **new points to a already-ALLOCATED ParseTree. It will contain the resulting subtree, or stay empty.

 * Therefore:
 * - The caller must alloc the ParseTree** new
 * - The called must assign *new->data, child and sibling

 * Return a integer flag with the operation final status (see #define below).
*/

#define PARSING_ERROR -1
#define SUBTREE_OK 0
#define MEMORY_ERROR 1


void skip_WS(struct TokenList** list) {
    // jump ahead of all whitespaces
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
 *
 * First, all WS are skipped. Then, if the check fails *tok unchanged. Otherwise it's advanced.
*/
int _single_Token_template(struct TokenList** tok, struct ParseTree** new, enum TokenType type, char* lexeme) {
    if (*tok == NULL)
        return PARSING_ERROR;
    skip_WS(tok);
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


int is_QuotedStr (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, QuotedStr, NULL);
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


int is_While (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, While, "while");
}


int is_Break (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Break, "break");
}


int is_Continue (struct TokenList** tok, struct ParseTree** new) {
    return _single_Token_template(tok, new, Continue, "Continue");
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


int is_List (struct TokenList** tok, struct ParseTree** new) {
    return PARSING_ERROR;
}


int is_Str (struct TokenList** tok, struct ParseTree** new) {
    return PARSING_ERROR;
}


int is_Frac (struct TokenList** tok, struct ParseTree** new) {
    return PARSING_ERROR;
}


int is_Exp (struct TokenList** tok, struct ParseTree** new) {
    int status, has_sign;
    struct ParseTree *pow, *sign, *integer;
    struct Token* newTok;

    status = SUBTREE_OK;
    has_sign = 0;
    pow = alloc_ParseTree();
    sign = alloc_ParseTree();
    integer = alloc_ParseTree();

    status = is_Pow(tok, &pow);
    if (status != SUBTREE_OK) {
        free_ParseTree(pow);
        return status;
    }

    if ((*tok)->token->type == Plus) {
        status = is_Plus(tok, &sign);
        has_sign = 1;
    }
    else if ((*tok)->token->type == Minus) {
        status = is_Minus(tok, &sign);
        has_sign = 1;
    }
    if (has_sign && (status != SUBTREE_OK)) {
        free_ParseTree(pow);
        free_ParseTree(sign);
        return status;
    }

    status = is_Int(tok, &integer);
    if (status != SUBTREE_OK) {
        free_ParseTree(pow);
        free_ParseTree(sign);
        free_ParseTree(integer);
        return status;
    }

    newTok = new_Token((char[1]){'\0'}, Pow);
    if (newTok == NULL) {
        free_ParseTree(pow);
        free_ParseTree(sign);
        free_ParseTree(integer);
        return MEMORY_ERROR;
    }
    (*new)->data = newTok;
    (*new)->child = pow;
    if (has_sign) {
        (*new)->child->sibling = sign;
        sign->sibling = integer;
    }
    else
        (*new)->child->sibling = integer;
    return status;
}


int is_Float(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *integer, *frac, *pow;
    int status;
    int has_int, has_frac, has_pow;
    struct Token* newTok;

    newTok = new_Token((char[1]){'\0'}, Float);
    if (newTok == NULL)
        return MEMORY_ERROR;

    status = SUBTREE_OK;
    has_int = has_frac = has_pow = 0;
    frac = alloc_ParseTree();
    pow = alloc_ParseTree();
    integer = alloc_ParseTree();

    if ((*tok)->token->type == Dot) {
        has_frac = 1;
        status = is_Frac(tok, &frac);
        if (status != SUBTREE_OK) {
            free_Token(newTok);
            free_ParseTree(frac);
            free_ParseTree(pow);
            free_ParseTree(integer);
            return status;
        }
        if ((*tok)->token->type == Pow) {
            has_pow = 1;
            status = is_Exp(tok, &pow);
            if (status != SUBTREE_OK) {
                free_Token(newTok);
                free_ParseTree(frac);
                free_ParseTree(pow);
                free_ParseTree(integer);
                return status;
            }
        }
    }
    else if ((*tok)->token->type == Int) {
        has_int = 1;
        status = is_Int(tok, &integer);
        if (status != SUBTREE_OK) {
            free_Token(newTok);
            free_ParseTree(frac);
            free_ParseTree(pow);
            free_ParseTree(integer);
            return status;
        }
        if ((*tok)->token->type == Dot) {
            has_frac = 1;
            status = is_Frac(tok, &frac);
            if (status != SUBTREE_OK) {
                free_Token(newTok);
                free_ParseTree(frac);
                free_ParseTree(pow);
                free_ParseTree(integer);
                return status;
            }
        }
        if ((*tok)->token->type == Pow) {
            has_pow = 1;
            status = is_Exp(tok, &pow);
            if (status != SUBTREE_OK) {
                free_Token(newTok);
                free_ParseTree(frac);
                free_ParseTree(pow);
                free_ParseTree(integer);
                return status;
            }
        }
    }
    else
        status = PARSING_ERROR;

    if (status != SUBTREE_OK) {
        free_Token(newTok);
        free_ParseTree(frac);
        free_ParseTree(pow);
        free_ParseTree(integer);
        return status;
    }

    (*new)->data = newTok;
    if (has_int)
        (*new)->child = integer;
    else // must have frac
        (*new)->child = frac;

    if (has_int && has_frac) {
        (*new)->child->sibling = frac;
        if (has_pow)
            frac->sibling = pow;
    }
    else if (has_pow)
        (*new)->child->sibling = pow;

    if (! has_int)
        free_ParseTree(integer);
    if (! has_frac)
        free_ParseTree(frac);
    if (! has_pow)
        free_ParseTree(pow);

    return status;
}


int is_Num(struct TokenList** tok, struct ParseTree** new) {
    struct ParseTree *sign, *numeric;
    int status;
    struct Token* newTok; 

    newTok = new_Token((char[1]){'\0'}, Num);
    if (newTok == NULL)
        return MEMORY_ERROR;

    status = SUBTREE_OK;
    sign = alloc_ParseTree();

    if ((*tok)->token->type == Plus)
        status = is_Plus(tok, &sign);
    else if ((*tok)->token->type == Minus)
        status = is_Minus(tok, &sign);

    if (status != SUBTREE_OK) {
        free_ParseTree(sign);
        free_Token(newTok);
        return status;
    }

    numeric = alloc_ParseTree();

    status = is_Float(tok, &numeric);
    if (status != SUBTREE_OK) {
        free_ParseTree(sign);
        free_Token(newTok);
        free_ParseTree(numeric);
    }
    else {
        (*new)->data = newTok;
        (*new)->child = sign;
        sign->sibling = numeric;
        (*new)->sibling = NULL;
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

    subtree = alloc_ParseTree();
    if (subtree == NULL){
        free_Token(newTok);
        return MEMORY_ERROR;
    }

    // use LL(1) FOLLOW Sets
    struct TokenList* curr;
    curr = *tok;
    if (curr->token->type == Var)
        status = is_Var(tok, &subtree);
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

    if (status == SUBTREE_OK) {
        (*new)->data = newTok;
        (*new)->child = subtree;
        (*new)->sibling = NULL;
    }
    else {
        free_Token(newTok);
        free_ParseTree(subtree);
    }
    return status;
}



int is_Assign(struct TokenList** tok, struct ParseTree** tree) {
    struct ParseTree *var1, *eq, *var2;
    int status;
    struct Token *newTok;

    newTok = new_Token((char[1]) {'\0'}, Assign);
    if (newTok == NULL)
        return MEMORY_ERROR;

    var1 = alloc_ParseTree();
    if (var1 == NULL)
        return MEMORY_ERROR;
    status = is_Var(tok, &var1);
    if (status != SUBTREE_OK) {
        free_ParseTree(var1);
        return PARSING_ERROR;
    }

    eq = alloc_ParseTree();
    if (eq == NULL) {
        free_ParseTree(var1);
        return MEMORY_ERROR;
    }
    status = is_Equal(tok, &eq);
    if (status != SUBTREE_OK) {
        free_ParseTree(var1);
        free_ParseTree(eq);
        return PARSING_ERROR;
    }

    var2 = alloc_ParseTree();
    if (var2 == NULL) {
        free_ParseTree(var1);
        free_ParseTree(eq);
        return MEMORY_ERROR;
    }
    status = is_Var(tok, &var2);
    if (status != SUBTREE_OK) {
        free_ParseTree(var1);
        free_ParseTree(eq);
        free_ParseTree(var2);
        return PARSING_ERROR;
    }

    var1->sibling = eq;
    eq->sibling = var2;
    (*tree)->child = var1;
    (*tree)->data = newTok;
    return SUBTREE_OK;
}



int is_Input(struct TokenList** tok, struct ParseTree** new) {
    // Create subtree
    struct ParseTree* readin, *var;
    int status;
    struct Token* newTok;

    readin = alloc_ParseTree();
    if (readin == NULL)
        return MEMORY_ERROR;
    status = is_readIn(tok, &readin);
    if (status != SUBTREE_OK) {
        free_ParseTree(readin);
        return status;
    }
    var = alloc_ParseTree();
    if (var == NULL)
        return MEMORY_ERROR;
    status = is_Var(tok, &var);
    if (status != SUBTREE_OK) {
        free_ParseTree(readin);
        free_ParseTree(var);
        return status;
    }
    readin->sibling = var;
    newTok = new_Token((char[1]){'\0'}, Input);
    if (newTok == NULL) {
        free_ParseTree(var);
        free_ParseTree(readin);
        return MEMORY_ERROR;
    }
    (*new)->data = newTok;
    (*new)->child = readin;
    (*new)->sibling = NULL;
    return SUBTREE_OK;
}


int is_Output(struct TokenList** tok, struct ParseTree** new) {
    // Create subtree
    struct ParseTree *writeOut, *obj;
    int status;
    struct Token* newTok;// = new_Token((char[1]){'\0'}, Output);

    writeOut = alloc_ParseTree();
    if (writeOut == NULL)
        return MEMORY_ERROR;
    status = is_writeOut(tok, &writeOut);
    if (status != SUBTREE_OK) {
        free_ParseTree(writeOut);
        return status;
    }
    obj = alloc_ParseTree();
    if (obj == NULL) {
        free_ParseTree(writeOut);
        return MEMORY_ERROR;
    }
    status = is_Obj(tok, &obj);
    if (status != SUBTREE_OK) {
        free_ParseTree(writeOut);
        free_ParseTree(obj);
        return status;
    }
    writeOut->sibling = obj;

    // Link it
    newTok = new_Token((char[1]){'\0'}, Output);
    if (newTok == NULL) {
        free_ParseTree(writeOut);
        free_ParseTree(obj);
        return MEMORY_ERROR;
    }
    (*new)->data = newTok;
    (*new)->child = writeOut;
    (*new)->sibling = NULL;
    return SUBTREE_OK;
}


int is_IfLine(struct TokenList** tok, struct ParseTree** line) {
    return PARSING_ERROR;
}


int is_LoopLine(struct TokenList** tok, struct ParseTree** line) {
    return PARSING_ERROR;
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

    if (status != SUBTREE_OK) {
        free_ParseTree(subtree);
        free_Token(newTok);
        return status;
    }

    (*line)->data = newTok;
    (*line)->child = subtree;
    (*line)->sibling = NULL;
    return status;
}


int is_Program(struct TokenList* head, struct ParseTree** tree) {
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
    
    // Start by skipping all white spaces
    skip_WS(&head);

    int child = 1; // true - the first Line is always a child of Program
    int status;
    // A program is a (possibly empty) sequence of 'line' 'endline'
    while (head != NULL) {
        // Match Line
        status = is_Line(&head, &line);
        if (status != SUBTREE_OK) {
            printf("ERROR PARSING LINE: %d\n", status);
            break;
        }
        if (child) {
            current->child = line;
            child = !child;
        }
        else
            current->sibling = line;
        current = line;
        line = alloc_ParseTree();

        skip_WS(&head);
        // Match Endline
        status = is_Endline(&head, &endline);
        if (status != SUBTREE_OK) {
            printf("Missing ENDLINE\n");
            break;
        }
        current->sibling = endline;
        current = endline;
        endline = alloc_ParseTree();

        skip_WS(&head);

        //print_ParseTree(*tree);fflush(stdout);
        //print_Token(head->token);fflush(stdout);
    }
    free(line);
    free(endline);

    return status;
}


struct ParseTree* build_ParseTree (struct TokenList* head) {
    struct ParseTree* tree;
    tree = alloc_ParseTree();
    // Always match the start symbol of this grammar: program.
    int status;
    status = is_Program(head, &tree);
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
    stat(fileName, &buffer);
    char vec[buffer.st_size + 1];
    fread(vec, sizeof(char), buffer.st_size, file);
    fclose(file);
    vec[buffer.st_size] = '\0';
    const char* fp = vec;

    /*
     * Build Token list
    */
    // Will also print total count
    struct TokenList* list = build_TokenList(fp);
    if (list == NULL)
        return 1;
    //print_TokenList(list);
    struct TokenList* list2 = strip_WS(list);
    free_TokenList(list);

    struct ParseTree* tree = build_ParseTree(list2);
    if (tree != NULL) {
        print_ParseTree(tree);
        free_ParseTree(tree);
    }
    else
        printf("PARSING ERROR\n");
    free_TokenList(list2);
    return 0;
}
