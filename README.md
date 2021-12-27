# cache-hit-and-miss-simulator
This program simulates (counts the number of hits and misses) the behavior of a cache with a LRU policy. This program reads valgrind reference trace files 
in the format \n
[space]operation address,size
ex.
L 04f6b868,8
S 7ff0005c8,8
Command line args are as follows:
./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
-h: Optional help flag that prints usage info (not implemented)
-v: Optional verbose flag that displays trace info (not implemented)
-s <s>: Number of set index bits (S =2s is the number of sets)
-E <E>: Associativity (number of lines per set)
-b <b>: Number of block bits (B =2b is the block size)
-t <tracefile>: Name of the valgrind trace to replay
