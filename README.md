# CPU Cache Simulator (C++)

A configurable cache simulator that models how a CPU cache responds to a
memory access trace. Built to understand memory hierarchy, address decoding,
and the impact of cache design choices on hit rate.

## Features
- Direct-mapped and N-way set-associative configurations
- LRU (Least Recently Used) replacement policy
- Address decoding into tag / index / offset fields
- Hit/miss statistics and hit-rate reporting
- Benchmarking across configurations on the same trace

## Build & Run
    make
    ./cache_sim <cache_size> <block_size> <associativity> <trace_file>

Example:
    ./cache_sim 256 32 2 traces/trace1.txt

## Sample Analysis (256 B cache, 32 B blocks, trace1.txt)
| Config        | Hit rate |
|---------------|----------|
| Direct-mapped | 33.3%    |
| 2-way         | 50.0%    |
| 4-way         | 66.7%    |

Higher associativity reduces conflict misses: addresses 0x0000, 0x0100 and
0x0200 all map to set 0, so a direct-mapped cache thrashes between them,
while a 4-way cache holds all three simultaneously.

## Possible Extensions
- Write-through / write-back policies with dirty bits
- Multi-level (L1/L2) simulation
- FIFO and random replacement policies for comparison