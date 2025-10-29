
#include <stdlib.h>
#include <stdint.h>
#include "page.h"

extern char _end_kernel;
struct ppage physical_page_array[128]; // 128 pages(nodes), each 2mb in length 
struct ppage *head = NULL;

struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
struct page pt[1024] __attribute__((aligned(4096)));

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

void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd) {
  struct ppage *currentpage = pglist; 

while (currentpage != NULL) {
  uint32_t pdindex = ((uint32_t)vaddr >> 22) & 0x03FF;
  uint32_t ptindex = (uint32_t)vaddr >> 12 & 0x03FF;

  if (pd[pdindex].present == 0) {
     pd[pdindex].present = 1;
     pd[pdindex].rw = 1;
     pd[pdindex].user = 1;
     pd[pdindex].writethru = 1;
     pd[pdindex].cachedisabled = 1;
     pd[pdindex].accessed = 1;
     pd[pdindex].pagesize = 1;
     pd[pdindex].ignored = 2;
     pd[pdindex].os_specific = 3;
     pd[pdindex].frame = ((uint32_t)pt) >> 12;
  }
  pt[ptindex].present = 1;
  pt[ptindex].rw = 1;
  pt[ptindex].user = 1;
  pt[ptindex].accessed = 1;
  pt[ptindex].dirty = 1;
  pt[ptindex].unused = 7;
  pt[ptindex].frame = ((uint32_t)currentpage->physical_addr) >> 12;

  vaddr = (vaddr + 4096);
  currentpage = currentpage->next; 
}
  return vaddr;
}

void loadPageDirectory(struct page_directory_entry *pd) {
    asm("mov %0,%%cr3"
        :
        : "r"(pd)
        :);
}

  void enablePaging() {
    asm("mov %cr0, %eax\n"
        "or $0x80000001, %eax\n"
        "mov %eax,%cr0");
    }




