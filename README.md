# cache-simulator
This was a project for CMSC 15400 at UChicago. This program simulates (counts the number of hits, misses and misses) the behavior of a cache with a LRU policy. This program reads valgrind reference trace files 
in the format 

[space]operation address,size

ex.

L 04f6b868,8

S 7ff0005c8,8

Command line args are as follows:

./csim-ref [-hv] -s < s > -E < E > -b < b > -t <tracefile>
  
-h: Optional help flag that prints usage info (not implemented)
  
-v: Optional verbose flag that displays trace info (not implemented)
  
-s < s >: Number of set index bits (S =2^s is the number of sets)
  
-E < E >: Associativity (number of lines per set)
  
-b < b >: Number of block bits (B =2^b is the block size)
  
-t < tracefile >: Name of the valgrind trace to replay
  
Use the funciton printSummary for the number of total hits, misses, and evictions
  
