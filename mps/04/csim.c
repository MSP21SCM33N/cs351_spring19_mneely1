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

int main(int argc, char *argv[] )
{
    int E = 0;
    int b = 0;
    int s = 0;
    char *t; 
    int misses = 0
    int hits = 0;
    int evicts = 0
    int size; 
    char command
    unsigned long long int address; // Initializes the data storage in the cache
    char parse_cmdline;
    extern char *optarg; // Needed for the getopt function

    while((parse_cmdLine = getopt(argc, argv, "hvs:E:b:t:"))!= -1) {// Parses cmd Line arguments
        switch(parse_cmdLine){
            case 's':
                s = atoi(optarg); // if option has a value, it points to the extern optarg, so you have to call the atoi() to convert the string to an int value
                break;
            case 'E':
                E = atoi(optarg);
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = optarg;
                break;  
        }
    }

    File *trace; // check to see if the tracefile in our command line exists
    if (( trace = fopen(t, "r")) == NULL){ // use the f open command and r to open and read the file. If it is equal to null, the tracefile isn't available to open
        printf("Could not open %s\n",t);
        exit(1);
    }

    cache c; //Initialize the cache structure itself
    cache_set set; // 
    cache_line line;

    cache.numbsets = b;
    cache.numb_lines = E;
    cache.block_size = s;

    c.sets = (cache_set *) malloc(sizeof(cache_set)*(pow(2.0,s)) ); // Initialize the cache sets and allowcate memory for it. 
    for (int i = 0; i < pow(2,s), i++){
        set.lines = (cache_line *) malloc(sizeof(cache_line)*(E));
        c.sets[i] = set;
       for (int j = 0; j < E; i++){
           line.valid = 0;
           line.tag = 0;
           line.line_row_cnt = 0;
           set.lines[j] = line;
       } 
    }

    while (fscanf(trace, "%c %llx, %d", &command, &address, &size) == 3){//Set the buffer to 3 to evaluate the commands in the tracefile
        switch(command){
            case 'L':
            case 'S':
                eval_cache(c,b, address, &misses, &hits, &evicts);
                break;
            case 'M':
                eval_cache(c, b, address, &misses, &hits, &evicts);
                eval_cache(c,b, address, &misses, &hits, &evicts);
                break;
            case 'I':
                break;
        }    
    
    }
    fclose(trace);
    printSummary(hits, misses, evicts);
    return 0;
}

void eval_cache( cache c, int b, unsigned long long int address, int *misses, int *hits, int * evicts){
    int cache_size = (64 -(c.numb_sets+c.block_size));
    mem_addr_t c_tag = address >> (c.numb_sets + c.block_size);// tag distinguishes between the different memory location. Dividing the address by 2^(numb_sets + block size) gives us the entire size of the tag block
    unsigned long long int temp = address << (cache_size); // Temp variable to store the 2^cache size times the address 
    unsigned long long int indexSet = temp >> (cache_size + c.block_size); // Index set = 2^(cachesize + block size)
    int numb_hits = *hits;
    int full = 1; 
    cache_set array_set = c.sets[indexSet];
    for (int i = 0; i < c.numb_lines; i++){// Evaluate the cache lines to see if there is a hit
        cache_line = array_set.lines[i];
        if (line.valid){
            if (line.tag = c_tag){
                *hits++;
                line.line_row_cnt++;
                array_set.lines[i]=line;
            }
        }else if(!(line.valid) && (full)){
            full = 0;
        }
    }

    if (numb_hits == *hits){//Something in the cache is not found
        *misses++;
    }else{
        return;
    }
    
    int *lines_used = (int *) malloc(sizeof(int)*2); // Allocated memory for our return pointer which will be used for our other function to get the less frequently used line in our cache
    int index = least_freq_used(array_set, c.numb_lines, lines_used);

    if (full){// When full, an evict has to be done
        *evicts++;
        array_set.lines[index].tag = c_tag;
        array_set.lines[index].line_row_cnt = lines_used[1] + 1;
    }else{//The case of an empty line
        for(int i = 0; i < c.numb_lines; i++){
            if (array_set.lines[i].valid == 0){
                array_set.lines[i].tag = c_tag;
                array_set.lines[i].valid = 1;
                array_set.lines[i].line_row_cnt = lines_used[1] + 1;
                return;
            }
        }
    }

}
int least_freq_used(cache_set set, int numb_lines, *line_used){
    int min_used= set.lines[0].line_row_cnt;
    int max_used = set.lines[0].line_row_cnt; 
    int index_min_used = 0;
    cache_line line;

    for (int i = 0; i < numb_lines; i++){
        line = set.lines[i];
        if (min_used > line.line_row_cnt){// If the min is used at this line
            index_min_used = i;
            min_used = line.line_row_cnt;
        }
        if (max_used < line.line_row_count){
            max_used = line.line_row_cnt;
        }
    }
    used_lines[0] = min_used;
    used_lines[1] = max_used;
    return index_min_used; 

}
