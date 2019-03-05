#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "lexer.h"

#define MAX_LINE 100

const char* type2char (enum TokenType t) {
    switch (t)
    {
        case Comma: return "Comma";
        case Lpar: return "Lpar";
        case Rpar: return "Rpar";
        case Lbrack: return "Lbrack";
        case Rbrack: return "Rbrack";
        case Lcurly: return "Lcurly";
        case Rcurly: return "Rcurly";
        case Plus: return "Plus";
        case Minus: return "Minus";
        case Equal: return "Equal";
        case EqEq: return "EqEq";
        //case Exclam: return "Exclam";
        case NotEq: return "NotEq";
        //case Question: return "Question";
        case Lesser: return "Lesser";
        case Greater: return "Greater";
        case LesserEq: return "LesserEq";
        case GreaterEq: return "GreaterEq";
        case Star: return "Star";
        case Div: return "Div";
        case FloatDiv: return "FloatDiv";
        case Percent: return "Percent";
        case Or: return "Or";
        case And: return "And";
        case Int: return "Int";
        case Float: return "Float";
        case Pow: return "Pow";
        case Dot: return "Dot";
        case QuotedStr: return "QuotedStr";
        case Bool: return "Bool";
        case Null: return "Null";
        case Var: return "Var";
        case Endline: return "Endline";
        case Keyword: return "Keyword";
        case WS: return "WS";
        default: return "UNK";
    }
}

void free_Token(struct Token* tok) {
    free(tok->lexeme);
    free(tok);
}

void free_TokenList(struct TokenList* tok) {
    struct TokenList* current;
    while ((current = tok) != NULL) {
        tok = tok->next;
        free_Token(current->token);
        free(current);
    }
}

int alloc_failed(struct Token* tok, char* tmp) {
    printf("FAILED TO (RE)ALLOC");
    free_Token(tok);
    free(tmp);
    return -1;
}

void print_Token(struct Token* p) {
    printf("<%s, %s>\n", p->lexeme, type2char(p->type));
}

void print_TokenList(struct TokenList* p) {
    int i = 0;
    struct TokenList* current = p;
    while (current != NULL) {
        i++;
        print_Token(current->token);
        current = current->next;
    }
    printf("====================\n");
    printf("Found %d Tokens\n", i);
    printf("====================\n");
}

char consume(const char** p) {
    // Return the current char while advancing the pointer
    char c = **p;
    (*p)++;
    return c;
}

int match_ws(const char** p, struct Token* tok) {
    // Build a Token with all consecutive blanks it can find
    if ( **p != ' ' &&
         **p != '\t' &&
         **p != '\r' &&
         **p != '\n')
    {
        printf("Bad call to whitespace!!");
        return 1;
    }
    int i = 0;
    char c;
    char* tmp;
    char ws[MAX_LINE] = "";
    do {
        if (i >= MAX_LINE - 1) {
            printf("Found more than %d white spaces\n", MAX_LINE);
            return 1;
        }
        c = consume(p); // p is incremented here
        ws[i++] = c;
    }
    while (**p == ' ' || **p == '\t' || **p == '\r' || **p == '\n');
    ws[i++] = '\0';
    tmp = realloc(tok->lexeme, i * sizeof(char));
    if (tmp == NULL)
        return alloc_failed(tok, tmp);
    else
        tok->lexeme = tmp;
    memcpy(tok->lexeme, ws, i);
    tok->type = WS;
    return 0;
}

int match_template(const char** p, struct Token* tok, char c, enum TokenType type) {
    // Build a Token with the character in input and advances the stream pointer.
    if (**p != c) {
        printf("Bad call to |%c| !!\n", c);
        return 1;
    }
    char* tmp;
    consume(p);
    tmp = realloc(tok->lexeme, 2 * sizeof(char));
    if (tmp == NULL)
        return alloc_failed(tok, tmp);
    else
        tok->lexeme = tmp;
    memcpy(tok->lexeme, (char[2]) {c, '\0'}, 2);
    tok->type = type;
    return 0;
}

int match_lbrack(const char** p, struct Token* tok) {
    return match_template(p, tok, '[', Lbrack);
}

int match_rbrack(const char** p, struct Token* tok) {
    return match_template(p, tok, ']', Rbrack);
}

int match_plus(const char** p, struct Token* tok) {
    return match_template(p, tok, '+', Plus);
}

int match_comma(const char** p, struct Token* tok) {
    return match_template(p, tok, ',', Comma);
}

