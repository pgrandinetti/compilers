#include <time.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "cgen.h"

// gcc main.c parser.c lexer.c


int main_parser(int argc, char* argv[]);
int main_lexer(int argc, char* argv[]);
int main_cgen(int argc, char* argv[]);


int main(int argc, char* argv[]) {
    //main_parser(argc, argv);
    main_cgen(argc, argv);
}


int main_cgen(int argc, char* argv[]) {
    struct ParseTree *tree, *tmp;
    char* code;
    int status;
    char* outFile;

    if (argc < 2) {
        printf("Expecting at the least 1 argument: file path.\n");
        return 1;
    }
    char const* const fileName = argv[1];

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);
    print_ParseTree(tree);

    if (status != SUBTREE_OK) {
        printf("PARSING ERROR\n");
        return - 1;
    }

    //tmp = tree;//->child->child;//->child->child;
    //tmp = tmp->sibling->sibling; // Expr
    //tmp = tmp->child->child->child->child->child; // Str

    code = code_gen(tree);
    if (code == NULL) {
        printf("Error in CODE-GEN");
        free_ParseTree(tree);
        return -1;
    }
    printf("Generated code is:\n|\n%s|\n", code);

    if (argc > 2)
        outFile = argv[2];
    else
        outFile = "./out.py";

    FILE *fp = fopen(outFile, "w");
    if (fp != NULL) {
        fputs(code, fp);
        fclose(fp);
    }

    free(code);
    free_ParseTree(tree);
    return status;

}

int main_parser(int argc, char* argv[]) {
    struct ParseTree *tree;
    int status;

    if (argc < 2) {
        printf("Expecting exactly 1 argument: file path.\n");
        return 1;
    }
    char const* const fileName = argv[1];

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    status = build_ParseTree_FromFile(fileName, &tree);
    print_ParseTree(tree);

    if (status != SUBTREE_OK) {
        printf("PARSING ERROR\n");
        return - 1;
    }

    free_ParseTree(tree);

    return status;
}


int main_lexer(int argc, char* argv[]) {
    /*
    const char* line = "+ >= continue<= - \"a  b !) c \"\"ah ahd\" ;\n == xyz != +0.53^-2 && a3c!=x + === [ Null)";
    struct Token tok;
    tok.lexeme = malloc(16 * sizeof(char));
    tok.type = UNK;
    while (next_Token(&line, &tok) > 0) {
        print_Token(&tok);
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
     * Build Token list
    */
    // Will also print total count
    clock_t begin = clock();
    struct TokenList* list = build_TokenList(fp);
    clock_t end = clock();


    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Total time spent building Token list = %f\n", time_spent);

    struct TokenList* no_ws = NULL;
    /*
     * Diplay all Tokens
    */
    if (list == NULL)
        printf("UNRECOGNIZED SEQUENCE OF CHARACTERS: NOT VALID TOKEN\n");
    else {
        print_TokenList(list);
        no_ws = strip_WS(list);
        print_TokenList(no_ws);
    }
    free_TokenList(list);
    free_TokenList(no_ws);
}
