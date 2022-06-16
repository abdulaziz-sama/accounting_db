#include "catalog.h"

void* initialize_catalog(void* node){
    set_num_of_tables(node, 0);
}

uint32_t* get_num_of_tables(void* node){
    return node;
}

void set_num_of_tables(void* node, uint32_t num){
    *get_num_of_tables(node) = num;
}

char* get_table_name(void* node, uint32_t table_num){
    return node + CATALOG_BODY_OFFSET + (TABLE_DATA_SIZE * table_num);
}

void set_table_name(void* node, char* table_name, uint32_t table_num){
    memcpy(get_table_name(node, table_num), table_name, TABLE_NAME_SIZE);
}

uint32_t* get_table_root_page_num(void* node, uint32_t table_num){
    return node + CATALOG_BODY_OFFSET + (TABLE_DATA_SIZE * table_num) 
        + TABLE_ROOT_PAGE_OFFSET;
}

void set_table_root_page_num(void* node, uint32_t root_page_num, uint32_t table_num){
    *get_table_root_page_num(node, table_num) = root_page_num;
}

uint32_t* get_table_record_id(void* node, uint32_t table_num){
    return node + CATALOG_BODY_OFFSET + (TABLE_DATA_SIZE * table_num) 
        + TABLE_RECORD_ID_OFFSET;
}

void set_table_record_id(void* node, uint32_t id_num, uint32_t table_num){
    *get_table_record_id(node, table_num) = id_num;
}

/*
if the table exists, it will return a pointer to an array consisting of
the table place, the root page number of the table and the ID value for rows.
*/

uint32_t* search_for_table(void* node, char* table_name){
    uint32_t num_of_tables = *get_num_of_tables(node);

    for(uint32_t i=0; i < num_of_tables; i++){
        if(strcmp(table_name, get_table_name(node, i)) == 0){
            uint32_t* result = malloc(sizeof(uint32_t) * 3);
            result[0] = i;
            result[1] = *get_table_root_page_num(node, i);
            result[2] = *get_table_record_id(node, i);
            return result;
        }
    }

    return NULL;
}