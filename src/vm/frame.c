#include <stdio.h>
#include <stdbool.h>
#include "debug.h"
#include "vm/page.h"
#include "hash.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"

/* List of physical pages allocated to user process */
static struct list lru_list;

void lru_init(){
  list_init(&lru_list);
}
void lru_destroy(){
  
}
void insert_lru(struct list_elem *e){
    list_push_back(&lru_list,e);
}

void remove_lru( struct list_elem *e){
    list_remove(e);
}

struct page *find_lru(uint8_t *kpage){
    struct list_elem *e;
    if (!list_empty(&lru_list)){
        for (e = list_begin (&lru_list); e != list_end (&lru_list);
        e = list_next (e))
        {
            struct page *p = list_entry (e, struct page, lru_elem);
            if (p->kaddr == kpage) return p;
        }
    }
    return NULL;
}

struct page *lru_get_page(struct vm_entry *vme){
  uint8_t *kpage = palloc_get_page (PAL_USER);
  while (kpage == NULL) {
    struct page *victim;
    victim = select_victim(&lru_list);
    swap_out(victim);
    kpage = palloc_get_page(PAL_USER);
  }
  struct page *p;
  p->kaddr = kpage;
  p->t = thread_current();
  p->vme = vme;
  insert_lru(&p->lru_elem);

  return p;
}

void *lru_free_page(uint8_t *kpage){
    struct page *p = find_lru(kpage);
    remove_lru(&p->lru_elem);
    palloc_free_page(kpage);
    free(p);
}

