/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define BLK_HDR_SIZE ALIGN(sizeof(blockHDR))
#define BLK_FTR_SIZE ALIGN(sizeof(blockFTR))
#define BLK_SIZE ALIGN(BLK_FTR_SIZE + BLK_HDR_SIZE)
#define NUM_SIZE_CLASSES 6
#define MIN_BLK_SIZE 32

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

size_t min_class_size[] = {MIN_BLK_SIZE, 40, 64 +MIN_BLK_SIZE, 128 + MIN_BLK_SIZE, 256 +MIN_BLK_SIZE, 1024 +MIN_BLK_SIZE};

typedef struct header blockHDR;
typedef struct footer blockFTR;
    
struct header{
    size_t size;
    blockHDR *next_pnt;
    blockHDR *prior_p; // Initializes a doubly linked list
};

struct footer{
    size_t size;
};

/* 
 * mm_init - initialize the malloc package.
 */
blockHDR *head;
void *first_fit(size_t size);
void print_heap();
void split(blockHDR *bp, size_t needed);
int mm_init(void)
{
  head = mem_sbrk(NUM_SIZE_CLASSES * BLK_HDR_SIZE);
  for (int i = 0; i <NUM_SIZE_CLASSES; i++){
    head[i].size = BLK_HDR_SIZE;
    head[i].next_pnt = &head[i];
    head[i].prior_p = &head[i];
  }

  return 0;

}

void print_heap(){
    blockHDR *bp = mem_heap_lo();
    while (bp < (blockHDR *)mem_heap_hi()){
        printf("%s block at %p, size%d\n",(bp->size&1)?"allocated":"free", bp, (int)(bp->size&~1));
        bp = (blockHDR *)((char*)bp + (bp->size & ~1));
    }
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
  int newsize = ALIGN(BLK_HDR_SIZE +size + BLK_FTR_SIZE);
  blockHDR *bg_pnt = first_fit(newsize); // Requesting k block bytes to allocate; need to find a fit in order to find a free block to hold the allocated block
  blockFTR *fp_pnt;
  if (bg_pnt == NULL){
    bg_pnt = mem_sbrk(newsize);// requesting more memory for heap
    if ((long)bg_pnt == -1){
        return NULL; // Essentially saying that there was an error with our break
    }else{
        fp_pnt = (blockFTR *)((char*)bg_pnt + newsize - BLK_FTR_SIZE);
        bg_pnt->next_pnt = bg_pnt->prior_p = bg_pnt;
        bg_pnt->size = newsize |1;
        fp_pnt->size = newsize |1;
        
    }
  }else{
    fp_pnt = (blockFTR*)((char*)(bg_pnt) + (bg_pnt->size & ~1) -BLK_FTR_SIZE);
    bg_pnt->size |= 1;
    fp_pnt->size |= 1; 
  }
  return (char *)bg_pnt + BLK_HDR_SIZE;
}
//does this work
/*
 * mm_free - Freeing a block does nothing.
 */
void split(blockHDR *bp, size_t needed){

 blockHDR *bbp;
 blockFTR *fp, *ffp; 
 size_t remaining = bp->size - needed;

 if(remaining <= BLK_SIZE)
   return;

 fp = (blockFTR*)((char*)bp + needed - BLK_FTR_SIZE);
 bbp = (blockHDR*)((char*)bp + needed);
 ffp = (blockFTR*)((char*)bbp + remaining - BLK_FTR_SIZE);
 fp->size = needed;
 bp->size = needed;
 ffp->size = remaining & ~1;
 bbp->size = remaining & ~1;
 for (int i = NUM_SIZE_CLASSES-1; i>0; i--){
   if (min_class_size[i] <= remaining) {
     bbp->next_pnt  = head[i].next_pnt;
     bbp->prior_p = &head[i];
     head[i].next_pnt = bbp;
     bbp->next_pnt->prior_p = bbp;
     break;
    }
  }
}
void *first_fit(size_t size){
    blockHDR *bp;
    for(int i = 0; i <NUM_SIZE_CLASSES-1; i++){
        if ((min_class_size[i] >= size) && (head[i].next_pnt != &head[i])){
            bp = head[i].next_pnt;
            head[i].next_pnt = bp->next_pnt;
            bp->next_pnt->prior_p = &head[i];
            bp->next_pnt = bp->prior_p = bp;
            return bp;
        }
    }
    bp = head[NUM_SIZE_CLASSES-1].next_pnt;
    while(bp != &head[NUM_SIZE_CLASSES-1]){
        if(bp->size >= size){
            bp->next_pnt->prior_p = bp->prior_p;
            bp->prior_p->next_pnt = bp->next_pnt;
            bp->next_pnt = bp->prior_p = bp;
            split(bp, size);
            return bp;
        }
        bp = bp->next_pnt;
    }
   return NULL;
 
}
void mm_free(void *ptr)
{
 blockHDR *bp,*final_hd,*next_hd;
 blockFTR *fp,*prior_ft,*final_ft;
 bp = ptr-BLK_HDR_SIZE;
 bp->size &= ~1;
 fp = (blockFTR*)((char *)bp + bp->size - BLK_FTR_SIZE);
 fp->size = bp->size;
 final_hd = bp;
 final_ft = fp;
 int flag = 0;
 if((blockHDR*)((char*)bp - BLK_HDR_SIZE) > &head[NUM_SIZE_CLASSES-1]){
   prior_ft = (blockFTR*)((char*)bp - BLK_FTR_SIZE);
   if(prior_ft->size == (prior_ft->size & ~1)){
     flag = 1;
     final_hd = (blockHDR *)((char*)bp - prior_ft->size);
     final_hd->size = prior_ft->size + bp->size;
     final_ft->size = final_hd->size;
     final_hd->next_pnt->prior_p = final_hd->prior_p;  //get prev block out of list
     final_hd->prior_p->next_pnt = final_hd->next_pnt;   //get prev block out of list
     final_hd->next_pnt = final_hd->prior_p = final_hd;
   }
 }
 if((void *)((char*)fp + BLK_FTR_SIZE) < mem_sbrk(0)){
    next_hd = (blockHDR *)((char*)fp + BLK_FTR_SIZE);
    if(next_hd->size == (next_hd->size & ~1)){
      final_ft = (blockFTR *)((char*)next_hd + next_hd->size - BLK_FTR_SIZE);
      if(flag)
        final_ft->size = prior_ft->size + bp->size + next_hd->size;
      else
        final_ft->size = fp->size + next_hd->size;
      final_hd->size = final_ft->size;
      next_hd->next_pnt->prior_p = next_hd->prior_p;   //get next block out of list
      next_hd->prior_p->next_pnt = next_hd->next_pnt;    //get next block out of list
      next_hd->prior_p = next_hd->next_pnt = next_hd;
    }
  }
  
 size_t fullsize = final_hd->size;
 for (int i = NUM_SIZE_CLASSES-1; i > 0; i--){
     if(min_class_size[i] <= fullsize){
         final_hd->next_pnt = head[i].next_pnt;
         final_hd->prior_p = &head[i];
         head[i].next_pnt = final_hd;
         final_hd->next_pnt->prior_p = final_hd;
         break;
     } 
   }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
  void *oldptr = ptr;
  void *newptr;
  size_t copySize;
    
  newptr = mm_malloc(size);
  if (newptr == NULL)
    return NULL;
  copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
  if (size < copySize)
    copySize = size;
  memcpy(newptr, oldptr, copySize);
  mm_free(oldptr);
  return newptr;
}
