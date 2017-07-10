#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// the oldest page in memory
int oldest_idx;

/* Page to evict is chosen using the fifo algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int fifo_evict() {
  // evict the current oldest page
  int victim = oldest_idx;
  // the oldest page is always the head of page addr list
  oldest_idx = (oldest_idx + 1) % memsize;
	return victim;
}

/* This function is called on each access to a page to update any information
 * needed by the fifo algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void fifo_ref(pgtbl_entry_t *p) {

	return;
}

/* Initialize any data structures needed for this
 * replacement algorithm
 */
void fifo_init() {
  oldest_idx = 0;
}
