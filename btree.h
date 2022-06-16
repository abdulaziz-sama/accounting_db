#ifndef _BTREE_
#define _BTREE_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "pager.h"

typedef enum {LEAF_NODE, INTERNAL_NODE} BtreeNodeType;

/* Record size and offset */

#define DEBIT_ACCOUNT_SIZE 31
#define DEBIT_ACCOUNT_VALUE_SIZE 4
#define CREDIT_ACCOUNT_SIZE 31
#define CREDIT_ACCOUNT_VALUE_SIZE 4


#define ROW_SIZE (DEBIT_ACCOUNT_SIZE + DEBIT_ACCOUNT_VALUE_SIZE \
    + CREDIT_ACCOUNT_SIZE + CREDIT_ACCOUNT_VALUE_SIZE)

#define DEBIT_ACCOUNT_OFFSET 0
#define DEBIT_ACCOUNT_VALUE_OFFSET (DEBIT_ACCOUNT_OFFSET + DEBIT_ACCOUNT_SIZE)
#define CREDIT_ACCOUNT_OFFSET (DEBIT_ACCOUNT_VALUE_OFFSET + DEBIT_ACCOUNT_VALUE_SIZE)
#define CREDIT_ACCOUNT_VALUE_OFFSET (CREDIT_ACCOUNT_OFFSET + CREDIT_ACCOUNT_SIZE)


/* Common node header layout */

#define NODE_TYPE_SIZE sizeof(uint8_t)
#define IS_ROOT_SIZE sizeof(uint8_t)
#define IS_ROOT_OFFSET NODE_TYPE_SIZE
#define PARENT_POINTER_SIZE sizeof(uint32_t)
#define PARENT_POINTER_OFFSET (IS_ROOT_OFFSET + IS_ROOT_SIZE)
#define COMMON_NODE_HEADER_SIZE (NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE)


/* Leaf node header layout */

#define LEAF_NODE_NUM_CELLS_SIZE sizeof(uint32_t)
#define LEAF_NODE_NUM_CELLS_OFFSET COMMON_NODE_HEADER_SIZE
#define LEAF_NODE_HEADER_SIZE (COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE)

/* Leaf node body layout */

#define LEAF_NODE_KEY_SIZE sizeof(uint32_t)
#define LEAF_NODE_VALUE_SIZE ROW_SIZE
#define LEAF_NODE_VALUE_OFFSET LEAF_NODE_KEY_SIZE
#define LEAF_NODE_CELL_SIZE (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)
#define LEAF_NODE_SPACE_FOR_CELLS (PAGE_SIZE - LEAF_NODE_HEADER_SIZE)
#define LEAF_NODE_MAX_CELLS (LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE)


uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
void initialize_leaf_node(void* node);

#endif