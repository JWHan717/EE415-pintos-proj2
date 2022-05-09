#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdbool.h>
#include "debug.h"
#include "hash.h"
#include "filesys/file.h"

enum vme_type
  {
    VM_BIN,     
    VM_ANON,
    VM_FILE
  };

struct vm_entry {
    struct hash_elem hashelem;
    struct file *f;
    enum vme_type type;
    uint8_t *vaddr;
    uint32_t zero_bytes;
    uint32_t read_bytes;
    off_t offset;
    bool writable;
};
/*
struct page{
    uint8_t *kaddr;             // physical address of page 
    struct vm_entry *vme;       // reference to the virtual page object 
    struct thread *t;           // Reference to the thread to which it belongs 
    struct list_elem lru_elem;
};*/

void vm_init(struct hash *);
void vm_destroy(struct hash *);
struct vm_entry *find_vme(void *);

bool insert_vme(struct hash *, struct vm_entry *);
bool delete_vme(struct hash *, struct vm_entry *);

static unsigned vm_hash_func(const struct hash_elem *, void *UNUSED);
static bool vm_less_func(const struct hash_elem *, const struct hash_elem *, void *UNUSED);
static void vm_destroy_func(struct hash_elem *, void *UNUSED);

bool load_file (void *kaddr, struct vm_entry *vme);

#endif /* vm/page.h */