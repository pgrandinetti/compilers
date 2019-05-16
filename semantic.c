#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "semantic.h"


int analyze_ListExpr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym);
int analyze_List(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym);
int analyze_ListElem(struct ParseTree *node, struct SymbolTable **table);
int analyze_Expr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym);
int analyze_Line(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack);
int analyze_ifCond(struct ParseTree *node, struct SymbolTable **table);


struct Symbol* new_Sym(char *sym) {
    struct Symbol *new;

    new = malloc(sizeof(struct Symbol));
    if (new == NULL)
        return NULL;
    new->sym = malloc(sizeof(char) * (strlen(sym) + 1));
    if (new->sym == NULL)
        return NULL;
    memcpy(new->sym, sym, strlen(sym) + 1);
    new->type = _undef;
    new->list_type = _undef;
    return new;
}


void free_Symbol(struct Symbol *sym) {
    if (sym == NULL)
        return;
    free(sym->sym);
    free(sym);
}


const char* type2str(int type){
    switch (type){
        case _int: return "int";
        case _float: return "float";
        case _string: return "string";
        case _bool: return "bool";
        case _null: return "null";
        case _list: return "list";
        case _undef: return "undef";
        case UNDEFINED_SYMBOL: return "UNDEFINED_SYMBOL";
        case NODE_TYPE_ERROR: return "NODE_TYPE_ERROR";
        case LIST_TYPE_ERROR: return "LIST_TYPE_ERROR";
        case NODE_OK: return "NODE_OK";
        case SEMANTIC_ERROR: return "SEMANTIC_ERROR";
        case BREAK_OUT_OF_CONTEXT: return "BREAK_OUT_OF_CONTEXT";
        case CONTINUE_OUT_OF_CONTEXT: return "CONTINUE_OUT_OF_CONTEXT";
        default: return "UNK";
    }
}


struct SymbolTable* alloc_SymbolTable() {
    struct SymbolTable *table;

    table = malloc(sizeof(struct SymbolTable));
    if (table == NULL)
        return NULL;
    table->head = NULL;
    table->next = NULL;
    return table;
}


void free_SymbolTable(struct SymbolTable *table) {
    struct SymbolTable *curr;

    while((curr=table) != NULL) {
        table = table->next;
        free_Symbol(curr->head);
        free(curr);
    }
}


struct Symbol* search_symbol(struct SymbolTable *table, char *lexeme) {
    while (table != NULL && table->head != NULL) {
        if (strcmp(table->head->sym, lexeme) == 0)
            return table->head;
        table = table->next;
    }
    return NULL;
}


void print_Symbol(struct Symbol *sym){
    if (sym == NULL)
        printf("Symbol is NULL\n");
    else
        printf("<sym=|%s|, type=%d>\n", sym->sym, sym->type);
}


void print_SymbolTable(struct SymbolTable *table){
    struct SymbolTable *current;

    current = table;
    printf("---Sym Table---\n");
    while (current != NULL && current->head != NULL) {
        print_Symbol(current->head);
        current = current->next;
    }
    printf("---End Table---\n");
}


void _add_symbol(struct SymbolTable **table, char *lexeme, int type) {
    struct Symbol *new;
    struct SymbolTable *new_head;

    new = new_Sym(lexeme);
    if (new == NULL) {
        printf("Memory Error While Creating Symbol: %s\n", lexeme);
        return;
    }
    new->type = type;
    new_head = alloc_SymbolTable();
    new_head->head = new;
    new_head->next = *table;
    *table = new_head;
    printf("Symbol Added Into Table: %s\n", lexeme);
    printf("Type Assigned to Symbol %s: %s\n", lexeme, type2str(type));
}


void add_symbol(struct SymbolTable **table, char *lexeme) {
    _add_symbol(table, lexeme, _undef);
}


struct ContextStack* alloc_Context() {
    return NULL;
}


void free_Context(struct ContextStack *stack){
    struct ContextStack *top;

    while((top=stack) != NULL){
        stack = stack->next;
        free(top);
    }
}


