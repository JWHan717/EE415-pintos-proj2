#include <stdio.h>
#include <stdbool.h>
#include "debug.h"
#include "vm/page.h"
#include "hash.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "userprog/process.h"
#include "page.h"
#include "frame.h"
#include "swap.h"
#include "userprog/pagedir.h"
#include "devices/block.h"
#include "bitmap.h"

static struct bitmap *swap_bitmap;

void swap_init(){
    swap_bitmap = bitmap_create(block_size(block_get_role(BLOCK_SWAP)));
    
}

struct page *select_victim(struct list *list){
    return list_front(list);
}

void swap_in(struct page *p){
    struct block *spartition = block_get_role(BLOCK_SWAP);
    block_sector_t sector;
    for (sector = 0; sector < bitmap_size(swap_bitmap); sector++){
        if(bitmap_test(swap_bitmap,sector)) break;
    }
    block_read(spartition,sector,p->kaddr);
}

void swap_out(struct page *page){
    struct thread *t = page->t;
    struct vm_entry *vme = page->vme;
    switch(page->vme->type){
        case VM_BIN:
            if (pagedir_is_dirty(t->pagedir,vme->vaddr)){
                struct block *spartition = block_get_role(BLOCK_SWAP);
                block_sector_t sector;
                for (sector = 0; sector < bitmap_size(swap_bitmap); sector++){
                    if(!bitmap_test(swap_bitmap,sector)) break;
                }
                block_write(spartition,sector,page->kaddr);
                bitmap_mark(swap_bitmap,sector);
            }
            pagedir_clear_page(t->pagedir,vme->vaddr);
            lru_free_page(page->kaddr);
            vme->type = VM_ANON;
            break;
        case VM_FILE:
            if (pagedir_is_dirty(t->pagedir,vme->vaddr)){
                file_write_at(vme->f,vme->vaddr,vme->read_bytes,vme->offset);
            }
            pagedir_clear_page(t->pagedir,vme->vaddr);
            lru_free_page(page->kaddr);
            break;
        case VM_ANON:
            struct block *spartition = block_get_role(BLOCK_SWAP);
            block_sector_t sector;
            for (sector = 0; sector < bitmap_size(swap_bitmap); sector++){
                if(!bitmap_test(swap_bitmap,sector)) break;
            }
            block_write(spartition,sector,page->kaddr);
            bitmap_mark(swap_bitmap,sector);
            pagedir_clear_page(t->pagedir,vme->vaddr);
            lru_free_page(page->kaddr);
            break;
        default:
            break;
    }
}