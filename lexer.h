enum TokenType {
    // Basic Token Types in the Grammar
    Comma,
    Lpar, // (
    Rpar, // )
    Lbrack, // [
    Rbrack, // ]
    Lcurly, // {
    Rcurly, // }
    Plus,
    Minus,
    Equal,
    EqEq, // ==
    //Exclam, // !
    NotEq, // !=
    Lesser, // <
    Greater, // >
    LesserEq,
    GreaterEq,
    Star, // *
    Div, // /
    FloatDiv, // /.
    Percent, // %
    Or,
    And, // &
    Int,
    Float,
    Pow,
    Dot,
    QuotedStr, // "
    Bool,
    Null,
    Var,
    ReadIn,
    WriteOut,
    If,
    While,
    Break,
    Continue,
    Else,
    Endline,
    WS, // white space(s)
    UNK,
/* -----=====----- */
    // Grammar Constructs
    // used by the parser, not the lexer
    Program,
    Line,
    Assign,
    Input,
    Output,
    IfLine,
    LoopLine,
    Expr,
    BaseExpr,
    Operator,
    CondOp,
    IfCond,
    IfBody,
    OptElse,
    LoopBody,
    Obj,
    Str,
    Num,
    Frac,
    List,
    ListExpr
};

// Generate readable string instead of int
const char* type2char (enum TokenType t);

struct Token{
    char* lexeme;
    enum TokenType type;
};

struct TokenList {
    struct Token* token;
    struct TokenList* next;
};

void print_Token(struct Token* p);

void print_TokenList(struct TokenList* p);

void free_Token(struct Token* tok);

void free_TokenList(struct TokenList* tok);

struct Token* new_Token(char* lexeme, enum TokenType tok);

struct TokenList* new_TokenList(struct Token* tok);

/*
 * Create a TokenList from the characters stream
 * (typically a file with source code).
 * Return NULL if the characters sequence is not valid in the Grammar.
*/
struct TokenList* build_TokenList(const char* fp);

struct TokenList* strip_WS(struct TokenList* list);
