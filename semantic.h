#include "parser.h"

#define _int 0
#define _float 1
#define _string 2
#define _null 3
#define _bool 4
#define _list 5

#define _undef -100
#define UNDEFINED_SYMBOL -1
#define NODE_TYPE_ERROR -2
#define LIST_TYPE_ERROR -3


int resultType_aritm [6][6] = {
    /*
     Valid for + - * /
     Row index is 1st operand type.
     Col index is 2nd operand type.
     Value is the result type of the operation,
     or _undef if the operation is impossible.
    */

    /*             int       float    null    string    bool    list*/
    /* int   */ { _int,     _float,  _undef,  _undef,  _undef, _undef},
    /* float */ { _float,   _float,  _undef,  _undef,  _undef, _undef},
    /* string */{ _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* bool  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* null  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* list  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef}
};

int resultType_FloatDiv [6][6] = {
    /*
     Valid for /.
    */

    /*             int       float    null    string    bool    list*/
    /* int   */ { _float,   _float,  _undef,  _undef,  _undef, _undef},
    /* float */ { _float,   _float,  _undef,  _undef,  _undef, _undef},
    /* string */{ _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* bool  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* null  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* list  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef}
};


int resultType_compare [6][6] = {
    /*
     Valid for < > <= >=
     Row index is 1st operand type.
     Col index is 2nd operand type.
     Value is the result type of the operation,
     or _undef if the operation is impossible.
    */
    /*             int       float    null    string    bool    list*/
    /* int   */ { _bool,    _bool,   _undef,  _undef,  _undef, _undef},
    /* float */ { _bool,    _bool,   _undef,  _undef,  _undef, _undef},
   /* string */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* bool  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* null  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef},
    /* list  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef}
};


int resultType_logic [6][6] = {
    /*
     Valid for && || != ==
     Row index is 1st operand type.
     Col index is 2nd operand type.
     Value is the result type of the operation,
     or _undef if the operation is impossible.
    */
    /*             int       float    null    string    bool    list */
    /* int   */ { _bool,    _bool,   _bool,   _bool,   _bool,  _undef},
    /* float */ { _bool,    _bool,   _bool,   _bool,   _bool,  _undef},
   /* string */ { _bool,    _bool,   _bool,   _bool,   _bool,  _undef},
    /* bool  */ { _bool,    _bool,   _bool,   _bool,   _bool,  _undef},
    /* null  */ { _bool,    _bool,   _bool,   _bool,   _bool,  _undef},
    /* list  */ { _undef,   _undef,  _undef,  _undef,  _undef, _undef}
};


struct Symbol {
    char *sym;
    int type;
    int list_type; // type of each element if list
};


struct SymbolTable {
    // currently implemented as a linked list
    struct Symbol *head;
    struct SymbolTable *next;
};


struct Symbol* new_Sym(char *sym);

struct SymbolTable* alloc_SymbolTable();

struct Symbol* search_symbol(struct SymbolTable *table, char *lexeme);

int analyze_ListExpr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym);
int analyze_List(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym);
int analyze_ListElem(struct ParseTree *node, struct SymbolTable **table);
int analyze_Expr(struct ParseTree *node, struct SymbolTable **table, struct Symbol **sym);
