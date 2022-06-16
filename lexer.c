#include "lexer.h"

/* A DFA for the language {CREATE, APPEND, SELECT, RECORD} */

static char* keywords[4] = {"APPEND", "CREATE", "RECORD", "SELECT"};

/* Build the DFA */
/* The start state is 1, and trap state is 0 */

static char keywords_dfa[26][26] = {0};
static char keywords_accepting_states[4];


/*
DFA for identifiers, the alphabet is {a-z, A-Z, 0-9, _}, 
and they have to start with a letter.
identifier_dfa[n][0] for letter transition
identifier_dfa[n][1] for digit transition and underscore
start state is 1
*/

static char identifier_dfa[3][2] = {
    {0,0},
    {2,0},
    {2,2}
};

static char identifier_accepting_states = 2;


// numbers_dfa, start state is 1, trap state is 0, accepting state is 2.
static char numbers_dfa[3] = {0,2,2};

static char numbers_accepting_states = 2;

// punctuations_dfa, start state is 1, trap state is 0, accepting state is 2.

char punctuations_dfa[3] = {0,2,0};

static char punctuations_accepting_states = 2;



/* 

Functions 

*/

void create_keyword_dfa(){

    char number_of_states = 2;
    char number_of_accepting_states = 0;
    for(int i=0; i<4; i++){
        int current_state = 1;
        while(*keywords[i] != '\0'){
            int index = *keywords[i] - 65;
            if(keywords_dfa[current_state][index] == 0){
                keywords_dfa[current_state][index] = number_of_states;               
                current_state = number_of_states++;
            } else {
                current_state = keywords_dfa[current_state][index];
            }
            keywords[i]++;
        }
        keywords_accepting_states[number_of_accepting_states++] = current_state;
    }
    
    // for(int i=0; i<number_of_accepting_states; i++){
    //     printf("accepting state: %i\n", keywords_accepting_states[i]);
    // }    
}
      

int get_keyword(char* keyword){
    if(strcmp(keyword, "APPEND") == 0) return APPEND;
    else if(strcmp(keyword, "CREATE") == 0) return CREATE;
    else if(strcmp(keyword, "RECORD") == 0) return RECORD;
    else if(strcmp(keyword, "SELECT") == 0) return SELECT;
}

int get_punctuation(char* punctuation){
    if(punctuation[0] == '{') return OPEN_PAREN;
    else if(punctuation[0] == '}') return CLOSE_PAREN;
    else if(punctuation[0] == ':') return COLON;
    else if(punctuation[0] == ',') return COMMA;
}


