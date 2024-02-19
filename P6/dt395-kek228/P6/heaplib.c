#include <stdlib.h>
#include <stdio.h>
#include "heaplib.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "spinlock.h"

/* 
 * Global lock object.  You should use this global lock for any locking you need to do.
 */
#ifdef __riscv
volatile lock_t malloc_lock = {.riscv_lock = 0};
#else
volatile lock_t malloc_lock = {.pthread_lock = PTHREAD_MUTEX_INITIALIZER};
#endif
typedef struct heap_h{
    int heap_size;
    struct block *first_block;
} heap_h;

typedef struct block{
    unsigned int block_size; 
    struct block *prev; 
    struct block *next;
    bool in_use;
} block;


/* the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
int hl_init(void *heap, unsigned int heap_size) {
    mutex_lock(&malloc_lock);
    if(heap==NULL){
        mutex_unlock(&malloc_lock);
        return FAILURE;
    }
    if(heap_size<MIN_HEAP_SIZE){
        mutex_unlock(&malloc_lock);
        return FAILURE;
    }    
    int offset =(8-(*(int *)&heap%8))%8;
    heap_h *heapp=(heap_h*)((char *)heap+ offset);
    heapp->heap_size=heap_size-sizeof(heap_h) -offset;
    heapp->first_block=(block*)((char*)(heapp)+sizeof(heap_h));
    block *free=(block *)heapp->first_block;
    free->block_size=heapp->heap_size;
    free->prev=NULL;
    free->next=NULL;
    free->in_use=false;
    mutex_unlock(&malloc_lock);
    return SUCCESS;
}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_alloc(void *heap, unsigned int block_size) {
    mutex_lock(&malloc_lock);
    if(heap==NULL||block_size<=0){
        mutex_unlock(&malloc_lock);
        return FAILURE;
    }
    unsigned int needed=block_size + sizeof(block);
    int offset = (8-(*(int *)&heap%8))%8;
    //printf("%d", offset);
    block_size=needed + (8-(needed%8))%8;
    needed = block_size;
    heap_h *heapp=(heap_h*)((char *)heap+ offset);
    if(heapp->heap_size<block_size){
        mutex_unlock(&malloc_lock);
        return 0;
    }
    void* next_b=heapp->first_block;
    bool end=false;
    block* best_b =NULL;
    while(!end){
        if(next_b!=NULL){
            block *b=(block*)next_b;
            if(b->block_size>=needed&&(b->in_use)==false){
                if(best_b==NULL||b->block_size<best_b->block_size){
                    best_b=b;
                }
            }
            next_b=b->next;
        }
        else{
           end=true;
        }
    }
    if(best_b==NULL){
        mutex_unlock(&malloc_lock);
        return FAILURE;
    }
    if(best_b->block_size>=block_size+2*sizeof(block)){
        block* nb=(block*)(((char *)best_b)+block_size+sizeof(block));
        nb->block_size=best_b->block_size-(block_size+sizeof(block));
        nb->next=best_b->next;
        nb->prev=best_b;
        nb->in_use=false;
        best_b->block_size=block_size;
        best_b->in_use=true;
        best_b->next=nb;
        mutex_unlock(&malloc_lock);
        return (void*)(((char *)best_b)+sizeof(block));
    }
    else{
        best_b->in_use=true;
        best_b->block_size=best_b->block_size-sizeof(block);
        mutex_unlock(&malloc_lock);
        return (void*)(((char *)best_b)+sizeof(block));
    }
/*

    block* p=best_b->prev;
    block* n=best_b->next;

    printf("p: %p\n", p);
    printf("n: %p\n", n);
    printf("best_b: %p\n", best_b);
    printf("best_b->prev: %p\n", best_b->prev);
    printf("best_b->next: %p\n", best_b->next);

    block *free=(block*)((char *)best_b);
    free->block_size=best_b->block_size-block_size;

    printf("free->prev: %p\n", free->prev);
    printf("free->next: %p\n", free->next);

    if(p==NULL){
        (*free).prev=p;

    printf("%s\n","RESETprev_TO_NULL");
    }
    else{
        free->prev=p;
    }
    if(n==NULL){
        (*free).next=n;

    printf("%s\n", "RESETnext_TO_NULL");

    }
    else{
        free->next=n;
    }
    printf("best_b: %p\n", best_b);
    printf("best_b->prev: %p\n", best_b->prev);
    printf("best_b->next: %p\n", best_b->next);
    printf("free: %p\n", free);
    printf("free->prev: %p\n", free->prev);
    printf("free->next: %p\n", free->next);
    
    dest->block_size=best_b->block_size-block_size;
    block* prevv=(block*)free->prev;
 
    if(prevv!=NULL){
        (*prevv).next=free;
    }
    block* nextt=(block*)free->next;
    if (nextt!=NULL){
        nextt->prev=free;
    }
   if (heapp->first_block==best_b){
    heapp->first_block=free;
   }
    return best_b;*/
}






