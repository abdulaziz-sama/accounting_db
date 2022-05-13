#ifndef _PARSER_
#define _PARSER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

/*
The grammar:
Q   -> APPEND RECORD {ID : NUM , ID : NUM} ID
    | SELECT ID
    | CREATE ID
*/

bool Q(Token* token);

#endif