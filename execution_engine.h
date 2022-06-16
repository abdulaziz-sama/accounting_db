#ifndef _EXECUTION_ENGINE_
#define _EXECUTION_ENGINE_

#include "parser.h"
#include "pager.h"
#include "btree.h"

void execute(Q_Node* root, Pager* pager);

#endif