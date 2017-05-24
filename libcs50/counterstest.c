/*
 * counterstest.c - test program for CS50 counters module
 *
 * usage:
 *   read integers from stdin to use as test data.
 *   each integer should be separated by space, 
 *   otherwise, the program will treat as end of input.
 *
 * Kazuma Honjo, April 2017
 */

// Conditional inclusion for platform specific builds
#ifdef NOPEBBLE // we are *not* building for pebble

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "counters.h"

static void totalcount(void *arg, const int key, int count);

/* main */
int main(){
	counters_t *counters;
	printf("starting test...\n");

	counters = counters_new();
	if (counters == NULL){
		fprintf(stderr, "counters_new failed\n");
		exit(1);
	}

	// test counters_add
	// count should be key 0: 5 count and key 1: 1 count
	counters_add(counters,0);
	counters_add(counters,0);
	counters_add(counters,0);
	counters_add(counters,0);
	counters_add(counters,0);
	counters_add(counters,1);
	printf("initial counter: ");
	counters_print(counters, stdout);
	printf("\n");

	// when key is negative or counters is NULL
	printf("when key is negative or counters is NULL\n");
	printf("this should not change the result: ");
	counters_add(counters,-1);
	counters_add(NULL, 0);
	// these functions should not change the result
	counters_print(counters, stdout);
	printf("\n");
	printf("test for counters_add ended \n\n");

	
	// test counters_get
	printf("test counters_get");
	int count;
	count = counters_get(counters, 0);  // this should return 5
	printf("count for 0 is %d\n", count);

	// when counters is NULL or key is not found
 	count = counters_get(NULL,0);
	if (count!=0){
	  fprintf(stderr, "error when counters is NULL\n");
	  exit(2);
	}
	count = counters_get(counters, -1);
	if (count!=0){
          fprintf(stderr, "error when finding negative key\n");
          exit(2);
	}
	count = counters_get(counters, 2);
	if (count!=0){
          fprintf(stderr, "error when key not found\n");
          exit(2);
	}
	printf("test for counters_get ended \n\n");


	// test counters_set
	printf("test counters_set\n");
	printf("change the count of 0 to 100\n");
	counters_set(counters, 0, 100);
	counters_print(counters, stdout);
      	printf("\n");

	// when counters is NULL or key is negative
	// these should not change the result
	printf("testing when the counters is NULL, key is negative, or count is negative\n");
	printf("this should not change the result\n");
	counters_set(counters, 0, -10);
	counters_set(NULL, 0, 0);
	counters_print(counters,stdout);
	printf("\n");
	counters_set(NULL, -10, 10);
	counters_print(counters,stdout);
	printf("\ntest for counters_set ended\n\n");
	

	// test counters_print
	printf("test counters_print\n");
	printf("when NULL fp: ");
	counters_print(counters, NULL);
	printf("\n");
	printf("when NULL ctrs: ");
	counters_print(NULL, stdout);
	printf("\ntest for counters_print ended\n\n");


	// test counters_iterate
	printf("test counters_iterate\n");
	int total = 0;
	counters_iterate(counters, &total, totalcount);
	printf("total count: %d\n", total);
	// test when ctrs is NULL or itemfunc is NULL
	total = 0;
	counters_iterate(NULL, &total, totalcount);
	if (total != 0){
	  fprintf(stderr, "total count when ctrs is NULL: %d\nerror\n", total);
	  exit(3);
	}
	total = 0;
	counters_iterate(counters, &total, NULL);
	if (total != 0){
	  fprintf(stderr, "total count when itemfunc is NULL: %d\nerror\n", total);
	  exit(3);
	}
	printf("test for counters_iterate ended\n\n");


	// test counters_delete
	counters_delete(counters);

	
	// read from stdin
	printf("type in integer keys\n");
	counters_t *ctrs;
        printf("starting test...\n");

        ctrs = counters_new();
        if (ctrs == NULL){
                fprintf(stderr, "counters_new failed\n");
                exit(1);
        }
	int num;
	int status;       // current status - did scanf succeed or not
	while ( (status = scanf("%d", &num)) != EOF) {
		// if number is an integer, add to the counters
		if (status == 1){
			if (num >= 0){
				counters_add(ctrs, num);
			}
		}

		// if ctrl + D is pressed, there are no more input
		else if (status == EOF){
			printf("^D\n");
			break;
		}

		// if any non-integer, non-space input has been detected, end reading
		else {
			fprintf(stderr, "Non-integer or non-space input found.\n");
			scanf("%*s");
			break;
		}
	}

	// print all counters
	counters_print(ctrs, stdout);
	printf("\n");

	counters_delete(ctrs);
}


/* count the total count of the collection
 */ 
static void
totalcount(void *arg, const int key, int count){
  int *total = arg;
  if (total != NULL){
    (*total) += count;
  }
}

#endif
