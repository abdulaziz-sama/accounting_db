#include "lexer.h"

// #define TRAP_STATE 0
// #define START_STATE 1
// #define NUMBER_OF_STATES 2


char number_of_states = 2;
char number_of_accepting_states = 0;

void create_dfa(){
    for(int i=0; i<4; i++){
        char current_state = START_STATE;
        while(*keywords[i] != '\0'){
            char index = *keywords[i] - 65;
            if(keywords_dfa[current_state][index] == 0){
                keywords_dfa[current_state][index] = number_of_states;               
                current_state = number_of_states++;
            } else {
                current_state = keywords_dfa[current_state][index];
            }
            keywords[i]++;
        }
        accepting_states[number_of_accepting_states++] = current_state;
    }
    
    for(int i=0; i<number_of_accepting_states; i++){
        printf("accepting state: %i\n", accepting_states[i]);
    }    
}


char check_word(char* word){
    char current_state = START_STATE;
    while(*word != '\0'){
        if(*word >= 65 && *word <= 90){
            char index = *word - 65;
            current_state = keywords_dfa[current_state][index];
            word++;
        } else {
            printf("character is not in the alphabet.");
            exit(1);
        }
    }

    // check if the final state is accepting state
    for(int i=0; i<4; i++){
        if(current_state == accepting_states[i]){
            return 1;
        }
    }

    return 0;
}