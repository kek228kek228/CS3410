#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "heaplib.h"
#include <pthread.h>

#define HEAP_SIZE 1024
#define NUM_TESTS 24
#define NPOINTERS 100
#define ARRAY_LEN 128
#define ADD_BYTES(base_addr, num_bytes) (((char *)(base_addr)) + (num_bytes))


// TODO: Add test descriptions as you add more tests...
const char* test_descriptions[] = {
    /* our SPEC tests */
    /* 0 */ "single init, should return without error",
    /* 1 */ "single init then single alloc, should pass",
    /* 2 */ "single alloc which should fail b/c heap is not big enough",
    /* 3 */ "multiple allocs, verifying no hard-coded block limit",
    /* your SPEC tests */
    /* 4  */ "if block is null, resize should act like alloc",
    /* 5  */ "if there is not enough space, block should not be allocated",
    /* 6  */ "checks whether a heap is created when there IS NOT enough space.",
    /* 7  */ "when block is resized, its data should be preserved",
    /* 8  */ "release should act as nop if block is null",
    /* 9  */ "checks whether alloc returns a pointer that has the correct alignment",
    /* 10 */ "if block is null, resize should act like alloc (2nd test)",
    /* 11 */ "checks for 8 - byte alignment",
    /* 12 */ "your description here",
    /* 13 */ "your description here",
    /* 14 */ "your description here",
    /* 15 */ "example threading test",
    /* STRESS tests */
    /* 16 */ "alloc & free, stay within heap limits",
    /* 17 */ "alloc, release, resize checks correctness of address",
    /* 18 */ "causing segfault by alloc, release, resize many times",
    /* 19 */ "causing segfault by init, alloc, release, resize many times",
    /* 20 */ "checks if blocks in use being used before they are released",
    /* 21 */ "many iterations of alloc blocks of random size, resize to 0",
    /* 22 */ "your description here",
    /* 23 */ "your description here",
};

/* ------------------ COMPLETED SPEC TESTS ------------------------- */

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init
 * SPECIFICATION BEING TESTED:
 * hl_init should successfully complete (without producing a seg
 * fault) for a HEAP_SIZE of 1024 or more.
 *
 * MANIFESTATION OF ERROR:
 * A basic test of hl_init.  If hl_init has an eggregious programming
 * error, this simple call would detect the problem for you by
 * crashing.
 *
 * Note: this shows you how to create a test that should succeed.
 */
int test00() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    return SUCCESS;
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is room in the heap for a request for a block, hl_alloc
 * should sucessfully return a pointer to the requested block.
 *
 * MANIFESTATION OF ERROR:
 * The call to hl_alloc will return NULL if the library cannot find a
 * block for the user (even though the test is set up such that the
 * library should be able to succeed).
 */
int test01() {

    // simulated heap is just a big array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);

    // if this returns null, test01 returns FAILURE (==0)
    return (hl_alloc(heap, HEAP_SIZE/2) != NULL);
}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * If there is not enough room in the heap for a request for a block,
 * hl_alloc should return NULL.
 *
 * MANIFESTATION OF ERROR:
 * This test is designed to request a block that is definitely too big
 * for the library to find. If hl_alloc returns a pointer, the library is flawed.
 *
 * Notice that heaplame's hl_alloc does NOT return NULL. (This is one
 * of many bugs in heaplame.)
 *
 * Note: this shows you how to create a test that should fail.
 *
 * Surely it would be a good idea to test this SPEC with more than
 * just 1 call to alloc, no?
 */
int test02() {

    // simulated heap is just an array
    char heap[HEAP_SIZE];

    hl_init(heap, HEAP_SIZE);
    // if this returns NULL, test02 returns SUCCESS (==1)
    return !hl_alloc(heap, HEAP_SIZE*2);

}

/* THIS TEST IS COMPLETE AND WILL NOT BE INCOPORATED INTO YOUR GRADE.
 *
 * FUNCTIONS BEING TESTED: init & alloc
 * SPECIFICATION BEING TESTED:
 * There should be no hard-coded limit to the number of blocks heaplib
 * can support. So if the heap gets larger, so should the number of
 * supported blocks.
 *
 * MANIFESTATION OF ERROR:
 * See how many blocks are supported with heap size N. This number should
 * increase with heap size 2N. Otherwise fail!
 *
 * Note: unless it is fundamentally changed, heaplame will always fail
 * this test. That is fine. The test will have more meaning when run on
 * your heaplib.c
 */
