#include <time.h>
#include <stdio.h>
#include <sys/stat.h>

#include "lexer.h"

int main(int argc, char* argv[]) {
    /*
    const char* line = "+ >= continue<= - \"a  b !) c \"\"ah ahd\" ;\n == xyz != +0.53^-2 && a3c!=x + === [ Null)";
    struct token tok;
    tok.lexeme = malloc(16 * sizeof(char));
    tok.type = UNK;
    while (next_token(&line, &tok) > 0) {
        print_token(&tok);
    }
    free(tok.lexeme);
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
     * Build token list
    */
    // Will also print total count
    clock_t begin = clock();
    struct token* list = build_token_list(fp);
    clock_t end = clock();

    /*
     * Diplay all tokens
    */
    if (list == NULL) {
        printf("UNRECOGNIZED SEQUENCE OF CHARACTERS: NOT VALID TOKEN\n");
    }
    else {
        int j = 0;
        struct token* current = list;
        while (current != NULL) {
            print_token(current);
            j++;
            current = current->next;
        }
        printf("Tot tokens = %d\n", j);
        printf("===================================\n");
    }
    free_token(list);

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Total time spent building token list = %f\n", time_spent);
}
