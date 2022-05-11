#include <stdio.h>
#include <stdbool.h>
#include "debug.h"
#include "vm/page.h"
#include "hash.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "page.h"
#include "frame.h"
#include "swap.h"
#include "userprog/pagedir.h"
#include "devices/block.h"
#include "bitmap.h"

static struct block *swap_block;
static struct bitmap *swap_bitmap;

static size_t swap_size;
static const size_t SLOTS = PGSIZE / BLOCK_SECTOR_SIZE;

void swap_init(){
    swap_block = block_get_role(BLOCK_SWAP);
    if(swap_block == NULL) exit(-1);

    swap_size = block_size(swap_block) / SLOTS;
    swap_bitmap = bitmap_create(swap_size);
    bitmap_set_all(swap_bitmap, true);
}

void swap_in(size_t swap_idx, struct page *page){
    ASSERT (is_kernel_vaddr(page->kaddr));
    ASSERT (swap_idx <= swap_size);
    printf("%d\n", swap_idx);
    if(bitmap_test(swap_bitmap, swap_idx)) exit(-1);

    for(size_t i=0; i < SLOTS; ++i) {
        block_read(swap_block, swap_idx*SLOTS + i, page + BLOCK_SECTOR_SIZE*i);
    }
    bitmap_set(swap_bitmap, swap_idx, true);
}

size_t swap_out(struct page *page){
    if (page < PHYS_BASE) exit(-1);

    size_t swap_idx = bitmap_scan(swap_bitmap, 0, 1, true);
    for (size_t i=0; i < SLOTS; ++i) {
        if(pagedir_is_dirty(page->t->pagedir, page->kaddr)) {
            block_write(fs_device, swap_idx*SLOTS + i, page + BLOCK_SECTOR_SIZE*i);
        }
        block_write(swap_block, swap_idx*SLOTS + i, page + BLOCK_SECTOR_SIZE*i);
    }
    bitmap_set(swap_bitmap, swap_idx, false);
    return swap_idx;
}
