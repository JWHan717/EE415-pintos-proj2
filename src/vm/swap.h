#ifndef VM_SWAP_H
#define VM_SWAP_H

#include <stdbool.h>
#include "debug.h"
#include "hash.h"
#include "filesys/file.h"
#include "page.h"
#include "frame.h"

void swap_init(void);
size_t swap_out(struct page *);
void swap_in(size_t, struct page *);


#endif /* vm/swap.h */