void push_Context(struct ContextStack **stack, enum TokenType type) {
    struct ContextStack *top;

    top = malloc(sizeof(struct ContextStack));
    if (top == NULL)
        return;
    top->top = type;
    top->next = *stack;
    *stack = top;
}


enum TokenType pop_Context(struct ContextStack **stack) {
    struct ContextStack *top;
    enum TokenType res;

    if (*stack == NULL)
        return UNK;

    top = *stack;
    res = top->top;
    *stack = (*stack)->next;
    free(top);
    return res;
}


void print_Context (struct ContextStack *stack) {
    printf("---Context---\n");
    while (stack != NULL){
        printf("<%s>\n", type2char(stack->top));
        stack = stack->next;
    }
    printf("---End Ctx---\n");
}


int len_Context(struct ContextStack *stack){
    int count;

    count = 0;
    while ( stack != NULL){
        ++count;
        stack = stack->next;
    }
    return count;
}


int assign_type(struct SymbolTable **table, char *lexeme, int type) {
    struct SymbolTable *tab;

    tab = *table;
    while (tab != NULL && tab->head != NULL) {
        if (strcmp(tab->head->sym, lexeme) == 0){
            tab->head->type = type;
            printf("Assigned type to identifier %s: %s\n", lexeme, type2str(type));
            return type;
        }
        tab = tab->next;
    }
    printf("ERROR: Tried to assign type to unkown identifier: %s.\n", lexeme);
    return UNDEFINED_SYMBOL;
}


int is_ComparisonOp (enum TokenType type) {
    return (type == Greater ||
            type == GreaterEq ||
            type == Lesser ||
            type == LesserEq);
}


int is_LogicOp (enum TokenType type) {
    return (type == EqEq ||
            type == NotEq ||
            type == And ||
            type == Or);
}


int is_AritmOp (enum TokenType type) {
    return (type == Plus ||
            type == Minus ||
            type == Star ||
            type == Div ||
            type == FloatDiv ||
            type == Percent);
}

/* 
   ---------------
   ---------------
*/

int _analyze_Program(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    struct ParseTree *line;
    int status, count;
    int res;

    line = node->child;
    status = _undef;
    count = 0;
    res = NODE_OK;
    while (line != NULL){
        printf("-----Line %d-----\n", ++count);
        status = analyze_Line(line, table, stack);
        printf("\n");
        if (status < 0){
            printf("ERROR: %s\n", type2str(status));
            res = SEMANTIC_ERROR;
        }
        else
            printf("OK. Type is %s\n", type2str(status));
        printf("-----------------\n");
        // Skip Endline
        line = line->sibling->sibling;
    }
    return res;
}

int analyze_Program(struct ParseTree *node) {
    struct SymbolTable *table;
    struct ContextStack *stack;
    int res;

    table = alloc_SymbolTable();
    stack = alloc_Context();

    res = _analyze_Program(node, &table, &stack);

    free_SymbolTable(table);
    free_Context(stack);

    return res;
}


int analyze_Var(struct ParseTree *node, struct SymbolTable **table) {
    struct Symbol* found;

    found = search_symbol(*table, node->data->lexeme);
    // found contains the _type of the symbol, or UNDEFINED
    if (found != NULL)
        return found->type;
    printf("Variable Symbol Not Found In Symbol Table: %s\n", node->data->lexeme);
    return UNDEFINED_SYMBOL;
}


int analyze_List(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym) {
    struct ParseTree *elems;
    int type;

    elems = node->child->sibling; // skip '['
    type = analyze_ListExpr(elems, table, sym);
    if (type < 0)
        return type;
    if (sym != NULL){
        printf("Setting List Type to %s: ", type2str(type));
        if (sym != NULL){
            printf(": %s\n", (*sym)->sym);
            (*sym)->list_type = type;
        }
        else
            printf("\n");
    }
    return _list;
}


int analyze_Num(struct ParseTree *tree) {
    struct ParseTree *num;

    num = tree->child; // the Float node
    if (num->data->type != Float)
        // it's the sign
        num = num->sibling;
    if (num->child->data->type != Int)
        return _float;
    if (num->child->sibling != NULL)
        // then there is either frac or pow
        return _float;
    return _int;
}


