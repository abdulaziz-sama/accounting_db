#ifndef _PARSER_
#define _PARSER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

typedef enum {Q1_TYPE, Q2_TYPE, Q3_TYPE} NodeType;

typedef struct numExpr {
    void* value;
} numExpr;

typedef struct idExpr {
    char* name;
} idExpr;

typedef struct TokenNode {
    token_class value;
} TokenNode;

typedef struct Q1_Node {
    TokenNode* append;
    TokenNode* record;
    TokenNode* open_paren;
    idExpr* debit_account;
    TokenNode* first_colon;
    numExpr* debit_value;
    TokenNode* comma;
    idExpr* credit_account;
    TokenNode* second_colon;
    numExpr* credit_value;
    TokenNode* close_paren;
    idExpr* table_name;
} Q1_Node;

typedef struct Q2_Node {
    TokenNode* select;
    idExpr* table_name;
} Q2_Node;

typedef struct Q3_Node {
    TokenNode* create;
    idExpr* table_name;
} Q3_Node;

typedef struct Q_Node {
    NodeType tag;
    union {
        Q1_Node q1;
        Q2_Node q2;
        Q3_Node q3;
    } node;

} Q_Node;

/*
The grammar:
Q   -> APPEND RECORD {ID : NUM , ID : NUM} ID
    | SELECT ID
    | CREATE ID
*/

Q_Node* Q(Token* token);

#endif