int match_minus(const char** p, struct Token* tok) {
    return match_template(p, tok, '-', Minus);
}

int match_lpar(const char** p, struct Token* tok) {
    return match_template(p, tok, '(', Lpar);
}

int match_rpar(const char** p, struct Token* tok) {
    return match_template(p, tok, ')', Rpar);
}

int match_lcurly(const char** p, struct Token* tok) {
    return match_template(p, tok, '{', Lcurly);
}

int match_rcurly(const char** p, struct Token* tok) {
    return match_template(p, tok, '}', Rcurly);
}

int match_star(const char** p, struct Token* tok) {
    return match_template(p, tok, '*', Star);
}

int match_percent(const char** p, struct Token* tok) {
    return match_template(p, tok, '%', Percent);
}

int match_pow(const char** p, struct Token* tok) {
    return match_template(p, tok, '^', Pow);
}

int match_dot(const char** p, struct Token* tok) {
    return match_template(p, tok, '.', Dot);
}

int match_equal(const char** p, struct Token* tok) {
    if (**p != '=') {
        printf("Bad call to = !\n");
        return 1;
    }
    char* tmp;
    // Now use lookahead for one more equal
    consume(p);
    if (**p == '=') {
        consume(p);
        tmp = realloc(tok->lexeme, 3 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[3]) {'=', '=', '\0'}, 3 * sizeof(char));
        tok->type = EqEq;
    }
    else {
        tmp = realloc(tok->lexeme, 2 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[2]) {'=', '\0'}, 2 * sizeof(char));
        tok->type = Equal;
    }
    return 0;
}

int match_lesser(const char** p, struct Token* tok) {
    if (**p != '<') {
        printf("Bad call to < !\n");
        return 1;
    }
    char* tmp;
    consume(p);
    if (**p == '=') {
        consume(p);
        tmp = realloc(tok->lexeme, 3 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[3]) {'<', '=', '\0'}, 3 * sizeof(char));
        tok->type = LesserEq;
    }
    else {
        tmp = realloc(tok->lexeme, 2 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[2]) {'<', '\0'}, 2 * sizeof(char));
        tok->type = Lesser;
    }
    return 0;
}

int match_greater(const char** p, struct Token* tok) {
    if (**p != '>') {
        printf("Bad call to > !\n");
        return 1;
    }
    char* tmp;
    consume(p);
    if (**p == '=') {
        consume(p);
        tmp = realloc(tok->lexeme, 3 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[3]) {'>', '=', '\0'}, 3 * sizeof(char));
        tok->type = GreaterEq;
    }
    else {
        tmp = realloc(tok->lexeme, 2 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[2]) {'>', '\0'}, 2 * sizeof(char));
        tok->type = Greater;
    }
    return 0;
}

int match_div(const char** p, struct Token* tok) {
    if (**p != '/') {
        printf("Bad call to / !\n");
        return 1;
    }
    char* tmp;
    consume(p);
    if (**p == '.') {
        consume(p);
        tmp = realloc(tok->lexeme, 3 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[3]) {'/', '.', '\0'}, 3 * sizeof(char));
        tok->type = LesserEq;
    }
    else {
        tmp = realloc(tok->lexeme, 2 * sizeof(char));
        if (tmp == NULL)
            return alloc_failed(tok, tmp);
        else
            tok->lexeme = tmp;
        memcpy(tok->lexeme, (char[2]) {'/', '\0'}, 2 * sizeof(char));
        tok->type = Lesser;
    }
    return 0;
}

int match_two_template(const char** p, struct Token* tok, char c1, char c2, enum TokenType type) {
    // Builds a Token with the two given char (c1, c2) as lexeme. Advances the stream pointer.
    if (**p != c1) {
        printf("Bad call to |%c| !\n", c1);
        return 1;
    }
    char* tmp;
    consume(p);
    if (**p != c2) {
        printf("Unrecognized Token. |%c| must be followed by |%c|\n", c1, c2);
        return 1;
    }
    consume(p);
    tmp = realloc(tok->lexeme, 3 * sizeof(char));
    if (tmp == NULL)
        return alloc_failed(tok, tmp);
    else
        tok->lexeme = tmp;
    memcpy(tok->lexeme, (char[3]) {c1, c2, '\0'}, 3 * sizeof(char));
    tok->type = type;
    return 0;
}

int match_noteq(const char** p, struct Token* tok) {
    match_two_template(p, tok, '!', '=', NotEq);
}

int match_or(const char** p, struct Token* tok) {
    match_two_template(p, tok, '|', '|', Or);
}

