#include <stdio.h>
#include <stdbool.h>
#include "debug.h"
#include "vm/page.h"
#include "hash.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "userprog/pagedir.h"
#include "filesys/file.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"

/* List of physical pages allocated to user process */
static struct list lru_list;
static struct list_elem *lru_ptr;

static struct lock frame_lock;

static struct page *select_victim(uint32_t *pagedir);

void lru_init(void){
  list_init(&lru_list);
  lock_init(&frame_lock);
  lru_ptr = NULL;
}

void insert_lru(struct page *page){
  if (lru_ptr == NULL) lru_ptr = &page->lru_elem;
  list_push_back(&lru_list, &page->lru_elem);
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
  lock_acquire(&frame_lock);

  uint8_t *kpage = palloc_get_page (PAL_USER);
  while (kpage == NULL) {
    struct page *victim = select_victim(&lru_list);
    // pagedir_clear_page(victim->t->pagedir, victim->kaddr);

    size_t swap_idx = swap_out(victim);
    vme->swap_idx = swap_idx;

    kpage = palloc_get_page(PAL_USER);
  }

  struct page *p = malloc(sizeof(struct page));
  if (p == NULL) {
    lock_release(&frame_lock);
    return NULL;
  }
  p->kaddr = kpage;
  p->t = thread_current();
  p->vme = vme;
  p->pinned = false;
  insert_lru(p);

  lock_release(&frame_lock);
  return p;
}

void *lru_free_page(struct page *p){
    //remove_lru(p);
    palloc_free_page(p->kaddr);
    p->vme->type = VM_ANON;
}

static struct page *select_victim(uint32_t *pagedir) {
  /* iterate through lru_list, and find/return a page where
  pinned == false and pagedir_is_accessed() == false */
  for (int i=0; i < 2*(list_size(&lru_list)); i++) {
    lru_ptr = (lru_ptr = list_end(&lru_list)) ? list_head(&lru_list) : list_next(lru_ptr);
    struct page *page = list_entry(lru_ptr, struct page, lru_elem);

    if(page->pinned) {
      continue;
    } else if (pagedir_is_accessed(pagedir, page->kaddr)) {
      pagedir_set_accessed(pagedir, page->kaddr, false);
      continue;
    } 
    return page;
  }
  return list_entry(list_next(lru_ptr), struct page, lru_elem);
  exit(-1); /* No page to evict = no memory */
}

// static struct page *select_victim(void) {
//   return list_entry(list_front(&lru_list), struct page, lru_elem);
// }
