/*
I'll be using https://github.com/cstack/db_tutorial implementation of B-tree, while
modifying a few things to make it works appropriately for the purpose of my project.
*/


#ifndef _BTREE_
#define _BTREE_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "pager.h"

typedef enum { NODE_INTERNAL, NODE_LEAF } BtreeNodeType;

typedef struct Table {
  uint32_t table_place;
  uint32_t table_root_page;
  uint32_t table_row_id_value;
  struct Pager* pager;
} Table;


typedef struct Cursor {
  Table* table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table;  // Indicates a position one past the last element
} Cursor;

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


/*
 * Internal Node Header Layout
 */
#define INTERNAL_NODE_NUM_KEYS_SIZE sizeof(uint32_t)
#define INTERNAL_NODE_NUM_KEYS_OFFSET COMMON_NODE_HEADER_SIZE
#define INTERNAL_NODE_RIGHT_CHILD_SIZE sizeof(uint32_t)
#define INTERNAL_NODE_RIGHT_CHILD_OFFSET (INTERNAL_NODE_NUM_KEYS_OFFSET + \
    INTERNAL_NODE_NUM_KEYS_SIZE)
#define INTERNAL_NODE_HEADER_SIZE (COMMON_NODE_HEADER_SIZE + \
    INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE)

/*
 * Internal Node Body Layout
 */
#define INTERNAL_NODE_KEY_SIZE sizeof(uint32_t)
#define INTERNAL_NODE_CHILD_SIZE sizeof(uint32_t)
#define INTERNAL_NODE_CELL_SIZE (INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE)
/* Keep this small for testing */
#define INTERNAL_NODE_MAX_CELLS 3


/* Leaf node header layout */

#define LEAF_NODE_NUM_CELLS_SIZE sizeof(uint32_t)
#define LEAF_NODE_NUM_CELLS_OFFSET COMMON_NODE_HEADER_SIZE
#define LEAF_NODE_NEXT_LEAF_SIZE sizeof(uint32_t)
#define LEAF_NODE_NEXT_LEAF_OFFSET \
    LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE
#define LEAF_NODE_HEADER_SIZE  (COMMON_NODE_HEADER_SIZE + \
    LEAF_NODE_NUM_CELLS_SIZE + LEAF_NODE_NEXT_LEAF_SIZE)


/* Leaf node body layout */

#define LEAF_NODE_KEY_SIZE sizeof(uint32_t)
#define LEAF_NODE_VALUE_SIZE ROW_SIZE
#define LEAF_NODE_VALUE_OFFSET LEAF_NODE_KEY_SIZE
#define LEAF_NODE_CELL_SIZE (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)
#define LEAF_NODE_SPACE_FOR_CELLS (PAGE_SIZE - LEAF_NODE_HEADER_SIZE)
#define LEAF_NODE_MAX_CELLS (LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE)
#define LEAF_NODE_RIGHT_SPLIT_COUNT ((LEAF_NODE_MAX_CELLS + 1) / 2)
#define LEAF_NODE_LEFT_SPLIT_COUNT \
    ((LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT)


uint32_t* leaf_node_num_cells(void* node);
void* leaf_node_cell(void* node, uint32_t cell_num);
uint32_t* leaf_node_key(void* node, uint32_t cell_num);
void* leaf_node_value(void* node, uint32_t cell_num);
void initialize_leaf_node(void* node);
BtreeNodeType get_node_type(void* node);
void set_node_type(void* node, BtreeNodeType type);
void set_node_root(void* node, bool is_root);

Cursor* table_find(Table* table, uint32_t key);
Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key);
void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, void* record);
void leaf_node_insert(Cursor* cursor, uint32_t key, void* record);

void print_tree(struct Pager* pager, uint32_t page_num, uint32_t indentation_level);

Cursor* table_start(Table* table);
void cursor_advance(Cursor* cursor);
void* cursor_value(Cursor* cursor);

#endif