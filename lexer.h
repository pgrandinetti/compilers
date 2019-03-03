enum token_type {
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

const char* type2char (enum token_type t);

struct token{
    char* lexeme;
    enum token_type type;
    struct token* next;
};

void print_token(struct token* p);

void free_token(struct token* tok);

struct token* build_token_list(const char* fp);
