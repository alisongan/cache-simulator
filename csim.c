#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "cachelab.h"

int time = 0;
int hits=0;
int misses = 0;
int evictions = 0;

//cache size = s*e*b
//array of structs
typedef struct {
    int timestamp;
    int tag;
    int set; 
    int valid_bit;
    int size;
} cacheline;


cacheline* createCache(int s, int e, int b)
{
    int count = 0;
    cacheline* arr = (cacheline*)malloc(sizeof(cacheline) * (pow(2,s)*e));
    for (int i =0; i< pow(2,s); i++ )
    {
        for (int j = 0; j <e; j++)
        {
            arr[count].timestamp = 0;
            arr[count].tag = 0;
            arr[count].set = i;
            arr[count].valid_bit = 0;
            arr[count].size = pow(2,b);

            count++;
        }
    }
    return arr;   
}

bool searchInCache(cacheline* cache, int set, int e, int tag, int* i)
{
    int index = set * e;
    while (cache[index].set == set)
    {
        if(cache[index].valid_bit ==1 && cache[index].tag == tag)
        {
            *i = index;
            return true;
        }
        index++;
    }
    return false;
}

/*returns true if all the valid bits in a set are 1 */
bool setAllFull( cacheline* cache, int set, int e)
{
    int index = set * e;
    while (cache[index].set == set)
    {
        if(cache[index].valid_bit != 1)
        {
            return false;
        }
        index++;
    }
    return true;
}

/*returns the index of an empty line in the cache given a set number*/
int findEmptyLine( cacheline* cache, int set, int e)
{
    if (setAllFull(cache, set,e) ==true)
    {
        printf("check beforehand that the set is not full\n");
        exit(0);
    }
    int index = set * e;
    while (cache[index].set == set)
    {
        if(cache[index].valid_bit != 1)
        {
            return index;
        }
        index++;
    }    
    return -1;
}

/*
make sure to check if the set is full before using
returns the index of the least recently used line
*/
int LRU (cacheline* cache, int set, int e)
{
    int index = set * e;
    int least = index;
    while (cache[index].set == set)
    {
        if(cache[index].timestamp < cache[least].timestamp)
        {
            least = index;
        }
        index++;
    }  
    return least;  
}
/*reads a line of a file
0 = I
1 = L
2 = S
3 = M */
//using &????
int readInstructionType(char* str)
{
    if (str[0] != ' ') // must be I
    {
        return 0;
    }
    else if (str[1] == 'L') // must be I
    {
        return 1;
    }
    else if (str[1] == 'S') // must be I
    {
        return 2;
    }
    else 
    {
        return 3;
    }
}


/* 
if first character not I, finds and returnsthe address in the instruction 
*/
int readAddress( char* str)
{
    int count =0;
    if (readInstructionType(str)!= 0)
    {
        int index = 3;
        while (str[index] != ',')
        {
            //instr = strcat(instr,&str[index]);
            count++;
            index++;
        }
    }
    else 
    {
        printf("error, I instruction type");
        exit(0);
    }
    char instr[count+1];
    strncpy(instr, &str[3], count);
    instr[count] = '\0';
    return  (int) strtol(instr, NULL, 16);
}

/*changes the tag of the cacheline at the given index*/
void write(cacheline* cache, int tag, int index)
{
    if(cache[index].valid_bit == 0)
    {
        cache[index].valid_bit = 1;
    }
    cache[index].tag = tag;
    time++;
    cache[index].timestamp = time;
}

/*returns the tag from an address */
int getTag(int address, int s, int b)
{
    int mask = ~((int)(pow(2, (s+b)) -1));
    int tag = (address & mask);
    tag = tag / (pow(2,(s+b)));
    return tag;

    //wrong 
}

/*returns the set from an address */
int getSet(int address, int s, int b)
{
    int l_mask = pow(2, (s+b)) -1;
    int r_mask = pow(2, b) -1;
    int mask = l_mask | r_mask;
    return ((address & mask) >> b);
}

void printCache(cacheline* cache, int s, int e)
{
    int index = 0;
    printf("\nset bits: %d lines per set: %d\n",s,e);
    for (int i = 0; i< pow(2,s) ; i++)
    {
        for (int j =0; j < e ; j++)
        {
            printf("set: %d line: %d  valid bit: %d tag: %d timestamp: %d\n", 
            cache[index].set,j, cache[index].valid_bit,cache[index].tag, cache[index].timestamp);
            index ++;
        }
    }
}

void executeInstruction(cacheline* cache , char* instr, int s, int e, int b)
{
    // int instrtype = readInstructionType(instr);
    // printf("instrtype: %d\n", instrtype);
    int index;
    int address = readAddress(instr);
    printf("address: %d\n", address);
    int tag = getTag(address, s, b);
    printf("tag: %d\n", tag);
    int set = getSet(address, s,b);
    printf("set: %d\n", set);
    bool inCache = searchInCache(cache, set, e , tag , &index);
    bool fullSet;
    
    //consider instruction type?
    if( inCache)
    {
        hits++;
        time++;
        cache[index].timestamp = time;
        //update timestamp
        printf("hit");
    }
    else
    {
        misses++;
        fullSet = setAllFull(cache, set, e);
        if (fullSet)
        {
            evictions++;
            index = LRU(cache, set, e);
            //printf("miss eviction, index: %d\n", index);
        }
        else
        {
            index = findEmptyLine(cache, set, e);
            //printf("miss index: %d\n", index);
        }
        write(cache, tag, index);
    }
    //printCache(cache, s,e);
    printf("\n");

}

void executeInstructionType(cacheline* cache, char* instr, int s, int e, int b)
{
    int instrtype = readInstructionType(instr);
    printf("instrtype: %d\n", instrtype);
    if (instrtype ==0 )
    {
        return;
    }
    executeInstruction(cache, instr, s, e,b);
    if (instrtype ==3)
    {
        executeInstruction(cache, instr, s,e,b);
    }
}


int main(int argc, char *argv[])
{
    int o, s, e, b;
    char* t; 
    FILE* trace; 
    size_t size;
    size_t ret=1;
    t = (char*)malloc(5); 
    while ((o = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (o)
        {
            case 'h':
                printf("help");
                break;
            case 'v':
                printf("verbose");
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                e = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace = fopen(optarg, "r");
            break;
        }
    }
    cacheline* cache = createCache(s,e,b);
    //printCache(cache,s,e); 
    printf("%d %d %d\n",s,e,b);
    ret = getline(&t, &size, trace);
    while(ret != EOF)
    {
        printf("%zu %s\n", ret, t);
        executeInstructionType(cache, t,s,e,b);
        ret = getline(&t, &size, trace);
    }

    printSummary(hits, misses, evictions);
    free(cache);
    free(t);


    return 0;
}

