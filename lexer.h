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
    Keyword,
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
    ifLine,
    loopLine,
    keywordLine,
    Expr,
    BaseExpr,
    Operator,
    CondOp,
    IfConf,
    IfBody,
    OptElse,
    LoopBody,
    Obj,
    Str,
    Num
};

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

struct TokenList* build_TokenList(const char* fp);
