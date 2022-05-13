#include "pager.h"
#include "lexer.h"
#include "parser.h"

void print_and_free(Token* head){
    Token* temp = NULL;
    while(head != NULL){
        // printf("Token class %i\n", head->class);
        if(head->value != NULL){
            // printf("value %s\n", (char*)head->value);
            free(head->value);
        }

        temp = head;
        head = head->next;
        free(temp);
    }
}

int main(int argc, char* argv[]){

    create_keyword_dfa();
    char* str1 = "APPEND RECORD {cash:APPEND, expenses  : 500} jornal";
    char* advance = str1;
    Token* result = NULL;

    /*
    Create a linked list of tokens to pass to the parser
    */
   Token* head = NULL;
   Token* tail = NULL;
   

    while(*advance != '\0'){
        result = tokenize(&str1, &advance);

        if(result != NULL){
            if(head == NULL){
                head = result;
                tail = result;
            } else {
                tail->next = result;
                tail = result;
            }
        } else break;
    }

    if(Q(head)){
        printf("Successfully parsed\n");
    } else {
        printf("Parsing failed\n");
    }

    print_and_free(head);
    
    // char* result = strify(&str1, &advance);
    // printf("%s\n", str1);

    // free(result);

    // str1 = advance;
    // result = strify(&str1, &advance);
    // printf("%s\n", result);

    // free(result);
    
}