int analyze_Obj(struct ParseTree *tree, struct SymbolTable **table, struct Symbol **sym) {
    struct ParseTree *node;
    int list_type;

    node = tree->child; // Obj always have 1 child
    if (node->data->type == Num)
        return analyze_Num(node);
    if (node->data->type == Str)
        return _string;
    if (node->data->type == Null)
        return _null;
    if (node->data->type == Bool)
        return _bool;
    if (node->data->type == Var)
        return analyze_Var(node, table);
    if (node->data->type == ListElem)
        return analyze_ListElem(node, table);
    if (node->data->type == List)
        return analyze_List(node, table, sym);
    printf("Leaf (Obj) Token Type Not Recognized\n");
    print_ParseTree(node);
    return NODE_TYPE_ERROR;
}


int analyze_BaseExpr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym) {
    struct ParseTree *child;

    child = node->child;
    if (child->data->type == Obj)
        return analyze_Obj(child, table, sym);
    // else must be Lpar '(' and sub expression
    child = child->sibling;
    return analyze_Expr(child, table, sym);
}


int analyze_Term(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym) {
    struct ParseTree *child, *op;
    int type1, type2, result;

    child = node->child;
    type1 = analyze_BaseExpr(child, table, sym);
    if (type1 < 0)
        return type1;
    if (child->sibling == NULL)
        return type1;
    op = child->sibling; // save the operator
    type2 = analyze_Term(op->sibling, table, sym);
    if (type2 < 0)
        return type2;

    // Now compute the result type
    if (is_AritmOp(op->data->type))
        if (op->data->type == FloatDiv)
            result = resultType_FloatDiv[type1][type2];
        else
            result = resultType_aritm[type1][type2];
    else
        result = NODE_TYPE_ERROR;
    if (result == _undef)
        printf("Operation %s not defined for types: %s, %s\n", op->data->lexeme, type2str(type1), type2str(type2));
    return result;
}


int analyze_Pred(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym) {
    struct ParseTree *child, *op;
    int type1, type2, result;

    child = node->child;
    type1 = analyze_Term(child, table, sym);
    if (type1 < 0)
        return type1;
    if (child->sibling == NULL)
        return type1;
    op = child->sibling; // save the operator
    type2 = analyze_Pred(op->sibling, table, sym);
    if (type2 < 0)
        return type2;

    // Now compute the result type
    if (op->data->type == Plus || op->data->type == Minus)
        result = resultType_aritm[type1][type2];
    else
        result = NODE_TYPE_ERROR;
    if (result == _undef)
        printf("Operation %s not defined for types: %s, %s\n", op->data->lexeme, type2str(type1), type2str(type2));
    return result;
}


int analyze_Expr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym){
    struct ParseTree *child, *op;
    int type1, type2, result;

    child = node->child;
    type1 = analyze_Pred(child, table, sym);
    if (type1 < 0){
        printf("Sub Expression Ill-Formed ");
        if (sym != NULL)
            printf("For Symbol: %s\n", (*sym)->sym);
        else
            printf("\n");
        return type1;
    }
    if (child->sibling == NULL)
        return type1;
    op = child->sibling; // save the operator
    type2 = analyze_Expr(op->sibling, table, sym);
    if (type2 < 0){
        printf("Sub Expression Ill-Formed ");
        if (sym != NULL)
            printf("For Symbol: %s\n", (*sym)->sym);
        else
            printf("\n");
        return type2;
    }

    // Now compute the result type
    if (is_ComparisonOp(op->data->type))
        result = resultType_compare[type1][type2];
    else if (is_LogicOp(op->data->type))
        result = resultType_logic[type1][type2];
    else
        result = NODE_TYPE_ERROR;
    return result;
}