Token* tokenize(char** start, char** advance){
    int letters = 0;

    /*
    keep consuming input until current accepting dfas == 0
    then check previous accepting dfas to get the accepting dfa with highest priority
    */
    char previous_accepting_dfas = 0;
    char current_accepting_dfas = 0;
    
    char keyword_accept = 0b00000001;
    char keyword_accept_complement = 0b11111110;
    char identifier_accept = 0b00000010;
    char identifier_accept_complement = 0b11111101;
    char number_accept = 0b00000100;
    char number_accept_complement = 0b11111011;
    char punctuation_accept = 0b00001000;
    char punctuation_accept_complement = 0b11110111;

    /* Current state for each DFA */
    char keywords_current_state = 1;
    char identifiers_current_state = 1;
    char numbers_current_state = 1;
    char punctuations_current_state = 1;

    // consume any white space
    while(*(*advance) == ' '){
        *advance = (*advance+1);
    }
    *start = *advance;


    char current_symbol;
    while(*(*advance) != '\0'){
        current_symbol = *(*advance);

        /* 
        let all DFAs consume current symbol and transition based on it,
        then check whther they accept it or not
        */

       // First is the keyword DFA
       if(current_symbol >= 65 && current_symbol<= 90){
            char index = current_symbol - 65;
            keywords_current_state = keywords_dfa[keywords_current_state][index];
            /*
            check if keyword_current_state is an accepting state.
            If yes, then OR keyword_accept with current_accepting_dfas to make the
            bit that represent keyword dfa = 1, meaning it accepts it.
            If no, then AND current_accepting_dfas with keyword_accept_complement to make the
            bit that represent keyword dfa = 0, meaning it doesn't accept it.
            */
           int accepted_by_keyword = 0;
            for(int i=0; i<4; i++){
                if(keywords_current_state == keywords_accepting_states[i]){
                    accepted_by_keyword = 1;
                    break;
                }
            }
            if(accepted_by_keyword){
                current_accepting_dfas = current_accepting_dfas | keyword_accept;
            } else {
               current_accepting_dfas = current_accepting_dfas & keyword_accept_complement;                
            }

       } else {
           /* 
           if symbol is not in the alphabet, go to trap state,
           then AND current_accepting_dfas with keyword_accept_complement to make the
           bit that represent keyword dfa = 0, meaning it doesn't accept it.
           */
           keywords_current_state = 0;
           current_accepting_dfas = current_accepting_dfas & keyword_accept_complement;
       }


    // The identifier DFA
        if(current_symbol >= 'a' && current_symbol <= 'z' || current_symbol >= 'A' && current_symbol <= 'Z'){
            identifiers_current_state = identifier_dfa[identifiers_current_state][0];
        } else if(current_symbol >= '0' && current_symbol <= '9' || current_symbol == '_'){
            identifiers_current_state = identifier_dfa[identifiers_current_state][1];
        } else {
            /*
           if symbol is not in the alphabet, go to trap state,
           then AND current_accepting_dfas with identifier_accept_complement to make the
           bit that represent identifier dfa = 0, meaning it doesn't accept it.
            */
           identifiers_current_state = 0;
           current_accepting_dfas = current_accepting_dfas & identifier_accept_complement;
        }

        /* repeat same thing with identifier DFA if it accepts */
        if(identifiers_current_state == 2){
            current_accepting_dfas = current_accepting_dfas | identifier_accept;
        }


        // The numbers DFA
        if(current_symbol >= '0' && current_symbol <= '9'){
            numbers_current_state = numbers_dfa[numbers_current_state];
            if(numbers_current_state == 2){
                current_accepting_dfas = current_accepting_dfas | number_accept;
            } else {
                current_accepting_dfas = current_accepting_dfas & number_accept_complement;
            }
        } else {
            //if symbol is not in the alphabet, go to trap state, and repeat same steps as before
            numbers_current_state = 0;
            current_accepting_dfas = current_accepting_dfas & number_accept_complement;
        }



        // punctuations DFA

        if(current_symbol == '{' || current_symbol == '}' || current_symbol == ':' ||
        current_symbol == ',') {
            punctuations_current_state = punctuations_dfa[punctuations_current_state];
            if(punctuations_current_state == punctuations_accepting_states){
                current_accepting_dfas = current_accepting_dfas | punctuation_accept;        
            } else {
                current_accepting_dfas = current_accepting_dfas & punctuation_accept_complement;
            }
        } else {
            //if symbol is not in the alphabet, go to trap state, and repeat same steps as before
            punctuations_current_state = 0;
            current_accepting_dfas = current_accepting_dfas & punctuation_accept_complement;
        }





        /**********/

        if(current_accepting_dfas == 0){
            break;
        }

        letters++;
        *advance = (*advance+1);
        previous_accepting_dfas = current_accepting_dfas;

    }

    if(previous_accepting_dfas == 0){
        /* invalid input */
        printf("Invalid string starting from:\n%s \n", (*start));
        return NULL;
    } else {
       Token* token = malloc(sizeof(Token));
        char* value = malloc(sizeof(char) * letters + 1);
        memcpy(value, *start, letters);
        value[letters] = '\0';

       int keyword_dfa_accepted = keyword_accept & previous_accepting_dfas;
       if(keyword_dfa_accepted){
           token->class = get_keyword(value);
           token->value = NULL;
           token->next = NULL;
           free(value);
       } 
       else if(previous_accepting_dfas == punctuation_accept){
           token->class = get_punctuation(value);
           token->value = NULL;
           token->next = NULL;
           free(value);
       } 
       else {
           /* since the token can be either an identifier or a number, and not both, I can
           get away with this */
           token->class = previous_accepting_dfas;
           token->value = value;
           token->next = NULL;
       }

        return token;
    }

}