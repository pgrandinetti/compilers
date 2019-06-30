#include "parser.h"

#define INDENT_LEV 4

char* cgen_Expr (struct ParseTree* tree);
char* cgen_Assign (struct ParseTree* tree, int indent);
char* cgen_Input (struct ParseTree* tree, int indent);
char* cgen_IfLine (struct ParseTree* tree, int indent);
char* cgen_LoopLine (struct ParseTree* tree, int indent);
char* cgen_Break (struct ParseTree* tree, int indent);
char* cgen_Continue (struct ParseTree* tree, int indent);
char* cgen_Line (struct ParseTree* tree, int indent);
char* cgen_IfLine (struct ParseTree* tree, int indent);
char* cgen_Program (struct ParseTree* tree, int indent);