int analyze_ListElem(struct ParseTree *node, struct SymbolTable **table) {
    char *var;
    struct ParseTree *idx;
    struct Symbol *found, *found_idx;

    var = node->child->data->lexeme;
    found = search_symbol(*table, var);
    if (found == NULL){
        printf("List Name Not Found In Symbol Table: %s\n", var);
        return UNDEFINED_SYMBOL;
    }
    else if (found->type != _list){
        printf("Identifier is not a list: %s\n", var);
        return NODE_TYPE_ERROR;
    }
    else {
        idx = node->child->sibling->sibling;
        if (idx->data->type == Var) {
            found_idx = search_symbol(*table, idx->data->lexeme);
            if (found_idx == NULL){
                printf("Index For List %s is Undefined Symbol: %s\n", var, idx->data->lexeme);
                return UNDEFINED_SYMBOL;
            }
            else if (found_idx->type != _int){
                printf("List %s Indexes Must Be Int, it is %s\n", var, type2str(found_idx->type));
                return NODE_TYPE_ERROR;
            }
        }
        return found->list_type;
    }
}


int analyze_ListExpr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym) {
    // List expressions are required to have 1 type only
    int type, curr_type, first;
    struct ParseTree *curr_obj;

    curr_obj = node->child;
    first = 1;
    while (curr_obj != NULL) {
        if (curr_obj->data->type == Comma)
            // skip comma in list
            curr_obj = curr_obj->sibling;
        curr_type = analyze_Obj(curr_obj, table, sym);
        if (curr_type == UNDEFINED_SYMBOL || curr_type == _undef)
            return curr_type;
        if (first) {
            type = curr_type;
            first = 0;
        }
        else
            if (type != curr_type)
                if (type == _int && curr_type == _float)
                    type = _float;
                else if (type == _float && curr_type != _int)
                    return LIST_TYPE_ERROR;
        curr_obj = curr_obj->sibling;
    }
    return type;
}


int analyze_Assign(struct ParseTree *node, struct SymbolTable **table) {
    struct ParseTree *var, *expr;
    int found_var, valid_expr;
    struct Symbol *sym;

    var = node->child;
    expr = var->sibling->sibling;

    found_var = analyze_Var(var, table);
    if (found_var == UNDEFINED_SYMBOL)
        add_symbol(table, var->data->lexeme);

    sym = search_symbol(*table, var->data->lexeme);
    valid_expr = analyze_Expr(expr, table, &sym);
    if (valid_expr < 0)
        printf("Expression is not valid\n");
    else{
        if (sym->type != _undef && valid_expr != sym->type)
            printf("WARNING: Type for Identifier will be overwritten: %s. It was %s.\n",
                   var->data->lexeme, type2str(sym->type));
        assign_type(table, var->data->lexeme, valid_expr);
    }
    return valid_expr;
}


int analyze_Input(struct ParseTree *node, struct SymbolTable **table) {
    struct ParseTree *readin, *var;
    struct Symbol *found;
    int type;

    readin = node->child;
    var = readin->sibling;
    if (strcmp(readin->data->lexeme, "readInt") == 0)
        type = _int;
    else if (strcmp(readin->data->lexeme, "readFloat") == 0)
        type = _float;
    else if (strcmp(readin->data->lexeme, "readStr") == 0)
        type = _string;
    else if (strcmp(readin->data->lexeme, "readBool") == 0)
        type = _bool;
    else
        type = _undef;

    found = search_symbol(*table, var->data->lexeme);
    if (found != NULL)
        printf("WARNING: Identifier Will Be Overwritten: %s\n", var->data->lexeme);
    if (type == _undef)
        printf("WARNING: Identifier Will Have Undefined Type: %s\n", var->data->lexeme);
    _add_symbol(table, var->data->lexeme, type);
    return type;
}


int analyze_Output(struct ParseTree *node, struct SymbolTable **table) {
    int res;
    res = analyze_Obj(node->child->sibling, table, NULL);
    if (res > 0)
        return NODE_OK;
    return res;
}


int analyze_ifCond(struct ParseTree *node, struct SymbolTable **table) {
    struct ParseTree *expr;
    int res;

    expr = node->child->sibling;
    res = analyze_Expr(expr, table, NULL);
    if (res != _bool){
        printf("ERROR. If Condition Must Be Boolean Expression: %s\n", type2str(res));
        return NODE_TYPE_ERROR;
    }
    return res;
}


