#ifndef _PAGER_
#define _PAGER_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "btree.h"
#include "catalog.h"

#define PAGER_MAXIMUM_FRAMES 100
#define PAGE_SIZE 4096



typedef struct FrameData {
    char dirty; /* 0 if false, 1 if true */
    char pinned; /* 0 if false, 1 if true */
    char referenced; /* 1 if there's a valid entry in page table refers to it, 0 if not */
    unsigned char page_number; /* page number that's in the frame */
} FrameData;

typedef struct Pager
{
    int fd;
    uint32_t num_pages;
    uint32_t file_length;
    FrameData frame_data[PAGER_MAXIMUM_FRAMES];
    char* page_table;
    uint32_t page_table_size;
    void* buffer_pool;
    // delete num_rows later after implementing the table data structure
    uint32_t num_rows;

} Pager;


/* function */
Pager* create_pager(char* filename);
void destroy_pager(Pager* pager);
/* return number of the frame where the page resides */
int fetch_page(Pager* pager, uint32_t page_num);
void flush_frame(Pager* pager, unsigned char frame_num);
void flush_all_pages(Pager* pager);
/* returns number of newly create page */
int new_page(Pager* pager);

void close_db(Pager* pager);

int search_for_empty_frame(Pager* pager);
void* get_frame_offset(Pager* pager, unsigned char frame_num);
int search_and_pin(Pager* pager, uint32_t page_num);
void initialize_pagetable(Pager* pager);
void initialize_frame_data(Pager* pager);


#endif
