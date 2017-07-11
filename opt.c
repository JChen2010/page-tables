#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"
#include "sim.h"

extern int debug;

extern struct frame *coremap;

// Number of traces
int num_traces;

// Current trace;
int curr_trace;

// Array for storing traces
addr_t *traces;

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	// Time until current frame use (measured in # of traces)
	int t;

	// Store maximum time to frame use and index of occurence
	int max;
	int max_index;
	max = 0;
	
	// Calculate time to next use of frame for each frame in coremap
	int i;
	for (i = 0; i < memsize; i = i + 1) {

		t = 0;
		int j;
		j = curr_trace;

		// Iterate through each future trace, and compute time to next use of
		// current frame. We compare the vaddrs right shifted by PAGE_SHIFT
		// since we only care about vaddrs refering to the same page frame,
		// not the exact location.
		while ((coremap[i].vaddr >> PAGE_SHIFT) != (traces[j] >> PAGE_SHIFT)) {
			t = t + 1;
			j = j + 1;

			// Frame is never accessed again, return frame
			if (j == num_traces) {
				return i;
			}
		}

		// Compare time for current frame to max
		if (t > max) {
			max = t;
			max_index = i;
		}

	}

	// Return index of frame with the longest time until next use
	return max_index;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	// Increment current trace
	curr_trace = curr_trace + 1;

	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	curr_trace = 0;
	num_traces = 0;

	FILE *tfp = fopen(tracefile, "r");

	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;

	// Count the number of traces
	while(fgets(buf, MAXLINE, tfp) != NULL) {
		num_traces = num_traces + 1;
	}

	// Initialize and store traces in array
	traces = malloc(num_traces * sizeof(addr_t));
	fseek(tfp, 0, SEEK_SET);

	int traces_index;
	traces_index = 0;

	while(fgets(buf, MAXLINE, tfp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			traces[traces_index] = vaddr; 
			traces_index = traces_index + 1;
		}
	}
	fclose(tfp);
}

