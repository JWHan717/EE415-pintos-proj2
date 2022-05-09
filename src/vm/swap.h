#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <stdbool.h>
#include "debug.h"
#include "hash.h"
#include "filesys/file.h"
#include "page.h"
#include "frame.h"

void swap_init();
struct page *select_victim(struct list *);
void swap_out(struct page *);
void swap_in(struct page *);


#endif /* vm/swap.h */