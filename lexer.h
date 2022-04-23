#ifndef _LEXER_
#define _LEXER_

#include <stdio.h>
#include <stdlib.h>


/* 
The world's worst lexer.
I made it while learning about DFA & NFA, it has limited functionality,
and probably will be rewritten from scratch in the future depending on how far
I'll go in my learning journey.
*/

/* A DFA for the language {CREATE, APPEND, SELECT, RECORD} */

static char* keywords[4] = {"APPEND", "CREATE", "RECORD", "SELECT"};

/* Build the DFA */

#define TRAP_STATE 0
#define START_STATE 1

static char keywords_dfa[26][26] = {0};
static char accepting_states[4];

void create_dfa();

/* a temporary function to test the working of the DFA, return 1 on success, 0 on failure. */
char check_word(char* word);

#endif