int test03() {

    // now we simulate 2 heaps, once 2x size of the first
    char heap[HEAP_SIZE], heap2[HEAP_SIZE*2];
    int num_blocks = 0;
    int num_blocks2 = 0;

    hl_init(heap, HEAP_SIZE);

    while(true) {
        int *array = hl_alloc(heap, 8);
        if (array)
            num_blocks++;
        else
            break;
    }

    hl_init(heap2, HEAP_SIZE*2);

    while(true) {
        int *array = hl_alloc(heap2, 8);
        if (array)
            num_blocks2++;
        else
            break;
    }
#ifdef PRINT_DEBUG
    
    printf("%d blocks (n), then %d blocks (2n) allocated\n", num_blocks, num_blocks2);
#endif

    // hoping to return SUCCESS (==1)
    return (num_blocks2 > num_blocks);
}

/* ------------------ YOUR SPEC TESTS ------------------------- */

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc,  resize
 * SPECIFICATION BEING TESTED: if block is null then resize should behave like alloc
 *
 *
 * MANIFESTATION OF ERROR: If the block remains null, then the test fails.
 *
 */
int test04() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    //create a null block 
    void *block = hl_alloc(heap,2*HEAP_SIZE);

    //resizing the null block
    block = hl_resize(heap, block, HEAP_SIZE/2);

    //checking if block is null
    if (block == NULL) {
        return FAILURE;
    }
    return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED: if heap is not big enough for alloc size, block should not be allocated, ie
 * it should remain null
 *
 *
 * MANIFESTATION OF ERROR: If the block is null, then the test passes
 *
 */
int test05() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    //creating a null block
    void *block = hl_alloc(heap, HEAP_SIZE*2); 

    //checking if block is null
    if (block == NULL){
        return SUCCESS;
    }
    return FAILURE;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:init
 * SPECIFICATION BEING TESTED: Checks whether a "heap" is created when there IS NOT enough space.
 *
 *
 * MANIFESTATION OF ERROR: if the heap is created (ie, hl_init does not return 0), then then the tests 
 * fails because the heap should not have been created. If it returns 0, then test passes
 *
 */
int test06() {

    char heap[HEAP_SIZE];

    //try to create heap of size 0
    int heap_created_f = hl_init(heap, 0);

    //checking if heap was created
    if (heap_created_f) {
        return FAILURE;
    }
    return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: resize
 * SPECIFICATION BEING TESTED: block contens should be maintained if block is resized to a larger 
 * size
 *
 *
 * MANIFESTATION OF ERROR: allocate to blocks of the same size, resize one to be larger. It compares
 * the first 32 bits of both. If not identical, test fails
 */
int test07() {

    
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    void *block_ = hl_alloc(heap, 32);
    char *block1 = (char*)block_;

    hl_alloc(heap, 32);

    //resizing the original block from 32 to 64 bits
    char *block2 = (char*)hl_resize(heap, block_, 64);

    int i = 0;

    //checking if each 32 bits is the same
    while(i < 32){
        if (block1[i] != block2[i]){
        return FAILURE;
        }
        i++;
    }
    return SUCCESS;
}



/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: hl_release
 * SPECIFICATION BEING TESTED: hl_release is a nop if block is null
 *
 *
 * MANIFESTATION OF ERROR: compares heap before and after the release of the null block to check
 * if it is the same. If it is the same, then the test passes. 
 *
 */
int test08() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    
    //creating null block
    void *block = hl_alloc(heap, 2*HEAP_SIZE);

    char heap_copy[HEAP_SIZE];
    memmove(heap_copy, heap, HEAP_SIZE*sizeof(char));

    hl_release(heap, block);
    hl_alloc(heap, 16);

    //checking if heap contents remain the same before after after release of a null block. s
    //should be same bc nop
    if (strcmp(heap, heap_copy) == 0){
        return SUCCESS;
    }

    return FAILURE;
    
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED: Checks whether hl_alloc returns a pointer that has the correct
 * alignment.
 *
 *
 * MANIFESTATION OF ERROR: iterates through many allocs, fails if any block is not 8-byte aligned
 *
 */
int test09() {

    char array[HEAP_SIZE];
    void *block;
    bool aligned_f = false;

    hl_init(array, HEAP_SIZE);

    for (int i = 1; i < 1000; i++){
        block = hl_alloc(array, ARRAY_LEN*i);
        aligned_f = !((uintptr_t)block & (ALIGNMENT - 1));
        if (!aligned_f) {
            return FAILURE;
        }
    }

    return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: release, resize
 * SPECIFICATION BEING TESTED: if a null block is resized, it should behave like alloc
 *
 *
 * MANIFESTATION OF ERROR: if the size of nullblock is not equal to 8 (val it was resized to), 
 * then test will fail
 *
 */

int test10() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);

    //creating ull block 
    void *nullblock = hl_alloc(heap, 0);

    hl_release(heap, nullblock); 
    hl_resize(heap, 0, 8);

    if (sizeof(nullblock)==8){
        return SUCCESS;
    }
    return FAILURE;
   
}



