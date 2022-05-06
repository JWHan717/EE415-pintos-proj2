#include <stdio.h>
#include <stdbool.h>
#include "debug.h"
#include "page.h"
#include "hash.h"
#include "palloc.h"
#include "vaddr.h"

static bool setup_stack (void **esp) 
{
  uint8_t *kpage;
  bool success = false;

  kpage = palloc_get_page (PAL_USER | PAL_ZERO);
  if (kpage != NULL) 
    {
      success = install_page (((uint8_t *) PHYS_BASE) - PGSIZE, kpage, true);
      if (success)
        *esp = PHYS_BASE;
      else
        palloc_free_page (kpage);
    }
  /* Create vm_entry */
  /* Set up vm_entry members */
  /* Using insert_vme(), add vm_entry to hash table */
  return success;
}

/* Using file_read_at(), write physical memory as much as read_bytes.
Return file_read_at status, pad as much as zero_bytes.
If file is loaded to memory, return true. */
bool load_file (void *kaddr, struct vm_entry *vme) {
    /* Load page in disk to physical memory */
    /* Load a page to kaddr by <file, offset> of vme */
    /* If fail to write all 4KB, fill the rest with zeros. */
}

/* Hash table initialization. */
void vm_init (struct hash *vm) {

}

/* Delete hash table. */
void vm_destroy (struct hash *vm) {

}

struct vm_entry *find_vme (void *vaddr) {

}

bool insert_vme (struct hash *vm, struct vm_entry *vme) {

}

bool delete_vme (struct hash *vm, struct vm_entry *vme) {

}

static unsigned vm_hash_func (const struct hash_elem *e, void *aux UNUSED) {

}

static bool vm_less_func (const struct hash_elem *a,
                        const struct hash_elem *b, void *aux UNUSED) {

}

static void vm_destroy_func(struct hash_elem *e, void *aux UNUSED) {

}