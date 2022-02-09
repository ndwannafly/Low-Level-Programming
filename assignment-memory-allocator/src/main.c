#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#include "mem.h"
#include "util.h"
#include "mem_internals.h"

#define HEAP_SIZE 1000

void *heap;
struct block_header *block;

int number_allocated_blocks(struct block_header *block){
    int res = 0;
    struct block_header *nxt = block;
    for(;;){
    	if(!nxt->next) return res;
	++res;
	nxt = nxt ->next;
    }
    return res;
}

void test1(){
   printf("test 1: _malloc function testing....!\n");
   void *b = _malloc(100);
   if(b == NULL){
   	err("Error: _malloc calls but memory is not allocated!");
   }
   if(block->capacity.bytes != 100){
   	err("Error: _malloc calls memory is allocated but wrong size!"); 
   }
   printf("Passed test1: _malloc works fine!\n");
   _free(b);
  
}

void test2(){
   printf("test2: _free one block testing.....!\n");
   void *b1 = _malloc(200);
   void *b2 = _malloc(300);
   if(number_allocated_blocks(block) != 2){
      _free(b1);
      _free(b2);
      err("Error: not enough blocks are allocated");
   }

   _free(b1);
   if (number_allocated_blocks(block) !=1 ){
   	err("Error: _free calls but incorrect number of freed blocks");
   }
   _free(b2);
   printf("Passed test2: _free one block works fine!\n");
}

void test3(){
   printf("test3: _free two blocks testing.....!\n");
   void *b1 = _malloc(200);
   void *b2 = _malloc(300);
   void *b3 = _malloc(400);
   
   if(number_allocated_blocks(block) != 3){
   	_free(b1);
	_free(b2);
	_free(b3);
	err("Error: not enough blocks are allocated");
   }

   _free(b1);
   _free(b2);

   if(number_allocated_blocks(block) != 1){
   	err("Error: _free calls but incorrect number of freed blocks");
   }

   _free(b3);
   printf("Passed test3: _free two blocks work fine!\n");
}

void test4(){
    printf("test4: test heap memory expands the old one ....!\n");
    void *b1 = _malloc(900);
    void *b2 = _malloc(500);
    
    if(number_allocated_blocks(block) != 2){
    	err("Error: heap memory doesn\'t expand the old one");
    }
    _free(b1);
    _free(b2);
    printf("Passed test4: heap memory expands the old one when the memory runs out\n");
}

void test5(){
    printf("test5: the new region is allocated in a different place testing......! \n");
    for(;;){
    	if(block->next) block = block->next;
	else break;
    }

    void *addr = block + block->capacity.bytes;
    size_t cnt= (size_t) addr / sysconf(_SC_PAGESIZE);
    size_t add = (size_t) addr % sysconf(_SC_PAGESIZE);

    uint8_t *all = (uint8_t *) (sysconf(_SC_PAGESIZE) * (cnt + (add >1)));
    mmap(all, 100, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    void *allocated = _malloc(1000);
    if ((uint8_t *) block->next == (uint8_t *) allocated - offsetof(struct block_header, contents)){
        err("Error: the new region is allocated in the same place");
    }
    _free(allocated);
    printf("Passed test5: _the new region is allocated in a different place correctly\n");
}
int main(){
    heap = heap_init(HEAP_SIZE);
    block = (struct block_header *) heap;
    if(!heap || !block){
    	err("fail to allocate heap memory");
    }

    test1();
    test2();
    test3(); 
    test4();
    test5();
    printf("Passed all testcases!\n");
    return 0;
}

