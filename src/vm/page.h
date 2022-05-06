#include <stdbool.h>
#include "debug.h"
#include "hash.h"
#include "file.h"

struct vm_entry {
    struct hash_elem *hashelem;
    struct file *f;
};

void vm_init(struct hash *);
void vm_destroy(struct hash *);
struct vm_entry *find_vme(void *);

bool insert_vme(struct hash *, struct vm_entry *);
bool delete_vme(struct hash *, struct vm_entry *);

static unsigned vm_hash_func(const struct hash_elem *, void *UNUSED);
static bool vm_less_func(const struct hash_elem *, const struct hash_elem *, void *UNUSED);
static void vm_destroy_func(struct hash_elem *, void *UNUSED);