/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED: checking for 8 byte alignment
 * 
 *
 *
 * MANIFESTATION OF ERROR: if block mod 8 is not 0, then fail. This means that it is not 8 - byte aligned
 *
 */
int test11() {

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);


    for(int size = 0; size <= 8; size++){
        uintptr_t block = (uintptr_t)hl_alloc(heap, size);
        if(block % 8 != 0){
            return FAILURE;
        }
    }

    return SUCCESS;
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED: 
 * SPECIFICATION BEING TESTED: 
 *
 *
 * MANIFESTATION OF ERROR: 
 * 
 */

int test12() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    void* block=hl_alloc(heap, HEAP_SIZE/2);
    hl_release(heap, block);
    return (NULL!=hl_alloc(heap, HEAP_SIZE/2));
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test13() {
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    void* block=hl_alloc(heap, HEAP_SIZE/2);
    hl_resize(heap, block, HEAP_SIZE/4);
    return NULL!=hl_alloc(heap, HEAP_SIZE/2);
    
}

/* Find something that you think heaplame does wrong. Make a test
 * for that thing!
 *
 * FUNCTIONS BEING TESTED:
 * SPECIFICATION BEING TESTED:
 *
 *
 * MANIFESTATION OF ERROR:
 *
 */

int test14() {
    /*
    char heap[HEAP_SIZE];
    int num_blocks = 0;
    int num_blocks2 = 0;
    void** array = malloc(25*sizeof(void*));
    hl_init(heap, HEAP_SIZE);

    while(true) {
        void *array2 = hl_alloc(heap, 8);
        if (array){
            array[num_blocks]=array2;
            num_blocks++;
            
        }
        else
            break;
    }
    int acc=0;
    while(acc<num_blocks) {
        if(acc<num_blocks){

            hl_release(heap, array[acc]);
            acc++;
        }
        else
            break;
    }
        while(true) {
        void *array2 = hl_alloc(heap, 8);
        if (array){
            array[num_blocks2]=array2;
            num_blocks2++;
            
        }
        else
            break;
    }
    return (num_blocks2==num_blocks);
}
*/

    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    void* block=hl_alloc(heap, HEAP_SIZE/2);
    return NULL!=hl_resize(heap, block, 2*HEAP_SIZE/3);
    

 



}

//typedef struct {
//    int arg1;
//    void* heap;
//    pthread_barrier_t* barrier;
//} arg_struct;

/* The signature for a function run as a thread is a single void* argument returning a void*.
 * If you want to pass in arguments, you need to define a struct of arguments and cast manually.
 */

//void* thread_function(void* ptr){
//    arg_struct* args = (arg_struct*) ptr;
//    pthread_barrier_t* barrier = args->barrier;
//    //wait for all threads to synchronize at a barrier so they will run concurrently
//    pthread_barrier_wait(barrier);
//    hl_alloc(args->heap, 16);
//    //do stuff with your arguments here
//    return NULL;
//}


/* THIS TEST IS NOT FINISHED.  It is a scaffold you can use for a threaded test.
 *
 * FUNCTIONS BEING TESTED: alloc
 * SPECIFICATION BEING TESTED:
 * Malloc library must be thread-safe: multiple threads simultaneously using the library
 * should not interfere with each other or cause a deadlock
 *
 * MANIFESTATION OF ERROR:
 * Test runs forever on a deadlock, or integrity is violated on a race condition.
 */
int test15() {
//    char heap[HEAP_SIZE];
//    hl_init(heap, HEAP_SIZE);
//
//    int n_threads = 100;
//    //int num_iter = 10000;
//    pthread_t threads[n_threads];
//    int irets[n_threads]; //return values of the threads
//
//   //initialize a synchronization barrier
//    pthread_barrier_t barrier;
//    pthread_barrier_init(&barrier, NULL, n_threads);
//
//    arg_struct args[n_threads];
//   for(int i = 0; i < n_threads; i++){
//	args[i] = (arg_struct) {.arg1=1, .barrier = &barrier, .heap=&heap}; //inline initialization of a struct
//
//    }
//    
//
//    
//    //create threads to execute your function
//    for(int i = 0; i < n_threads; i++){
//	irets[i] = pthread_create(&threads[i], NULL, thread_function, (void*) &args[i]);
//    }
//    //wait for all of the threads to finish before moving on
//    for(int i = 0; i < n_threads; i++){
//	pthread_join(threads[i], NULL);
//    }
//    //cleanup the barrier
//    pthread_barrier_destroy(&barrier);
//    printf("Thread 0 returned: %d\n", irets[0]);
    
    char heap[HEAP_SIZE];
    hl_init(heap, HEAP_SIZE);
    void* block=hl_alloc(heap, HEAP_SIZE*2);
    return NULL!=hl_resize(heap, block, HEAP_SIZE/4);
    
}

