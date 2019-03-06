#include "lexer.h"

struct ParseTree {
    struct Token* data;
    struct ParseTree* child;
    struct ParseTree* sibling;
};


/*
 * Create a new ParseTree from a given Token.
 * The Token is COPIED with all its internal data into a new memory loc.
 * Return NULL if a memory error happens.
*/
struct ParseTree* new_ParseTree(struct Token* c);

void print_ParseTree(struct ParseTree* tree, int indent);

/*
 * Free memory for the ParseTree and all its internal data, including children and siblings.
 * ParseTrees are built by *cloning* Tokens, so only the clones are freed.
*/
void free_ParseTree(struct ParseTree* tree);
