#include <stdio.h>
#include <stdbool.h>
#include "debug.h"
#include "vm/page.h"
#include "hash.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "page.h"
#include "frame.h"
#include "swap.h"
#include "userprog/pagedir.h"
#include "devices/block.h"
#include "bitmap.h"

static struct bitmap swap_bitmap;

void swap_init(){
    //swap_bitmap = bitmap_create(block_size(block_get_role(BLOCK_SWAP)));
    
}

struct page *select_victim(struct list *list){
    return list_front(list);
}

void swap_in(struct page *p){
    return;
}

void swap_out(struct page *page){
    struct thread *t = page->t;
    struct vm_entry *vme = page->vme;
    int i;
    if (pagedir_is_dirty(t->pd,vme->vaddr)){
        struct block *spartition = block_get_role(BLOCK_SWAP);
        //block_sector_t sector;
        /*for (i = 0; i < &swap_bitmap->bit_cnt; i++){
            if(!bitmap_test(&swap_bitmap,i)) break;
        }*/
        block_write(spartition,i,page->kaddr);
    }
    pagedir_clear_page(t->pd,page);
}