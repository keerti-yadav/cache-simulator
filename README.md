# 🧠 CPU Cache Simulator

**A configurable CPU cache simulator in C++ that models hit/miss behavior of direct-mapped and N-way set-associative caches using LRU replacement — built to understand memory hierarchy from the inside out.**

![Language](https://img.shields.io/badge/language-C++17-blue.svg)
![Build](https://img.shields.io/badge/build-g%2B%2B%20%2F%20Makefile-brightgreen.svg)
![Replacement](https://img.shields.io/badge/replacement-LRU-orange.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)

---

## 📌 Overview

Modern CPUs spend a significant fraction of execution time waiting on memory. The cache hierarchy exists to bridge the speed gap between the processor and main memory, and its design — size, block size, associativity, replacement policy — directly determines real-world performance and power efficiency.

This project is a **software model of that hardware**. It consumes a trace of memory addresses, decodes each address into **tag / index / offset** fields, and simulates how a real cache would respond — reporting hits, misses, and hit rate across different configurations on the *same* workload.

> Built as part of my preparation for systems & embedded software engineering roles, to move beyond theory and develop an intuitive, quantitative feel for cache behavior.

---

## ✨ Features

- ✅ **Direct-mapped** and **N-way set-associative** cache configurations
- ✅ **LRU (Least Recently Used)** replacement policy
- ✅ Full **address decoding** — tag / index / offset extraction via bitwise operations
- ✅ **Hexadecimal trace file** input (supports comments and blank lines)
- ✅ Hit / miss / hit-rate **statistics reporting**
- ✅ **Cross-configuration benchmarking** on identical traces
- ✅ Defensive design: power-of-two validation via `assert`, unsigned address arithmetic

---

## 🧩 Core Concepts Implemented

### 1. Address Decoding
Every memory address is split into three fields:

```
        address (64-bit)
┌─────────────────────┬───────────────┬────────────────┐
│         TAG         │     INDEX     │     OFFSET     │
└─────────────────────┴───────────────┴────────────────┘
   remaining bits      log2(numSets)    log2(blockSize)
```

```cpp
uint64_t index = (address >> offsetBits) & (uint64_t)(numSets - 1);
uint64_t tag   = address >> (offsetBits + indexBits);
```

The mask `(numSets - 1)` replaces an expensive modulo operation — valid because the number of sets is a power of two, exactly as in real hardware.

### 2. Set-Associative Lookup
On every access, all ways in the target set are searched for a matching valid tag (**hit**). On a **miss**, the block is placed in an empty way, or the **least recently used** line is evicted.

### 3. LRU via Timestamps
Instead of maintaining a reordering linked list, each line stores a logical timestamp (`clock_`) of its last access. Eviction scans the small number of ways (typically 2–16) for the oldest timestamp — O(1) on hit, O(ways) on miss. This mirrors why real hardware *approximates* LRU: true LRU bookkeeping is too expensive at high associativity.

### 4. Miss Classification (observed in analysis)
- **Compulsory misses** — first access to a block; unavoidable.
- **Conflict misses** — multiple blocks mapping to the same set evicting each other; eliminated by higher associativity.

---

##  Repository Structure

```
cache-simulator/
├── src/
│   ├── Cache.h        # CacheLine, CacheConfig, CacheStats, Cache class interface
│   ├── Cache.cpp      # Address decoding, hit/miss logic, LRU eviction
│   └── main.cpp       # CLI parsing, trace reading, statistics reporting
├── traces/
│   └── trace1.txt     # Hand-checkable hex address trace
├── screenshots/       # Sample run outputs
├── Makefile
└── README.md
```

---

## 🛠️ Build & Run

### Requirements
- `g++` with C++17 support
- `make` (optional)

### Build
```bash
make
```
or directly:
```bash
g++ -std=c++17 -Wall -Wextra -g -O2 -o cache_sim src/main.cpp src/Cache.cpp
```

### Run
```bash
./cache_sim <cache_size_bytes> <block_size_bytes> <associativity> <trace_file>
```

**Example:**
```bash
./cache_sim 256 32 1 traces/trace1.txt   # direct-mapped
./cache_sim 256 32 2 traces/trace1.txt   # 2-way set-associative
./cache_sim 256 32 4 traces/trace1.txt   # 4-way set-associative
```

> All sizes must be powers of two (enforced by `assert`), matching real cache geometry.

---

## 📊 Benchmark Analysis

Same cache size (256 B), same block size (32 B), same trace — only associativity varies:

| Configuration | Sets | Hits | Misses | Hit Rate |
|:---|:---:|:---:|:---:|:---:|
| Direct-mapped (1-way) | 8 | 4 | 8 | **33.3%** |
| 2-way set-associative | 4 | 6 | 6 | **50.0%** |
| 4-way set-associative | 2 | 8 | 4 | **66.7%** |

### Why does hit rate climb?
The trace contains addresses `0x0000`, `0x0100`, and `0x0200`, which all map to **set 0**. In a direct-mapped cache they fight for the single line, causing repeated **conflict misses** (thrashing). Adding ways lets them coexist:

- **1-way:** each new address evicts the previous one → thrashing.
- **4-way:** all three blocks live in set 0 simultaneously → conflict misses vanish.

Compulsory misses (first touch of each block) remain constant across all configurations — exactly as cache theory predicts.

---

## 🧠 Design Decisions

| Decision | Rationale |
|---|---|
| `uint64_t` for addresses | Addresses are unsigned; shifting signed integers invites undefined behavior. Systems code uses unsigned types for bit work. |
| Power-of-two `assert` on block size | The masking trick `(numSets - 1)` equals modulo only for powers of two — the same reason real caches use power-of-two sizes (cheap hardware decoding). |
| LRU timestamp counter over linked list | Simpler, cache-friendly, and O(1) on the hot path (hits). Real CPUs approximate LRU for the same cost reasons. |
| Trace-driven simulation | Mirrors how real cache research is done (e.g., `cachegrind`, gem5) and enables reproducible benchmarking. |
| Config-driven CLI | One binary, many experiments — ideal for comparative analysis. |

---


*“The cache is where software meets the physics of the machine. Simulating it is the fastest way to understand why CPUs are built the way they are.”*
