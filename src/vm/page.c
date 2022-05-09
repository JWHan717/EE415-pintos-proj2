#include <stdio.h>
#include <stdbool.h>
#include "debug.h"
#include "vm/page.h"
#include "hash.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "filesys/file.h"

/* List of physical pages allocated to user process */
static struct list lru_list;

/* Using file_read_at(), write physical memory as much as read_bytes.
Return file_read_at status, pad as much as zero_bytes.
If file is loaded to memory, return true. */
bool load_file (void *kaddr, struct vm_entry *vme) {
    /* Load page in disk to physical memory */
    /* Load a page to kaddr by <file, offset> of vme */
    ASSERT(vme->f != NULL);
    //int read_bytes = file_read_at(vme->f, kaddr, vme->read_bytes, vme->offset);
    //if (read_bytes != (int)vme->read_bytes) return false;
    file_seek (vme->f, vme->offset);
    if (file_read (vme->f, kaddr, vme->read_bytes) != (int) vme->read_bytes)
       {
         //palloc_free_page (kaddr);
         return false; 
       }

    /* If fail to write all 4KB, fill the rest with zeros. */
    memset(kaddr + vme->read_bytes, 0, vme->zero_bytes);

    return true;
}

/* Hash table initialization. */
void vm_init (struct hash *vm) {
  hash_init(vm, vm_hash_func, vm_less_func, NULL);
}

/* Delete hash table. */
void vm_destroy (struct hash *vm) {
  hash_destroy(vm, vm_destroy_func);
}

/* Search vm_entry corresponding to vaddr 
  in the address space of the current process */
struct vm_entry *find_vme (void *vaddr) {
  struct hash_iterator i;

  hash_first (&i, &thread_current()->vm);
  while (hash_next (&i))
    {
      struct vm_entry *vme = hash_entry (hash_cur (&i), struct vm_entry, hashelem);
      // printf("%x, %x\n", vme->vaddr, vaddr);
      if (vme->vaddr == vaddr) return vme;
    }

  return NULL;
}

/* Insert vm_entry to hash table*/
bool insert_vme (struct hash *vm, struct vm_entry *vme) {
  struct hash_elem * h;
  h = hash_insert(vm, &vme->hashelem);
  return h == NULL;
}

/* Delete vm_entry from hash table */
bool delete_vme (struct hash *vm, struct vm_entry *vme) {
  struct hash_elem * h;
  h = hash_delete(vm, &vme->hashelem);
  return h != NULL;
}

/* Calculate where to put the vm_entry into the hash table */
static unsigned vm_hash_func (const struct hash_elem *e, void *aux UNUSED) {
  struct vm_entry *vme = hash_entry(e,struct vm_entry, hashelem);
  return hash_int((int)(vme->vaddr));
}

/* Compare address values of two entered hash_elem */
static bool vm_less_func (const struct hash_elem *a,
                        const struct hash_elem *b, void *aux UNUSED) {
    struct vm_entry *vme_a = hash_entry (a, struct vm_entry, hashelem);
    struct vm_entry *vme_b = hash_entry (b, struct vm_entry, hashelem);
    
    return (vme_a->vaddr < vme_b->vaddr);
}

/* Remove memory of vm_entry */
static void vm_destroy_func(struct hash_elem *e, void *aux UNUSED) {
  struct vm_entry *vme = hash_entry(e,struct vm_entry, hashelem);
  //hash_delete(vm, &vme->hashelem);
  free(vme);
}