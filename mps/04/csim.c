#include "cachelab.h"
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

// Direct mapped cache simulator


typedef unsigned long long int mem_addr_t; // Implements the data storage type for the data in the cache(the data storage size for a cache is 8 bytes, so this is why we use unsigned long long)

typedef struct{
    int valid; // flag that indicates the presence of a valid bit in a row
    mem_addr_t tag; //Tag stores the memory address of the data fetched 
    int line_row_cnt; // Indicates which specific row we are in accessing the cache

}cache_line; // Total struct is one row of the cache line (Data) 

typedef struct{
    cache_line *lines; // Points to an array of rows in a cache (set)
}cache_set;

typedef struct{
    int numb_sets, numb_lines, block_size;
    cache_set * sets; // Points to an array of sets in a cache set

}cache;

int main()
{
    printSummary(0, 0, 0);
    return 0;
}
