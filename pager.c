#include "pager.h"
#include "errno.h"
#include "fcntl.h"
#include "unistd.h"

Pager* create_pager(char* filename){
  
    
  int fd = open(filename,
                O_RDWR |      // Read/Write mode
                    O_CREAT,  // Create file if it does not exist
                S_IWUSR |     // User write permission
                    S_IRUSR   // User read permission
                );

  if (fd == -1) {
    printf("Failed to open file %s\n", filename);
    exit(EXIT_FAILURE);
  }

  off_t file_length = lseek(fd, 0, SEEK_END); 

  Pager* pager = malloc(sizeof(Pager));
  pager->fd = fd; 
  pager->file_length = file_length;
  pager->num_pages = (file_length / PAGE_SIZE);
  pager->buffer_pool = malloc(PAGE_SIZE * PAGER_MAXIMUM_FRAMES);

  if(pager->num_pages == 0){
    // new database file, create catalog for it
    void* node = malloc(PAGE_SIZE);
    initialize_catalog(node);

    // write catalog at the beginning of file (it will be at page 0)
    off_t offset = lseek(fd, 0, SEEK_SET);
    if(offset == -1){
      printf("Error seeking: %d\n", errno);
      exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(fd, node, PAGE_SIZE);
    if(bytes_written == -1){
      printf("Error writing: %d\n", errno);
      exit(EXIT_FAILURE);
    }

    pager->num_pages += 1;

  }

  /* malloc twice the size of existing pages */
  if(pager->num_pages > 5){
    char* page_table = malloc((sizeof(char) * pager->num_pages) * 2);
    pager->page_table_size = pager->num_pages * 2; 
    pager->page_table = page_table;       
  } else {
    /* else malloc 10 entries */
    char* page_table = malloc(sizeof(char) * 10);
    pager->page_table_size = 10;    
    pager->page_table = page_table;
  }
  initialize_pagetable(pager);
  initialize_frame_data(pager);

  return pager;

}

/* if the entry corresponding to the page_num has a value > 0 it means the page is in the frame
that corresponds to the same value in page_table[page_num], then pin and return the frame */
int search_and_pin(Pager* pager, uint32_t page_num){

  if(pager->page_table[page_num] >= 0){
    printf("implement pinning and referencing here.\n");
    return pager->page_table[page_num];
  }
  else {
    return -1;
  }
}


int search_for_empty_frame(Pager* pager){
  for(unsigned char i = 0; i < PAGER_MAXIMUM_FRAMES; i++){
    if(pager->frame_data[i].pinned == 0){
      if(pager->frame_data[i].dirty == 1){
        flush_frame(pager, i);
      }
      if(pager->frame_data[i].referenced == 1){
        /* old entry will have invalid reference */
        pager->page_table[pager->frame_data[i].page_number] = -1;
      }

      return i;
    }
  }

  return -1;
}


void* get_frame_offset(Pager* pager, unsigned char frame_num){
  return pager->buffer_pool + frame_num * PAGE_SIZE;
}


int new_page(Pager* pager){
  if(pager->num_pages+1 >= pager->page_table_size){
    printf("implement resizing page table");
  }

  uint32_t page_num = pager->num_pages;
  pager->num_pages = pager->num_pages + 1;

  int empty_frame = search_for_empty_frame(pager);
  if(empty_frame == -1){
    printf("Buffer pool is full.\n");
    pager->num_pages -= 1;
    return false;
  }


  pager->page_table[page_num] = empty_frame;
  pager->frame_data[empty_frame].page_number = page_num;
  pager->frame_data[empty_frame].pinned = 1;
  pager->frame_data[empty_frame].referenced = 1;
  pager->frame_data[empty_frame].dirty = 0;

  return empty_frame;

}


int fetch_page(Pager* pager, uint32_t page_num){

  if(page_num >= pager->num_pages){
    return new_page(pager);    
  }

  int found_frame = search_and_pin(pager, page_num);
  if(found_frame != -1){
    return found_frame;
  }
  
  int empty_frame = search_for_empty_frame(pager);
  if(empty_frame == -1){
    printf("Buffer pool is full.\n");
    return -1;
  }

  pager->page_table[page_num] = empty_frame;
  pager->frame_data[empty_frame].page_number = page_num;
  pager->frame_data[empty_frame].pinned = 1;
  pager->frame_data[empty_frame].referenced = 1;
  pager->frame_data[empty_frame].dirty = 0;

  lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
  ssize_t bytes_read = read(pager->fd, get_frame_offset(pager, empty_frame), PAGE_SIZE);
  if (bytes_read == -1) {
    printf("Error reading file: %d\n", errno);
    exit(EXIT_FAILURE);
  }
  
  return empty_frame;
}

void flush_frame(Pager* pager, unsigned char frame_num){
  off_t offset = lseek(pager->fd, pager->frame_data[frame_num].page_number * PAGE_SIZE, SEEK_SET);
  if(offset == -1){
    printf("Error seeking: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_written = write(pager->fd, get_frame_offset(pager, frame_num), PAGE_SIZE);
  if(bytes_written == -1){
    printf("Error writing: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

void flush_all_pages(Pager* pager){
  for(unsigned char i=0; i < PAGER_MAXIMUM_FRAMES; i++){
    if(pager->frame_data[i].dirty){
      flush_frame(pager, i);
    }
  }
}

void destroy_pager(Pager* pager){

  int result = close(pager->fd);
  if (result == -1) {
    printf("Failed to open file.\n");
    exit(EXIT_FAILURE);
  }

  free(pager->buffer_pool);
  free(pager->page_table);
  free(pager);

}

void initialize_pagetable(Pager* pager){
  for(uint32_t i = 0; i < pager->page_table_size; i++){
    pager->page_table[i] = -1;
  }
}

void initialize_frame_data(Pager* pager){
  for(int i = 0; i < PAGER_MAXIMUM_FRAMES; i++){
    pager->frame_data[i].dirty = 0;
    pager->frame_data[i].pinned = 0;
    pager->frame_data[i].referenced = 0;
    pager->frame_data[i].page_number = -1;
  }
}

void close_db(Pager* pager){
  flush_all_pages(pager);
  destroy_pager(pager);
  exit(EXIT_SUCCESS);
}