#include "parser.h"

void* term(Token* token,token_class tok_class){
    if(token == NULL || token->class != tok_class) return NULL;
    if(token->class == ID){
        
        idExpr* node = malloc(sizeof(idExpr));
        node->name = token->value;
        return node;    
    } else if(token->class == NUMBER){
        /*
        change the value attribute of this node to a void pointer, and then send it to
        is_unsigned_int to check for validity.
        */
        numExpr* node = malloc(sizeof(numExpr));
        node->value = token->value;
        return node;
    } else {
        TokenNode* node = malloc(sizeof(TokenNode));
        node->value = token->class;
        return node;
    }
}

void* Q1(Token* token){
    Q_Node* node = malloc(sizeof(Q_Node));
    node->tag = Q1_TYPE;

    bool result = ((node->node.q1.append = term(token, APPEND)) &&
    (token = token->next, node->node.q1.record = term(token, RECORD))
    && (token = token->next, node->node.q1.open_paren = term(token, OPEN_PAREN))
    && (token = token->next, node->node.q1.debit_account = term(token, ID))
    && (token = token->next, node->node.q1.first_colon = term(token, COLON))
    && (token = token->next, node->node.q1.debit_value = term(token, NUMBER))
    && (token = token->next, node->node.q1.comma = term(token, COMMA))
    && (token = token->next, node->node.q1.credit_account = term(token, ID))
    && (token = token->next, node->node.q1.second_colon = term(token, COLON))
    && (token = token->next, node->node.q1.credit_value = term(token, NUMBER))
    && (token = token->next, node->node.q1.close_paren = term(token, CLOSE_PAREN))
    && (token = token->next, node->node.q1.table_name = term(token, ID)));
    if(result) return node;
    else return NULL;
}

void* Q2(Token* token){
    Q_Node* node = malloc(sizeof(Q_Node));
    node->tag = Q2_TYPE;
    bool result = ((node->node.q2.select = term(token, SELECT)) && (token = token->next, 
    node->node.q2.table_name = term(token, ID)));
    if(result)
        return node;
    else return NULL;
}

void* Q3(Token* token){
    Q_Node* node = malloc(sizeof(Q_Node));
    node->tag = Q3_TYPE;
    bool result = ((node->node.q3.create = term(token, CREATE)) && (token = token->next, 
    node->node.q3.table_name = term(token, ID)));
    if(result)
        return node;
    else return NULL;
}

void* Q4(Token* token){
    Q_Node* node = malloc(sizeof(Q_Node));
    node->tag = Q4_TYPE;
    bool result = ((node->node.q4.select = term(token, SELECT)) &&
        (token = token->next, node->node.q4.from = term(token, FROM)) &&
        (token = token->next, node->node.q4.table_name = term(token, ID)) &&
        (token = token->next, node->node.q4.record = term(token, RECORD)) &&
        (token = token->next, node->node.q4.rowid = term(token, NUMBER))
    );
    if(result) return node;
    else return NULL;

}

void* Q5(Token* token){
    Q_Node* node = malloc(sizeof(Q_Node));
    node->tag = Q5_TYPE;
    bool result = ((node->node.q5.delete = term(token, DELETE)) &&
        (token = token->next, node->node.q5.from = term(token, FROM)) &&
        (token = token->next, node->node.q5.table_name = term(token, ID)) &&
        (token = token->next, node->node.q5.record = term(token, RECORD)) &&
        (token = token->next, node->node.q5.rowid = term(token, NUMBER))
    );
    if(result) return node;
    else return NULL;

}

Q_Node* Q(Token* token){
    Q_Node* node = NULL;
    bool result = (node = Q1(token)) || (node = Q2(token)) || (node = Q3(token)) ||
        (node = Q4(token)) || (node = Q5(token));
    if(result)
        return node;
    else return NULL;
}
