#ifndef _CATALOG_
#define _CATALOG_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "pager.h"


#define NUMBER_OF_TABLES_SIZE sizeof(uint32_t)
#define CATALOG_HEADER_SIZE (NUMBER_OF_TABLES_SIZE)

#define CATALOG_BODY_OFFSET CATALOG_HEADER_SIZE
#define TABLE_NAME_SIZE 31
#define TABLE_NAME_OFFSET 0
#define TABLE_ROOT_PAGE_SIZE sizeof(uint32_t)
#define TABLE_ROOT_PAGE_OFFSET (TABLE_NAME_OFFSET + TABLE_NAME_SIZE)
#define TABLE_RECORD_ID_SIZE sizeof(uint32_t)
#define TABLE_RECORD_ID_OFFSET (TABLE_ROOT_PAGE_OFFSET + TABLE_ROOT_PAGE_SIZE)

#define TABLE_DATA_SIZE (TABLE_NAME_SIZE + TABLE_ROOT_PAGE_SIZE + TABLE_RECORD_ID_SIZE)
#define SPACE_FOR_TABLE_DATA (PAGE_SIZE - CATALOG_HEADER_SIZE)
#define CATALOG_MAX_TABLES (SPACE_FOR_TABLE_DATA / TABLE_DATA_SIZE)

void* initialize_catalog(void* node);

uint32_t* get_num_of_tables(void* node);
void set_num_of_tables(void* node, uint32_t num);

char* get_table_name(void* node, uint32_t table_num);
void set_table_name(void* node, char* table_name, uint32_t table_num);

uint32_t* get_table_root_page_num(void* node, uint32_t table_num);
void set_table_root_page_num(void* node, uint32_t root_page_num, uint32_t table_num);

uint32_t* get_table_record_id(void* node, uint32_t table_num);
void set_table_record_id(void* node, uint32_t id_num, uint32_t table_num);

uint32_t* search_for_table(void* node, char* table_name);

#endif