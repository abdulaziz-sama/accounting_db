#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pager.h"
#include "lexer.h"
#include "parser.h"
#include "check.h"
#include "execution_engine.h"
#include "btree.h"



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


    if(argc < 2){
        printf("File name is missing. To open a database, run the command:\n\
    \"./main filename\"\n");
    exit(EXIT_FAILURE);
    }

    char* filename = argv[1];
    create_keyword_dfa();

    Pager* pager = create_pager(filename);


    while(true){

        char* user_input = NULL;
        size_t buffer_length;
        ssize_t input_length;

        printf("Enter your query: ");

        ssize_t bytes_read = getline(&user_input, &buffer_length, stdin);

        if (bytes_read <= 0) {
            printf("Error reading input\n");
            exit(EXIT_FAILURE);
        }

        user_input[bytes_read - 1] = 0;


        if(strcmp(user_input, ".exit") == 0){
            printf("Exiting...\n");
            close_db(pager);
        }
        char* advance = user_input;
        Token* result = NULL;

        /*
        Create a linked list of tokens to pass to the parser
        */
        Token* head = NULL;
        Token* tail = NULL;
    

        while(*advance != '\0'){
            result = tokenize(&user_input, &advance);

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

        Q_Node* parse_tree = Q(head);

        if(parse_tree){
            printf("Successfully parsed\n");
            printf("Node type: %i\n", parse_tree->tag);
        } else {
            printf("Parsing failed\n");
        }
        if(parse_tree){
            if(analyseQ(parse_tree)){
                printf("Correct semantics\n");
            }
        }

        if(parse_tree){
            execute(parse_tree, pager);    
        }

        print_and_free(head);
    }
}
