#include "parser.h"

bool term(Token* token,token_class tok_class){
    if(token == NULL) return false;
    return token->class == tok_class;
}

bool Q1(Token* token){
    return (term(token, APPEND) && (token = token->next, term(token, RECORD))
    && (token = token->next, term(token, OPEN_PAREN))
    && (token = token->next, term(token, ID))
    && (token = token->next, term(token, COLON))
    && (token = token->next, term(token, NUMBER))
    && (token = token->next, term(token, COMMA))
    && (token = token->next, term(token, ID))
    && (token = token->next, term(token, COLON))
    && (token = token->next, term(token, NUMBER))
    && (token = token->next, term(token, CLOSE_PAREN))
    && (token = token->next, term(token, ID)));
}

bool Q2(Token* token){
    return (term(token, SELECT) && (token = token->next, term(token, ID)));
}

bool Q3(Token* token){
    return (term(token, CREATE) && (token = token->next, term(token, ID)));
}

bool Q(Token* token){
    return Q1(token) || Q2(token) || Q3(token);
}
