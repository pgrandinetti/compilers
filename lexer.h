enum TokenType {
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
    //Question, // ?
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
    Eof
};

const char* type2char (enum TokenType t);

struct Token{
    char* lexeme;
    enum TokenType type;
    struct Token* next;
};

void print_Token(struct Token* p);

void free_Token(struct Token* tok);

struct Token* build_Token_list(const char* fp);
