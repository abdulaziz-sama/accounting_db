#include "pager.h"
#include "lexer.h"

int main(int argc, char* argv[]){

    create_dfa();
    if(check_word("APPEND")){
        printf("success\n");
    } else {
        printf("failure\n");
    }
    
}