/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void hl_release(void *heap, void *bloc) {
/*    mutex_lock(&malloc_lock);*/
    if(bloc==NULL){
        return;
    }
    block* b=(block *)(((char *)bloc)-sizeof(block));
    
    //CASE 1: block is free
    if(b->in_use==false){
/*        mutex_lock(&malloc_lock);*/
        return;
    }

    //CASE 2: previous block is null OR next block is null OR next block is in use 
    if((b->prev==NULL||(b->prev)->in_use==true)&&(b->next==NULL||(b->next)->in_use==true)){
        b->in_use=false;
        b->block_size=b->block_size+sizeof(block);
/*        mutex_unlock(&malloc_lock);*/
        return;
    }
    //CASE 3: next block is null OR next is in use
    if((b->next==NULL||(b->next)->in_use==true)){
        ((block*)(b->prev))->next=b->next;
        if(b->next!=NULL){
            ((block*)(b->next))->prev=b->prev;
        }
        ((block*)(b->prev))->block_size=(((block*)(b->prev))->block_size)+sizeof(block)+(b->block_size);
/*        mutex_unlock(&malloc_lock);*/
        return;
    }
    //CASE 4: if previous block is ull or previous block is in use
    if((b->prev==NULL||(b->prev)->in_use==true)){
        b->block_size=((b->next)->block_size)+(b->block_size)+sizeof(block);
        if((b->next)->next!=NULL){
            ((b->next)->next)->prev=b;
        }
        b->next=(b->next)->next;
        b->in_use=false;
/*        mutex_unlock(&malloc_lock);*/
        return;
    }
    (b->prev)->next=(b->next)->next;
    if ((b->next)->next!=NULL){
        ((b->next)->next)->prev=b->prev;
    }
    (b->prev)->block_size=((b->prev)->block_size)+(b->block_size)+((b->next)->block_size)+2*sizeof(block);
/*    mutex_unlock(&malloc_lock);*/
    return;

}

/* See the .h for the advertised behavior of this library function.
 * These comments describe the implementation, not the interface.
 *
 * YOUR COMMENTS GO HERE.
 */
