#ifndef _LEXER_
#define _LEXER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {APPEND, CREATE, ID, RECORD, NUMBER, SELECT, OPEN_PAREN, CLOSE_PAREN,
COLON, COMMA} token_class;

typedef struct Token {
    token_class class;
    void* value;
    struct Token* next;
} Token;


/* 
The world's worst lexer.
I made it while learning about DFA & NFA, it has limited functionality,
and probably will be rewritten from scratch in the future depending on how far
I'll go in my learning journey.
*/


void create_keyword_dfa();
Token* tokenize(char** start, char** advance);


#endif