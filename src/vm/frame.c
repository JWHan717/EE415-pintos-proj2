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
static struct list_elem *lru_ptr;

static struct lock frame_lock;

static struct page *select_victim(uint32_t *pagedir);

void lru_init(){
  list_init(&lru_list);
  lock_init(&frame_lock);
  lru_ptr = NULL;
}

void lru_destroy(){
  
}
void insert_lru(struct page *page){
    list_push_back(&lru_list, &page->lru_elem);
}

void remove_lru( struct page *page){
    lock_acquire(&frame_lock);
    lru_free_page(page);
    lock_release(&frame_lock);
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
    if (victim->pinned) {
      continue;
    }
    pagedir_clear_page(victim->t->pagedir, victim->kaddr);
    bool dirty = pagedir_is_dirty(victim->t->pagedir, victim->kaddr);

    size_t swap_idx = swap_out(victim);

    kpage = palloc_get_page(PAL_USER);
    if (kpage == NULL) exit(-1);
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
  insert_lru(&p->lru_elem);

  lock_release(&frame_lock);
  return p;
}

void *lru_free_page(struct page *p){
    remove_lru(&p->lru_elem);
    palloc_free_page(p->kaddr);
    free(p);
}

static struct page *select_victim(uint32_t *pagedir) {
  /* iterate through lru_list, and find/return a page where
  pinned == false and pagedir_is_dirty() == true */
  return NULL;
}

// static struct page *select_victim(void) {
//   return list_entry(list_front(&lru_list), struct page, lru_elem);
// }
