#include "btree.h"

uint32_t* internal_node_num_keys(void* node) {
  return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t* internal_node_right_child(void* node) {
  return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t* internal_node_cell(void* node, uint32_t cell_num) {
  return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t* internal_node_child(void* node, uint32_t child_num) {
  uint32_t num_keys = *internal_node_num_keys(node);
  if (child_num > num_keys) {
    printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
    exit(EXIT_FAILURE);
  } else if (child_num == num_keys) {
    return internal_node_right_child(node);
  } else {
    return internal_node_cell(node, child_num);
  }
}

uint32_t* internal_node_key(void* node, uint32_t key_num) {
  return (void*)internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

void initialize_internal_node(void* node) {
  set_node_type(node, NODE_INTERNAL);
  set_node_root(node, false);
  *internal_node_num_keys(node) = 0;
}



uint32_t* leaf_node_num_cells(void* node){
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void* leaf_node_cell(void* node, uint32_t cell_num) {
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t* leaf_node_key(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num);
}

void* leaf_node_value(void* node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

uint32_t* leaf_node_next_leaf(void* node) {
  return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

void initialize_leaf_node(void* node) {
  set_node_type(node, NODE_LEAF);
  set_node_root(node, false);
  *leaf_node_num_cells(node) = 0;
  *leaf_node_next_leaf(node) = 0;  // 0 represents no sibling  
}



BtreeNodeType get_node_type(void* node) {
  uint8_t value = *((uint8_t*)(node));
  return (BtreeNodeType)value;
}

void set_node_type(void* node, BtreeNodeType type) {
  uint8_t value = type;
  *((uint8_t*)(node)) = value;
}

bool is_node_root(void* node) {
  uint8_t value = *((uint8_t*)(node + IS_ROOT_OFFSET));
  return (bool)value;
}

void set_node_root(void* node, bool is_root) {
  uint8_t value = is_root;
  *((uint8_t*)(node + IS_ROOT_OFFSET)) = value;
}

uint32_t get_node_max_key(void* node) {
  switch (get_node_type(node)) {
    case NODE_INTERNAL:
      return *internal_node_key(node, *internal_node_num_keys(node) - 1);
    case NODE_LEAF:
      return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
  }
}

uint32_t* node_parent(void* node) { return node + PARENT_POINTER_OFFSET; }

/*
Return the position of the given key.
If the key is not present, return the position
where it should be inserted
*/

uint32_t internal_node_find_child(void* node, uint32_t key) {
  /*
  Return the index of the child which should contain
  the given key.
  */

  uint32_t num_keys = *internal_node_num_keys(node);

  /* Binary search */
  uint32_t min_index = 0;
  uint32_t max_index = num_keys; /* there is one more child than key */

  while (min_index != max_index) {
    uint32_t index = (min_index + max_index) / 2;
    uint32_t key_to_right = *internal_node_key(node, index);
    if (key_to_right >= key) {
      max_index = index;
    } else {
      min_index = index + 1;
    }
  }

  return min_index;
}

void update_internal_node_key(void* node, uint32_t new_key) {
  uint32_t old_child_index = internal_node_find_child(node, new_key);
  *internal_node_key(node, old_child_index) = new_key;
}


Cursor* internal_node_find(Table* table, uint32_t page_num, uint32_t key) {
  int frame = fetch_page(table->pager, page_num);
  void* node = get_frame_offset(table->pager, frame);
  
  uint32_t child_index = internal_node_find_child(node, key);
  uint32_t child_num = *internal_node_child(node, child_index);  

  int child_frame = fetch_page(table->pager, child_num);
  void* child = get_frame_offset(table->pager, child_frame);
  switch (get_node_type(child)) {
    case NODE_LEAF:
      return leaf_node_find(table, child_num, key);
    case NODE_INTERNAL:
      return internal_node_find(table, child_num, key);
  }
}



Cursor* table_find(Table* table, uint32_t key) {
  uint32_t root_page_num = table->table_root_page;
  int frame = fetch_page(table->pager, root_page_num);
  void* root_node = get_frame_offset(table->pager, frame);

  if (get_node_type(root_node) == NODE_LEAF) {
    return leaf_node_find(table, root_page_num, key);
  } else {
    return internal_node_find(table, root_page_num, key);
  }
}

Cursor* leaf_node_find(Table* table, uint32_t page_num, uint32_t key) {
  int frame = fetch_page(table->pager, page_num);
  void* node = get_frame_offset(table->pager, frame);
  uint32_t num_cells = *leaf_node_num_cells(node);

  Cursor* cursor = malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->page_num = page_num;

  // Binary search
  uint32_t min_index = 0;
  uint32_t one_past_max_index = num_cells;
  while (one_past_max_index != min_index) {
    uint32_t index = (min_index + one_past_max_index) / 2;
    uint32_t key_at_index = *leaf_node_key(node, index);
    if (key == key_at_index) {
      cursor->cell_num = index;
      return cursor;
    }
    if (key < key_at_index) {
      one_past_max_index = index;
    } else {
      min_index = index + 1;
    }
  }

  cursor->cell_num = min_index;
  return cursor;
}



void create_new_root(Table* table, uint32_t right_child_page_num) {
  /*
  Handle splitting the root.
  Old root copied to new page, becomes left child.
  Address of right child passed in.
  Re-initialize root page to contain the new root node.
  New root node points to two children.
  */

 int root_frame = fetch_page(table->pager, table->table_root_page);
  void* root = get_frame_offset(table->pager, root_frame);

  int right_child_frame = fetch_page(table->pager, right_child_page_num);
  void* right_child = get_frame_offset(table->pager, right_child_frame);

  int left_child_frame = new_page(table->pager);
  void* left_child = get_frame_offset(table->pager, left_child_frame);

  /* Left child has data copied from old root */
  memcpy(left_child, root, PAGE_SIZE);
  set_node_root(left_child, false);

  /* Root node is a new internal node with one key and two children */
  initialize_internal_node(root);
  set_node_root(root, true);
  *internal_node_num_keys(root) = 1;
  *internal_node_child(root, 0) = table->pager->frame_data[left_child_frame].page_number;
  uint32_t left_child_max_key = get_node_max_key(left_child);
  *internal_node_key(root, 0) = left_child_max_key;
  *internal_node_right_child(root) = right_child_page_num;
  *node_parent(left_child) = table->table_root_page;
  *node_parent(right_child) = table->table_root_page;

  // mark newly created page as dirty
  table->pager->frame_data[left_child_frame].dirty = 1;

}

void internal_node_insert(Table* table, uint32_t parent_page_num,
                          uint32_t child_page_num) {
  /*
  Add a new child/key pair to parent that corresponds to child
  */

  int parent_frame = fetch_page(table->pager, parent_page_num);
  void* parent = get_frame_offset(table->pager, parent_frame);
  int child_frame = fetch_page(table->pager, child_page_num);
  void* child = get_frame_offset(table->pager, child_frame);
  uint32_t child_max_key = get_node_max_key(child);
  uint32_t index = internal_node_find_child(parent, child_max_key);

  uint32_t original_num_keys = *internal_node_num_keys(parent);
  *internal_node_num_keys(parent) = original_num_keys + 1;

  if (original_num_keys >= INTERNAL_NODE_MAX_CELLS) {
    printf("Need to implement splitting internal node\n");
    close_db(table->pager);
  }

  uint32_t right_child_page_num = *internal_node_right_child(parent);
  int right_child_frame = fetch_page(table->pager, right_child_page_num);
  void* right_child = get_frame_offset(table->pager, right_child_frame);

  if (child_max_key > get_node_max_key(right_child)) {
    /* Replace right child */
    *internal_node_child(parent, original_num_keys) = right_child_page_num;
    *internal_node_key(parent, original_num_keys) =
        get_node_max_key(right_child);
    *internal_node_right_child(parent) = child_page_num;
  } else {
    /* Make room for the new cell */
    for (uint32_t i = original_num_keys; i > index; i--) {
      void* destination = internal_node_cell(parent, i);
      void* source = internal_node_cell(parent, i - 1);
      memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
    }
    *internal_node_child(parent, index) = child_page_num;
    *internal_node_key(parent, index) = child_max_key;
  }

  table->pager->frame_data[parent_frame].dirty = 1;
}

void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, void* record) {
  /*
  Create a new node and the new row to it.
  Update parent or create a new parent.
  */
 

 int old_node_frame = fetch_page(cursor->table->pager, cursor->page_num);
  void* old_node = get_frame_offset(cursor->table->pager, old_node_frame);
  uint32_t new_node_frame = new_page(cursor->table->pager);
  void* new_node = get_frame_offset(cursor->table->pager, new_node_frame);
  initialize_leaf_node(new_node);
  *node_parent(new_node) = *node_parent(old_node);
  *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node); // no need, delete later
  *leaf_node_next_leaf(old_node) = cursor->table->pager->frame_data[new_node_frame].page_number;  


  // add the new row to the beginning of the newly created node.
  *(leaf_node_key(new_node, 0)) = key;
  memcpy(leaf_node_value(new_node, 0), record, LEAF_NODE_VALUE_SIZE);
  *(leaf_node_num_cells(new_node)) += 1;

  if (is_node_root(old_node)) {
      create_new_root(cursor->table, 
      cursor->table->pager->frame_data[new_node_frame].page_number);
  } else {
      uint32_t parent_page_num = *node_parent(old_node);
      uint32_t new_max = get_node_max_key(old_node);
      int parent_frame = fetch_page(cursor->table->pager, parent_page_num);
      void* parent = get_frame_offset(cursor->table->pager, parent_frame);

      update_internal_node_key(parent, new_max);
      internal_node_insert(cursor->table, parent_page_num, cursor->table->pager->frame_data[new_node_frame].page_number);
  }

  cursor->table->pager->frame_data[old_node_frame].dirty = 1;
  cursor->table->pager->frame_data[new_node_frame].dirty = 1;

}

void leaf_node_insert(Cursor* cursor, uint32_t key, void* record) {
  int frame = fetch_page(cursor->table->pager, cursor->page_num);
  void* node = get_frame_offset(cursor->table->pager, frame);

  uint32_t num_cells = *leaf_node_num_cells(node);
  if (num_cells >= LEAF_NODE_MAX_CELLS) {
    leaf_node_split_and_insert(cursor, key, record);
    return;
  }

  if (cursor->cell_num < num_cells) {
    // Make room for new cell
    for (uint32_t i = num_cells; i > cursor->cell_num; i--) {
      memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1),
             LEAF_NODE_CELL_SIZE);
    }
  }

  *(leaf_node_num_cells(node)) += 1;
  *(leaf_node_key(node, cursor->cell_num)) = key;
  memcpy(leaf_node_value(node, cursor->cell_num), record, LEAF_NODE_VALUE_SIZE);

  cursor->table->pager->frame_data[frame].dirty = 1;
  
}




/* A function for printing the tree structure */

void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) {
    printf("  ");
  }
}

void print_tree(struct Pager* pager, uint32_t page_num, uint32_t indentation_level) {
  int frame = fetch_page(pager, page_num);
  void* node = get_frame_offset(pager, frame);
  uint32_t num_keys, child;

  switch (get_node_type(node)) {
    case (NODE_LEAF):
      num_keys = *leaf_node_num_cells(node);
      indent(indentation_level);
      printf("- leaf (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        indent(indentation_level + 1);
        printf("- %d\n", *leaf_node_key(node, i));
      }
      break;
    case (NODE_INTERNAL):
      num_keys = *internal_node_num_keys(node);
      indent(indentation_level);
      printf("- internal (size %d)\n", num_keys);
      for (uint32_t i = 0; i < num_keys; i++) {
        child = *internal_node_child(node, i);
        print_tree(pager, child, indentation_level + 1);

        indent(indentation_level + 1);
        printf("- key %d\n", *internal_node_key(node, i));
      }
      child = *internal_node_right_child(node);
      print_tree(pager, child, indentation_level + 1);
      break;
  }
}



Cursor* table_start(Table* table) {
  Cursor* cursor =  table_find(table, 0);  

  int frame = fetch_page(table->pager, cursor->page_num);
  void* node = get_frame_offset(table->pager, frame);

  uint32_t num_cells = *leaf_node_num_cells(node);
  cursor->end_of_table = (num_cells == 0);

  return cursor;
}

void cursor_advance(Cursor* cursor) {
  int frame = fetch_page(cursor->table->pager, cursor->page_num);
  void* node = get_frame_offset(cursor->table->pager, frame);

  cursor->cell_num += 1;
  if (cursor->cell_num >= (*leaf_node_num_cells(node))) {
    /* Advance to next leaf node */
    uint32_t next_page_num = *leaf_node_next_leaf(node);
    if (next_page_num == 0) {
      /* This was rightmost leaf */
      cursor->end_of_table = true;
    } else {
      cursor->page_num = next_page_num;
      cursor->cell_num = 0;
    }
  }
}

void* cursor_value(Cursor* cursor) {
  int frame = fetch_page(cursor->table->pager, cursor->page_num);
  void* page = get_frame_offset(cursor->table->pager, frame);
  return leaf_node_cell(page, cursor->cell_num);
}