int match_and(const char** p, struct Token* tok) {
    match_two_template(p, tok, '&', '&', And);
}

int match_endline(const char** p, struct Token* tok) {
    match_two_template(p, tok, ';', '\n', Endline);
}

int match_quote(const char** p, struct Token* tok) {
    // Build a Token with the QuotedStr object found in the stream from
    // the current pointer location.
    if (**p != '"') {
        printf("Bad call to \" !!\n");
        return 1;
    }

    size_t curr_size = 32;
    // Important to call calloc, NOT malloc !!
    char* astring = calloc(curr_size, sizeof(char));
    if (astring == NULL)
        return alloc_failed(tok, astring);

    char c = consume(p); // "
    char* tmp;
    int totchar = 0;
    astring[totchar++] = c;
    do {
        c = consume(p);
        if (c == '\0') {
            printf("Quoted strings must be terminated with \" !!\n");
            free(astring);
            return 1;
        }
        if (totchar * sizeof(char) >= curr_size) {
            tmp = realloc(astring, 2*curr_size);
            if (tmp == NULL)
                return alloc_failed(tok, astring);
            else
                astring = tmp;
            curr_size *= 2;
        }
        astring[totchar++] = c;
    }
    while (c != '"');
    tmp = realloc(tok->lexeme, sizeof(char) * totchar + 1);
    if (tmp == NULL)
        return alloc_failed(tok, astring);
    else
        tok->lexeme = tmp;
    memcpy(tok->lexeme, astring, totchar);
    tok->lexeme[totchar] = '\0';
    tok->type = QuotedStr;
    free(astring);
    return 0;
}

int match_int(const char** p, struct Token* tok) {
    // Build Token matching integer number (sequence of digits)
    if (**p < 48 || **p > 57) {
        printf("Bad call to int !!\n");
        return 1;
    }
    char* tmp;
    size_t curr_size = 8;
    char c;
    int totchar = 0;
    // Handle unique case when the INT is 0
    if (**p == '0') {
        c = consume(p);
        if (**p >= 48 && **p <= 57) {
            printf("Cannot have INT starting with 0\n");
            return 1;
        }
        memcpy(tok->lexeme, (char[2]) {'0', '\0'}, 2);
        tok->type = Int;
        return 0;
    }
    tmp = realloc(tok->lexeme, curr_size);
    if (tmp == NULL)
        return alloc_failed(tok, tmp);
    else
        tok->lexeme = tmp;
    while (**p >= 48 && **p <= 57) {
        c = consume(p);
        tok->lexeme[totchar++] = c;
        if (totchar * sizeof(char) >= curr_size) {
            tmp = realloc(tok->lexeme, 2*curr_size);
            if (tmp == NULL)
                return alloc_failed(tok, tmp);
            else
                tok->lexeme = tmp;
            curr_size *= 2;
        }
    }
    tmp = realloc(tok->lexeme, sizeof(char) * totchar + 1);
    if (tmp == NULL)
        return alloc_failed(tok, tmp);
    else
        tok->lexeme = tmp;
    tok->lexeme[totchar] = '\0';
    tok->type = Int;
    return 0;
}

int match_id(const char** p, struct Token* tok) {
    // Build Token matching all possible variable identifiers, or keyword.
    if (**p < 65 || **p > 122 || (**p > 90 && **p < 97)) {
        printf("Bad call to ID !!\n");
        return 1;
    }
    char* tmp;
    size_t curr_size = 16;
    int totchar = 0;
    tmp = realloc(tok->lexeme, curr_size);
    if (tmp == NULL)
        return alloc_failed(tok, tmp); 
    else
        tok->lexeme = tmp;
    while ((**p >= 48 && **p <= 57) ||
           (**p >= 65 && **p <= 90) ||
           (**p >= 97 && **p <= 122)) {
        tok->lexeme[totchar++] = **p;
        consume(p);
        if (totchar * sizeof(char) >= curr_size) {
            tmp = realloc(tok->lexeme, 2*curr_size);
            if (tmp == NULL)
                return alloc_failed(tok, tmp); 
            else
                tok->lexeme = tmp;
            curr_size *= 2;
        } 
    }
    tmp = realloc(tok->lexeme, sizeof(char) * totchar + 1);
    if (tmp == NULL)
        return alloc_failed(tok, tmp); 
    else
        tok->lexeme = tmp;
    tok->lexeme[totchar] = '\0';
    if (strcmp(tok->lexeme, "NULL") == 0)
        tok->type = Null;
    else if (strcmp(tok->lexeme, "True") == 0 ||
             strcmp(tok->lexeme, "False") == 0)
        tok->type = Bool;
    else if (strcmp(tok->lexeme, "break") == 0 ||
            strcmp(tok->lexeme, "if") == 0 ||
            strcmp(tok->lexeme, "else") == 0 ||
            strcmp(tok->lexeme, "while") == 0 ||
            strcmp(tok->lexeme, "readIn") == 0 ||
            strcmp(tok->lexeme, "writeOut") == 0 ||
            strcmp(tok->lexeme, "continue") == 0 )
        tok->type = Keyword;
    else
        tok->type = Var;
    return 0;
}

