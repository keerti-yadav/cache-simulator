#ifndef CACHE_H
#define CACHE_H

#include <cstdint>
#include <vector>

struct CacheLine {
    bool valid = false;
    uint64_t tag = 0;
    uint64_t lastUsed = 0;   
};

struct CacheConfig {
    int cacheSize;       
    int blockSize;       
    int associativity;   
};

struct CacheStats {
    uint64_t hits = 0;
    uint64_t misses = 0;
};

class Cache {
public:
    Cache(const CacheConfig& cfg);
    bool access(uint64_t address);   
    const CacheStats& stats() const { return stats_; }
    int numSets() const { return numSets_; }

private:
    CacheConfig cfg_;
    int numSets_;
    int offsetBits_;
    int indexBits_;
    uint64_t clock_ = 0;
    CacheStats stats_;
    std::vector<std::vector<CacheLine>> sets_;

    static int log2int(int x);
};

#endif