/* ------------------ STRESS TESTS ------------------------- */

/* THIS TEST IS NOT FINISHED. It is a stress test, but it does not
 * actually test to see whether the library or the user writes
 * past the heap. You are encouraged to complete this test.
 *
 * FUNCTIONS BEING TESTED: alloc, free
 * SPECIFICATION BEING TESTED:
 * The library should not give user "permission" to write off the end
 * of the heap. Nor should the library ever write off the end of the heap.
 *
 * MANIFESTATION OF ERROR:
 * If we track the data on each end of the heap, it should never be
 * written to by the library or a good user.
 *
 */
int test16() {
    int n_tries    = 10000;
    int block_size = 16;

    // 1024 bytes of padding
    // --------------------
    // 1024 bytes of "heap"
    // --------------------  <-- heap_start
    // 1024 bytes of "padding"
    char memarea[HEAP_SIZE*3];

    char *heap_start = &memarea[1024]; // heap will start 1024 bytes in
    char *pointers[NPOINTERS];

    // zero out the pointer array
    memset(pointers, 0, NPOINTERS*sizeof(char *));

    hl_init(heap_start, HEAP_SIZE);
    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        if (pointers[index] == 0) {
            pointers[index] = hl_alloc(heap_start,  block_size);
        }
        else{
            hl_release(heap_start, pointers[index]);
            pointers[index] = 0;
        }
    }
    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: resize, alloc, release
 * INTEGRITY OR DATA CORRUPTION? DATA
 *
 * MANIFESTATION OF ERROR: if data address is not between the start and end of the block, test fails
 *
 */
int test17() {
    int n_tries = 50000;

    //setting up heap and padding as in previous example
    char memarea[HEAP_SIZE*3];
    char *start = &memarea[1024]; 
    char *end =  &memarea[2047]; 
    char *ptr[NPOINTERS];
    hl_init(start, HEAP_SIZE);
    
    // zero out the pointer array as in previous example
    memset(ptr, 0, NPOINTERS*sizeof(char *)); 
    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        int index = random() % NPOINTERS;
        if (ptr[index] == 0) {
            //randomize blocksize but keep within bounds of HEAP_SIZE
            int blocksize = random() % HEAP_SIZE;
            blocksize = blocksize + (ALIGNMENT- (blocksize % ALIGNMENT));
            ptr[index] = hl_alloc(start,  blocksize);

        }
        else{
            int newsize = random() % HEAP_SIZE;
            ptr[index] = hl_resize(start, ptr[index], newsize);
            if(ptr[index]!=NULL){
                for(int j = 0; j < newsize; j++){
                    void *address = ADD_BYTES(ptr[index],j);
                    if((char *)address > end || (char *)address < start){
                        return FAILURE;
                    }
                }
            }
            hl_release(start, ptr[index]);
            ptr[index] = 0;
        }
    }
    return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: alloc, release, resize
 * INTEGRITY OR DATA CORRUPTION? INTEGRITY
 *
 * MANIFESTATION OF ERROR: causing a segmentation fault by doing many iterations and allocating
 * and resizing blocks of random size. Similar to previous test but now initialiing the heap 
 * inside the loop
 *
 */
int test18() {

  int n_tries = 50000;
  char heap[HEAP_SIZE];
  hl_init(heap, HEAP_SIZE);
  for (int i = 0; i < n_tries; i++) {
       //randomize blocksize but keep within bounds of HEAP_SIZE
        int blocksize = random() % HEAP_SIZE;
        blocksize = blocksize + (ALIGNMENT- (blocksize % ALIGNMENT));
        void *block1 = hl_alloc(heap,  blocksize);
        //randomize the new size but keep within bounds of HEAP_SIZE
        int newsize = random() % HEAP_SIZE;
        void *blockresized = hl_resize(heap, block1, newsize);
        hl_release(heap, blockresized);
    }
  return SUCCESS;
}

/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: alloc, release, resize
 * INTEGRITY OR DATA CORRUPTION? INTEGRITY
 *
 * MANIFESTATION OF ERROR: causing a segmentation fault by doing many iterations and allocating
 * and resizing blocks of random size. Similar to previous test but now initialiing the heap 
 * inside the loop
 *
 */