int next_Token(const char** p, struct Token* tok) {
    /*
     * Process the next character(s) and store the Token.
     * Return values:
     * 0      - STATUS OK
     * -1    - MEMORY ISSUE
     * 1    - INVALID CHARACTER SEQUENCE FOUND
    */
    char c = **p;
    if (c == '\0') {
        printf("Reached end of input\n");
        return 0;
    }
    //printf("NEXT-TOKEN : Current char is |%c|\n", c);
    if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
        return match_ws(p, tok);
    else if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))
        return match_id(p, tok);
    else
        switch (c) {
            case '[': return match_lbrack(p, tok);
            case ']': return match_rbrack(p, tok);
            case '+': return match_plus(p, tok);
            case '-': return match_minus(p, tok);
            case ',': return match_comma(p, tok);
            case '(': return match_lpar(p, tok);
            case ')': return match_rpar(p, tok);
            case '{': return match_lcurly(p, tok);
            case '}': return match_rcurly(p, tok);
            case '=': return match_equal(p, tok);
            case '"': return match_quote(p, tok);
            case '>': return match_greater(p, tok);
            case '<': return match_lesser(p, tok);
            case '/': return match_div(p, tok);
            case '!': return match_noteq(p, tok);
            case '*': return match_star(p, tok);
            case '%': return match_percent(p, tok);
            case '|': return match_or(p, tok);
            case '&': return match_and(p, tok);
            case '0': return match_int(p, tok);
            case '1': return match_int(p, tok);
            case '2': return match_int(p, tok);
            case '3': return match_int(p, tok);
            case '4': return match_int(p, tok);
            case '5': return match_int(p, tok);
            case '6': return match_int(p, tok);
            case '7': return match_int(p, tok);
            case '8': return match_int(p, tok);
            case '9': return match_int(p, tok);
            case '^': return match_pow(p, tok);
            case ';': return match_endline(p, tok);
            case '.': return match_dot(p, tok);
            default: return 1;
        }
}

struct Token* new_Token(char* lexeme, enum TokenType type) {
    struct Token* new;
    new = malloc(sizeof(struct Token));
    if (new == NULL)
        return NULL;
    new->lexeme = malloc(sizeof(char) * (strlen(lexeme) + 1));
    if (new->lexeme == NULL)
        return NULL;
    memcpy(new->lexeme, lexeme, strlen(lexeme) + 1);
    new->type = type;
    return new;
}

struct TokenList* new_TokenList(struct Token* tok) {
    struct TokenList* new;
    new = malloc(sizeof(struct TokenList));
    if (new == NULL)
        return NULL;
    new->token = new_Token(tok->lexeme, tok->type);
    if (new->token == NULL)
        return NULL;
    new->next = NULL;
    return new;
}

struct TokenList* build_TokenList(const char* fp) {
    // Prepare memory
    char init[16] = {0};
    struct Token* tok = new_Token(init, UNK);
    struct TokenList* head = NULL;
    struct TokenList* current = head;
    struct TokenList* tail;
    // Iterate
    int i = 0;
    int exit;
    while (*fp != '\0' && (exit = next_Token(&fp, tok)) == 0) {
        i++;
        if (head == NULL) {
            head = new_TokenList(tok);
            if (head == NULL) {
                free_Token(tok);
                return NULL;
            }
            current = head;
        }
        else {
            tail = new_TokenList(tok);
            if (tail == NULL) {
                free_TokenList(head);
                free_Token(tok);
                return NULL;
            }
            current->next = tail;
            current = tail;
        }
    }
    free_Token(tok);
    if (exit == 0) {
        // Was able to read the entire file
        printf("Tot Tokens = %d\n", i);
        printf("===================================\n");
    }
    else{
        // Encountered some error
        free_TokenList(head);
        head = NULL;
    }
    return head;
}
