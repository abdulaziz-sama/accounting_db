#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
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
