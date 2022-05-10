#include "pager.h"
#include "lexer.h"

int main(int argc, char* argv[]){

    create_keyword_dfa();
    char* str1 = "APPEND RECORD {cash:200, expenses  : 500} jornal";
    char* advance = str1;
    Token* result = NULL;
    while(*advance != '\0'){
        result = tokenize(&str1, &advance);
        if(result != NULL){
            printf("Token class %i\n", result->class);
            if(result->value != NULL){
                printf("value %s\n", (char*)result->value);
                free(result->value);
            }

            free(result);
        } else break;
    }
    
    // char* result = strify(&str1, &advance);
    // printf("%s\n", str1);

    // free(result);

    // str1 = advance;
    // result = strify(&str1, &advance);
    // printf("%s\n", result);

    // free(result);
    
}
