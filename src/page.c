#include <stdio.h>
#include <stdlib.h>
#include "page.h"

extern char _end_kernel;

struct ppage physical_page_array[128]; // 128 pages(nodes), each 2mb in length 
struct ppage *head = NULL;

void init_pfa_list(void) {
  struct ppage *ptr = NULL;
  head = &physical_page_array[0];
   for (int k = 0; k < 128; k++) {
     ptr = &physical_page_array[k];
     ptr->physical_addr = &_end_kernel;

     if (k > 0) {
       ptr->prev = &physical_page_array[k - 1];
   } else {
     ptr->prev = NULL;
  }
    if (k < 127) {
      ptr->next = &physical_page_array[k + 1];
  } else {
   ptr->next = NULL;
  }
 }
}

struct ppage *allocate_physical_pages(unsigned int npages) {
     if (npages && head == 0) {
        return NULL; 
  }
 struct ppage *allocd_list = head; 
 struct ppage *ptr = head;
 struct ppage *tail = NULL;
 unsigned int count = 0;
    while (count < npages) {
         ptr = ptr->next;
         tail = ptr;
         count++;
  }
  head = ptr;
  if (head != NULL) {
     head->prev = NULL;
  }

  if (tail != NULL) {
    tail->next = NULL;
  }

  return allocd_list;
}

void free_physical_pages(struct ppage *ppage_list) {
    if (ppage_list == NULL) 
         return;
    struct ppage *tail = ppage_list;
    while (tail->next != NULL) 
         tail = tail->next; 
     
     tail->next = head;
     if(head != NULL)
       head->prev = tail;
    
     head = ppage_list;
     head->prev = NULL;  

}
