#include <stdarg.h>
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include "mem_internals.h"
#include "mem.h"
#include "util.h"

void debug_block(struct block_header* b, const char* fmt, ... );
void debug(const char* fmt, ... );

extern inline block_size size_from_capacity( block_capacity cap );
extern inline block_capacity capacity_from_size( block_size sz );

static bool            block_is_big_enough( size_t query, struct block_header* block ) { return block->capacity.bytes >= query; }
static size_t          pages_count   ( size_t mem )                      { return mem / getpagesize() + ((mem % getpagesize()) > 0); }
static size_t          round_pages   ( size_t mem )                      { return getpagesize() * pages_count( mem ) ; }

static void block_init( void* restrict addr, block_size block_sz, void* restrict next ) {
  *((struct block_header*)addr) = (struct block_header) {
    .next = next,
    .capacity = capacity_from_size(block_sz),
    .is_free = true
  };
}

static size_t region_actual_size( size_t query ) { return size_max( round_pages( query ), REGION_MIN_SIZE ); }

extern inline bool region_is_invalid( const struct region* r );



static void* map_pages(void const* addr, size_t length, int additional_flags) {
  return mmap( (void*) addr, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | additional_flags , 0, 0 );
}

/*  аллоцировать регион памяти и инициализировать его блоком */
static struct region alloc_region  ( void const * addr, size_t query ) {
  void *region_addr = map_pages(addr, query, MAP_FIXED);
  struct region res; // returning result;

  if (!region_addr || region_addr == MAP_FAILED){
  	region_addr = map_pages(NULL, query, 0);
	res = REGION_INVALID;
  }

  const block_size sz = {
  	.bytes = region_actual_size(query)
  };
  
  res = (struct region){
  	.addr = region_addr,
	.size = region_actual_size(query),
	.extends = true

  };

  block_init(region_addr, sz, NULL);
  return res;
}

static void* block_after( struct block_header const* block )         ;

void* heap_init( size_t initial ) {
  const struct region region = alloc_region( HEAP_START, initial );
  if ( region_is_invalid(&region) ) return NULL;

  return region.addr;
}

#define BLOCK_MIN_CAPACITY 24

/*  --- Разделение блоков (если найденный свободный блок слишком большой )--- */

static bool block_splittable( struct block_header* restrict block, size_t query) {
  return block-> is_free && query + offsetof( struct block_header, contents ) + BLOCK_MIN_CAPACITY <= block->capacity.bytes;
}

static bool split_if_too_big( struct block_header* block, size_t query ) {
  if(!block_splittable(block, query)) return false;

  const block_size nxt_sz = {
  	.bytes = block->capacity.bytes - query
  };

  const block_capacity prev_cap = {
  	.bytes = query,
  };
  
  //split 
  void* split_addr = ((uint8_t*)block + query + offsetof(struct block_header, contents));
  block_init(split_addr, nxt_sz, block->next);
  block->capacity = prev_cap;
  block->next = split_addr;
  return true;

}


/*  --- Слияние соседних свободных блоков --- */

static void* block_after( struct block_header const* block )              {
  return  (void*) (block->contents + block->capacity.bytes);
}
static bool blocks_continuous (
                               struct block_header const* fst,
                               struct block_header const* snd ) {
  return (void*)snd == block_after(fst);
}

static bool mergeable(struct block_header const* restrict fst, struct block_header const* restrict snd) {
	//printf("%s", "start mergeable\n");
  return fst->is_free && snd->is_free && blocks_continuous( fst, snd ) ;
}

static bool try_merge_with_next( struct block_header *block ) {
   struct block_header *nxt = block->next;
   
   if(!nxt || !mergeable(block, nxt)) return false;

   block->next = nxt->next;
   block->capacity.bytes += size_from_capacity(nxt->capacity).bytes;
   return true;
}


/*  --- ... ecли размера кучи хватает --- */

struct block_search_result {
  enum {BSR_FOUND_GOOD_BLOCK, BSR_REACHED_END_NOT_FOUND, BSR_CORRUPTED} type;
  struct block_header* block;
};


static struct block_search_result find_good_or_last  ( struct block_header* restrict block, size_t sz )    {
  	if(!block) return (struct block_search_result){.type = BSR_CORRUPTED};

	struct block_header *now = block;
	struct block_header *got = NULL;

	for(;;){ // until can go 
	  if(!now) break;
	  if(block_is_big_enough(sz, block) && now->is_free) 
	     return (struct block_search_result){
		  	.block = now,
			.type = BSR_FOUND_GOOD_BLOCK,
	      };
	  got = now;
	  now = now->next;
	}
	return (struct block_search_result){
		.block = got,
		.type = BSR_REACHED_END_NOT_FOUND,
	};
}

/*  Попробовать выделить память в куче начиная с блока `block` не пытаясь расширить кучу
 Можно переиспользовать как только кучу расширили. */
static struct block_search_result try_memalloc_existing ( size_t query, struct block_header* block ) {
//	printf("%s", "start try_memalloc_existing\n");
	struct block_header *nxt = block;
	for(;;){
	   if(nxt == NULL) break;
   	   try_merge_with_next(block);
	   nxt = nxt->next;	   
	}
//	printf("%s", "return try_memalloc_existing\n");
	return find_good_or_last(block, size_max(BLOCK_MIN_CAPACITY, query));
	

}



static struct block_header* grow_heap( struct block_header* restrict last, size_t query ) {
  query = size_max(BLOCK_MIN_CAPACITY, query);
  struct region grow = alloc_region(block_after(last), query + offsetof(struct block_header, contents)); // expand
  last->next = grow.addr;
  return last->next;
}

/*  Реализует основную логику malloc и возвращает заголовок выделенного блока */
static struct block_header* memalloc( size_t query, struct block_header* heap_start) {
   //printf("%s", "start memalloc\n");
   struct block_search_result exist = try_memalloc_existing(query, heap_start);
   //printf("%s\n", "memalloc");
   if (exist.type == BSR_REACHED_END_NOT_FOUND){
   	grow_heap(exist.block, query);
	return NULL;
   } 
   else if(exist.type == BSR_CORRUPTED) return NULL;
  
   split_if_too_big(exist.block, query);
   exist.block->is_free = false;
   return exist.block;
   
  
}

void* _malloc( size_t query ) {
//	printf("%s", "start malloc\n");
//	printf("query: %zu\n", query);
  struct block_header *const addr = memalloc( query, (struct block_header *) HEAP_START );
//  printf("%s","end malloc");
  if (addr) return addr->contents;
  else return NULL;
}

static struct block_header* block_get_header(void* contents) {
  return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void _free( void* mem ) {
  if (!mem) return ;
  struct block_header* header = block_get_header( mem );
  header->is_free = true;
  for(;;){
      if(!header) return;
      try_merge_with_next(header);
      header = header->next;
  }

}
