#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

// Use an integer variable to represent time
unsigned long time_count;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
  int victim;
  // This represents the least recent reference
  unsigned long least_recent_ref = time_count;
  int i;
  for (i = 0; i < memsize; ++i) {
    if (coremap[i].pte->last_ref < least_recent_ref) {
      // Found new oldest referenced PTE
      victim = i;
      // Update least recent ref time
      least_recent_ref = coremap[i].pte->last_ref;
    }
  }

	return victim;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
  // Update the last ref field of the PTE
  p->last_ref = time_count;

  // increment time for future reference
  time_count++;

	return;
}


/* Initialize any data structures needed for this
 * replacement algorithm
 */
void lru_init() {
  time_count = 0;
}
