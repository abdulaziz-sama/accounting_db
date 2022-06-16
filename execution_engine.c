#include "execution_engine.h"


void execute_append(Q_Node* root, Pager* pager){

    // check if the table exists
    int catalog_frame = fetch_page(pager, 0);
    void* catalog = get_frame_offset(pager, catalog_frame);

    // result[0] is table place, result[1] is table root page num, result[2] is row ID value
    uint32_t* result = search_for_table(catalog, root->node.q1.table_name->name);

    if(!result){
        printf("Table (%s) doesn't exits. Create table before usage.\n",
        root->node.q1.table_name->name);
        return;
    }


    int root_frame;
    void* node = NULL;

    if(result[1] == 0){
        // first time inserting into table, create a root page for it.
        int new_page_number = pager->num_pages;
        root_frame = fetch_page(pager, new_page_number);
        node = get_frame_offset(pager, root_frame);
        initialize_leaf_node(node);
        set_table_root_page_num(catalog, new_page_number, result[0]);
    } else {
        root_frame = fetch_page(pager, result[1]);
        node = get_frame_offset(pager, root_frame);
    }

    
    void* record = malloc(70);
    strncpy(record, root->node.q1.debit_account->name, 31);
    memcpy(record + DEBIT_ACCOUNT_VALUE_OFFSET, root->node.q1.debit_value->value, 4);
    strncpy(record + CREDIT_ACCOUNT_OFFSET, root->node.q1.credit_account->name, 31);
    memcpy(record + CREDIT_ACCOUNT_VALUE_OFFSET, root->node.q1.credit_value->value, 4);


    uint32_t* row_number = leaf_node_num_cells(node);
    if(*row_number >= LEAF_NODE_MAX_CELLS){
        printf("need to implement splitting node\n");
        close_db(pager);
    }


    memcpy(leaf_node_cell(node, *row_number), &result[2], sizeof(uint32_t));
    memcpy(leaf_node_value(node, *row_number), record, 70);

    // increment the number of cells count in leaf node
    *leaf_node_num_cells(node) += 1;

    pager->frame_data[root_frame].dirty = 1;

    // increment the row ID for table in the catalog
    set_table_record_id(catalog, result[2] + 1, result[0]);

    pager->frame_data[catalog_frame].dirty = 1;
    
}


void execute_select(Q_Node* root, Pager* pager){

    // check if the table exists
    int catalog_frame = fetch_page(pager, 0);
    void* catalog = get_frame_offset(pager, catalog_frame);

    // result[0] is table place, result[1] is table root page num, result[2] is row ID value
    uint32_t* result = search_for_table(catalog, root->node.q2.table_name->name);


    if(!result){
        printf("Table (%s) doesn't exits. Create table before usage.\n",
        root->node.q2.table_name->name);
        return;
    }

    if(result[1] == 0){
        printf("table is in catalog, but empty and has no root page.\n");
        return;
    }

    int root_frame = fetch_page(pager, result[1]);
    void* node = get_frame_offset(pager, root_frame);
    
    uint32_t num_cells = *leaf_node_num_cells(node);

    printf("number of cells: %u\n", num_cells);

    char* debit_acc, *credit_acc;
    uint32_t* debit_val, *credit_val;

    for(uint32_t i=0; i< num_cells; i++){
        printf("Key: %u\n", *leaf_node_key(node, i));
        debit_acc = (char*)leaf_node_value(node, i);
        debit_val = leaf_node_value(node, i) + DEBIT_ACCOUNT_VALUE_OFFSET;
        credit_acc = (char*)(leaf_node_value(node, i) + CREDIT_ACCOUNT_OFFSET);
        credit_val = leaf_node_value(node, i) + CREDIT_ACCOUNT_VALUE_OFFSET;
        printf("{%s:%u, %s:%u}\n", debit_acc, *debit_val, credit_acc, *credit_val);
    }
    
}

void execute_create(Q_Node* root, Pager* pager){

    int frame = fetch_page(pager, 0);
    void* catalog = get_frame_offset(pager, frame);

    uint32_t num_of_tables = *get_num_of_tables(catalog);
    if(num_of_tables >= CATALOG_MAX_TABLES){
        printf("Maximum number of tables reached, can't create anymore.\n");
        return;
    }

    set_table_name(catalog, root->node.q3.table_name->name, num_of_tables);
    set_table_record_id(catalog, 0, num_of_tables);

    /*
    zero is not a valid root page number of tables since it's perserved for the
    catalog, it has to be changed once an append is executed on the table for the
    first time and a new root page is created for it.
    */

   set_table_root_page_num(catalog, 0, num_of_tables);

   num_of_tables++;

   set_num_of_tables(catalog, num_of_tables);

   pager->frame_data[frame].dirty = 1;

}

void execute(Q_Node* root, Pager* pager){
    switch (root->tag)
    {
    case 0:
        execute_append(root, pager);
        break;
    case 1:
        execute_select(root, pager);
        break;
    case 2:
        execute_create(root, pager);
        break;
    }
}