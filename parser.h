#include "lexer.h"

struct ParseTree {
    struct Token* data;
    struct ParseTree* child;
    struct ParseTree* sibling;
};


#define PARSING_ERROR -1
#define SUBTREE_OK 0
#define MEMORY_ERROR 1


/*
 * Create a new ParseTree from a given Token.
 * The Token is COPIED with all its internal data into a new memory loc.
 * Therefore when the ParseTree created by this function will be freed, Token* c will not.
 * Return NULL if a memory error happens.
*/
struct ParseTree* new_ParseTree(struct Token* c);


struct ParseTree* alloc_ParseTree();


void print_ParseTree(struct ParseTree* tree);

/*
 * Free memory for the ParseTree and all its internal data, including children and siblings.
 * ParseTrees are built by *cloning* Tokens, so only the clones are freed.
*/
void free_ParseTree(struct ParseTree* tree);


int build_ParseTree (struct TokenList* head, struct ParseTree** tree);

int build_ParseTree_FromFile (const char *fileName, struct ParseTree **tree);