void *hl_resize(void *heap, void *bloc, unsigned int new_size) {
    if(bloc==NULL){
        return hl_alloc(heap, new_size);
    }
    void* b=(((char *)bloc)-sizeof(block));
    void* prev=((block *)b)->prev;
    void* next=((block *)b)->next;
    int in_bloc=((block*)b)->block_size;
    int availprev=0;
    int availnext=0;
    
    if(prev!=NULL&&!((block*)prev)->in_use){
        availprev=availprev+((block*)prev)->block_size;
    }
    if(next!=NULL&&!((block*)next)->in_use){
        availnext=availnext+((block*)next)->block_size;
    }
    int minavail=availprev;
    if (availnext>=availprev){
        minavail=availnext;
    }
    if(availnext+availprev+((block*)b)->block_size>=new_size){
        if(((block*)b)->block_size>=new_size){
             if(next!=NULL&&!((block*)next)->in_use){
                 void* new_next=(void*)(((char *)b)+sizeof(block)+new_size);
                 memmove(new_next,((block*)b)->next, sizeof(block));
                 ((block*)new_next)->block_size=((block*)new_next)->block_size+((block*)b)->block_size-new_size;
                 ((block*)b)->block_size=new_size; 
                 return bloc;
             }
             else{
                 if(((block*)b)->block_size-new_size>=sizeof(block)){
                     void* new_next=(void*)(((char *)b)+sizeof(block)+new_size);
                     ((block*)new_next)->next=NULL;
                     ((block*)new_next)->prev=b;
                     ((block*)new_next)->block_size=((block*)b)->block_size-new_size-sizeof(block);
                     ((block*)new_next)->in_use=false;
                     ((block*)b)->block_size=new_size;
                     ((block*)b)->next=new_next;
                 } 
                 return bloc;
             }
             
        }
        if(minavail+((block*)b)->block_size>=new_size){
            if (minavail+in_bloc>=new_size+2*sizeof(block)){
                if (availprev==minavail){
                    void*new_next= (void*)(((char *)prev)+sizeof(block)+new_size);
                    if(next!=NULL){
                        ((block*)next)->prev=new_next;
                    }  
                    memmove((void*)(((char *)prev)+sizeof(block)),bloc,((block*)b)->block_size);
                    ((block*)prev)->next=new_next;
                    ((block*)new_next)->next=next;
                    ((block*)new_next)->prev=prev;
                    ((block*)new_next)->block_size=availprev+in_bloc-(sizeof(block)+new_size);
                    ((block*)new_next)->in_use=false;
                    ((block*)prev)->in_use=true;
                    ((block*)prev)->block_size=new_size;
                    return (void*)(((char *)prev)+sizeof(block));
                }

                else{
                    block* new_next=(block*)(((char *)b)+new_size+sizeof(block));
                    if(((block*)next)->next!=NULL){
                        ((block*)((block*)next)->next)->prev=new_next;
                    }
                    memmove(new_next,((block*)b)->next,sizeof(block));
                    ((block*)b)->next=new_next;
                    new_next->block_size=availnext+in_bloc-(new_size+sizeof(block));
                    ((block*)b)->block_size=new_size;
                    return (void*)(((char *)b)+sizeof(block));

                }
            }
            else{
                if (availprev==minavail){
                    void*new_block= (void*)(((char *)prev)+sizeof(block));
                    ((block*)prev)->next=next;
                    if(next!=NULL){
                        ((block*)next)->next=prev;
                    }
                    memmove(new_block,bloc,((block*)b)->block_size);
                    ((block*)prev)->block_size=availprev+in_bloc;
                    return new_block;
                }
                else{
                    ((block*)b)->next=((block*)next)->next;
                    if(((block*)next)->next!=NULL){
                        ((block*)((block*)next)->next)->prev=b;
                    }
                    ((block*)b)->block_size=in_bloc+availnext;
                    return (void*)(((char *)b)+sizeof(block));

                }
            }            
        }















        if(availprev+availnext-minavail+((block*)b)->block_size>=new_size){
            if (availprev+availnext+in_bloc-minavail>=new_size+2*sizeof(block)){
                if (availprev!=minavail){
                    void*new_next= (void*)(((char *)prev)+sizeof(block)+new_size);
                    if(next!=NULL){
                        ((block*)next)->prev=new_next;
                    }  
                    memmove((void*)(((char *)prev)+sizeof(block)),bloc,((block*)b)->block_size);
                    ((block*)prev)->next=new_next;
                    ((block*)new_next)->next=next;
                    ((block*)new_next)->prev=prev;
                    ((block*)new_next)->block_size=availprev+in_bloc-(sizeof(block)+new_size);
                    ((block*)new_next)->in_use=false;
                    ((block*)prev)->in_use=false;
                    ((block*)prev)->block_size=new_size;
                    return (void*)(((char *)prev)+sizeof(block));
                }

                else{
                    block* new_next=(block*)(((char *)b)+new_size+sizeof(block));
                    if(((block*)next)->next!=NULL){
                        ((block*)((block*)next)->next)->prev=new_next;
                    }
                    memmove(new_next,((block*)b)->next,sizeof(block));
                    ((block*)b)->next=new_next;
                    new_next->block_size=availnext+in_bloc-(new_size+sizeof(block));
                    ((block*)b)->block_size=new_size;
                    return (void*)(((char *)b)+sizeof(block));

                }
            }
            else{
                if (availprev!=minavail){
                    void*new_block= (void*)(((char *)prev)+sizeof(block));
                    ((block*)prev)->next=next;
                    if(next!=NULL){
                        ((block*)next)->next=prev;
                    }
                    memmove(new_block,bloc,((block*)b)->block_size);
                    ((block*)prev)->block_size=availprev+in_bloc;
                    return new_block;
                }
                else{
                    ((block*)b)->next=((block*)next)->next;
                    if(((block*)next)->next!=NULL){
                        ((block*)((block*)next)->next)->prev=b;
                    }
                    ((block*)b)->block_size=in_bloc+availnext;
                    return (void*)(((char *)b)+sizeof(block));
                
                }
            }
        }
        void* new_block= (void*)(((char *)prev)+sizeof(block));
        if (availprev+availnext+in_bloc>=new_size+2*sizeof(block)){
            memmove(new_block,bloc,((block*)b)->block_size);
            ((block*)prev)->block_size=new_size;
            void* new_next=(block*)(((char *)prev)+new_size+sizeof(block));
            ((block*)((block*)next)->next)->prev=new_next;
            memmove(new_next,prev,sizeof(block));
            ((block*)new_next)->block_size=in_bloc+availprev+availnext-new_size-sizeof(block);
            ((block*)new_next)->prev=prev;
            ((block*)new_next)->in_use=false;
            ((block*)prev)->next=new_next;
            ((block*)prev)->in_use=true;
            return (block*)(((char *)prev)+sizeof(block));
        }
        else{
            ((block*)prev)->next=((block*)next)->next;
            ((block*)((block*)next)->next)->prev=prev;
            memmove(new_block,bloc,((block*)b)->block_size);
            ((block*)prev)->block_size=availprev+availnext+in_bloc;
            ((block*)prev)->in_use=true;
            return new_block;
        }
    }

    void* new_block = hl_alloc(heap, new_size);
    if (new_block != NULL){
        memcpy(new_block, bloc, sizeof(block));
        hl_release(heap, bloc);
        return new_block;
     }
    return 0;
    

}
