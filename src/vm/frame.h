#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include "debug.h"
#include "hash.h"
#include "filesys/file.h"
#include "page.h"

struct page{
    uint8_t *kaddr;             /* physical address of page */
    struct vm_entry *vme;       /* reference to the virtual page object */
    struct thread *t;           /* Reference to the thread to which it belongs */
    struct list_elem lru_elem;
};

void lru_init();
void lru_destroy();
void insert_lru(struct list_elem *);
void remove_lru(struct list_elem *);
struct page *find_lru(uint8_t *);

struct page *lru_get_page(struct vm_entry *);
void *lru_free_page(uint8_t *);

#endif /* vm/frame.h */