int test19() {

    int n_tries= 50000;
    for (int i = 0; i < n_tries; i++) {
            char heap[HEAP_SIZE];
            hl_init(heap, HEAP_SIZE);
            //randomize blocksize but keep within bounds of HEAP_SIZE
            int blocksize = random() % HEAP_SIZE;
            blocksize = blocksize + (ALIGNMENT- (blocksize % ALIGNMENT));
            void *block1 = hl_alloc(heap,  blocksize);
            int new_size = random() % HEAP_SIZE;
            void *blockresized = hl_resize(heap, block1, new_size);
            hl_release(heap, blockresized);
        }
    return SUCCESS;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: alloc, release
 * INTEGRITY OR DATA CORRUPTION? DATA
 *
 * MANIFESTATION OF ERROR: The data that is not freed should not have data allocated on it. We cann * allocate data in a block that is in use unless it has been freed
 *
 */
int test20() {
    int n_done=0;
    int n_tries = 50000;
    char heap[HEAP_SIZE];
    int in_use[HEAP_SIZE];
    int sizes[HEAP_SIZE];
    char *ptr[NPOINTERS];
    char *heap_start = &heap[0];
    int heap_start_int = (uintptr_t)heap_start;
    memset(ptr, 0, NPOINTERS*sizeof(char *));

    hl_init(heap_start, HEAP_SIZE);
    srandom(0);
    for (int i = 0; i < n_tries; i++) {
        n_done+=1;
        int index = random() % NPOINTERS;
        if (ptr[index] == 0) {
            printf("in_use[360], alloc: %d\n",in_use[360]);
            //randomize blocksize but keep within bounds of HEAP_SIZE
            int blocksize = random() % HEAP_SIZE;
            blocksize = blocksize + (ALIGNMENT- (blocksize % ALIGNMENT));
            ptr[index] = hl_alloc(heap_start,  blocksize);
            if(ptr[index]!=NULL){
              int addr = (uintptr_t)ptr[index];
              int bytes = addr - heap_start_int;
              sizes[bytes]= blocksize;
/*              printf("index, release: %d\n",index);
              printf("in_use: %d\n",in_use[(addr-heap_start_int]);
*/            for(int j = 0; j < blocksize; j++){
                
                if(in_use[bytes+j] == 1){
                  printf("blocksize: %d\n",n_done);
                  printf("blocksize: %d\n",blocksize);
                  printf("j: %d\n",j);
                  printf("badindex: %d\n",bytes);
                  printf("[bytess+j+1]: %d\n",in_use[bytes+j+1]);
                  printf("b+j: %d\n", bytes+j);
                  return FAILURE;
                }
                else{
                  in_use[bytes+j] =1;
                }
              }
            }
        }
        else{
            hl_release(heap_start, ptr[index]);
            printf("in_use[360], release: %d\n",in_use[360]);
/*            printf("index, release: %d\n",index);
            printf("in_use: %d\n",in_use[((uintptr_t)ptr[index])-heap_start_int]);
*/          int addr = (uintptr_t)ptr[index];
            int bytes = addr - heap_start_int;
            int size = sizes[bytes];
            for(int j = 0; j < size; j++){
              in_use[bytes+j] = 0;
            }
            sizes[bytes] = 0;
            ptr[index] = 0;
        }
    }
    return SUCCESS;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED: alloc, resize
 * INTEGRITY OR DATA CORRUPTION? INTEGRITY
 *
 * MANIFESTATION OF ERROR: causing a segmentation fault by doing many iterations and allocating
 * blocks of random size and resizing them to 0.
 *
 */
int test21() {
    int n_tries= 50000;
    for (int i = 0; i < n_tries; i++) {
            char heap[HEAP_SIZE];
            hl_init(heap, HEAP_SIZE);
            //randomize blocksize but keep within bounds of HEAP_SIZE
            int blocksize = random() % HEAP_SIZE;
            blocksize = blocksize + (ALIGNMENT- (blocksize % ALIGNMENT));
            void *block1 = hl_alloc(heap,  blocksize);
            int new_size = 0;
            void *blockresized = hl_resize(heap, block1, new_size);
            hl_release(heap, blockresized);
        }
    return SUCCESS;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test22() {

    return FAILURE;
}


/* Stress the heap library and see if you can break it!
 *
 * FUNCTIONS BEING TESTED:
 * INTEGRITY OR DATA CORRUPTION?
 *
 * MANIFESTATION OF ERROR:
 *
 */
int test23() {

    return FAILURE;
}