int analyze_OptElse(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    struct ParseTree *line;
    int res;

    // skip 'else' keyword
    line = node->child->sibling;
    while (line != NULL){
        res = analyze_Line(line, table, stack);
        if (res < 0){
            printf("ERROR In Else Body\n");
            return res;
        }
        // Skip Endline
        line = line->sibling->sibling;
    }
    return NODE_OK;
}


int analyze_ifBody(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    struct ParseTree *line;
    int res;

    // we alredy know ifBody cannot be empty from parsing
    line = node->child;
    while (line != NULL){
        res = analyze_Line(line, table, stack);
        if (res < 0){
            printf("ERROR In IfBody\n");
            return res;
        }
        // Skip Endline
        line = line->sibling->sibling;
        if (line == NULL)
            break;
        if (line->data->type == OptElse)
            return analyze_OptElse(line, table, stack);
    }
    return NODE_OK;
}


int analyze_IfLine(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    int res_cond, res_body;

    push_Context(stack, IfLine);

    res_cond = analyze_ifCond(node->child->sibling, table);
    res_body = analyze_ifBody(node->child->sibling->sibling, table, stack);

    pop_Context(stack);

    if (res_cond < 0){
        printf("If Condition Ill-Formed: %s\n", type2str(res_cond));
        return res_cond;
    }
    if (res_body < 0){
        printf("If Body Ill-Formed: %s\n", type2str(res_cond));
        return res_body;
    }
    return NODE_OK;
}


int analyze_BreakLine(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    // 'break' allowed only in conditionals and loops
    if ((*stack)->top != IfLine && (*stack)->top != LoopLine)
        return BREAK_OUT_OF_CONTEXT;
    return NODE_OK;
}


int analyze_ContinueLine(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    // 'continue' allowed only in conditionals and loops
    if ((*stack)->top != IfLine && (*stack)->top != LoopLine)
        return CONTINUE_OUT_OF_CONTEXT;
    return NODE_OK;
}


int analyze_LoopLine(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    int res_cond, res_body;

    push_Context(stack, LoopLine);

    res_cond = analyze_ifCond(node->child->sibling, table);
    res_body = _analyze_Program(node->child->sibling->sibling, table, stack);

    pop_Context(stack);

    if (res_cond < 0){
        printf("Loop Condition Ill-Formed: %s\n", type2str(res_cond));
        return res_cond;
    }
    if (res_body < 0){
        printf("Loop Body Ill-Formed: %s\n", type2str(res_cond));
        return res_body;
    }
    return NODE_OK;
}


int analyze_Line(struct ParseTree *node, struct SymbolTable **table, struct ContextStack **stack) {
    struct ParseTree *line;
    int res;

    line = node->child;
    if (line->data->type == Assign)
        res = analyze_Assign(line, table);
    else if (line->data->type == IfLine)
        res = analyze_IfLine(line, table, stack);
    else if (line->data->type == Break)
        res = analyze_BreakLine(line, table, stack);
    else if (line->data->type == Continue)
        res = analyze_BreakLine(line, table, stack);
    else if (line->data->type == Input)
        res = analyze_Input(line, table);
    else if (line->data->type == Output)
        res = analyze_Output(line, table);
    else if (line->data->type == LoopLine)
        res = analyze_LoopLine(line, table, stack);
    else
        res = SEMANTIC_ERROR;

    if (res < 0)
        return res;
    else
        return NODE_OK;
}


int main(int argc, char* argv[]){
    struct SymbolTable *table;
    struct ContextStack *stack;
    char sym;
    struct ParseTree *tree, *assign1, *line2, *assign3;
    int parser, semantic;

    if (argc < 2) {
        printf("Expecting exactly 1 argument: file path.\n");
        return -1;
    }
    char const* const fileName = argv[1];

    tree = alloc_ParseTree();
    if (tree == NULL)
        return MEMORY_ERROR;

    parser = build_ParseTree_FromFile(fileName, &tree);

    print_ParseTree(tree);

    if (parser != SUBTREE_OK){
        printf("PARSING ERROR\n");
        free_ParseTree(tree);
        return -1;
    }

    semantic = analyze_Program(tree);

    if (semantic < 0){
        printf("SEMANTIC ERROR\n");
        free_ParseTree(tree);
        return -1;
    }

    free_ParseTree(tree);
    return 0;
}
