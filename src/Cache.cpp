#include "Cache.h"
#include <cassert>

int Cache::log2int(int x) {
    int r = 0;
    while ((1 << r) < x) r++;
    return r;
}

Cache::Cache(const CacheConfig& cfg) : cfg_(cfg) {
    // block size must be a power of two
    assert(cfg.blockSize > 0 && (cfg.blockSize & (cfg.blockSize - 1)) == 0);
    numSets_    = cfg.cacheSize / (cfg.blockSize * cfg.associativity);
    assert(numSets_ >= 1);
    offsetBits_ = log2int(cfg.blockSize);
    indexBits_  = log2int(numSets_);
    sets_.assign(numSets_, std::vector<CacheLine>(cfg.associativity));
}

bool Cache::access(uint64_t address) {
    clock_++;

    // Split address into: [ tag | index | offset ]
    uint64_t index = (address >> offsetBits_) & (uint64_t)(numSets_ - 1);
    uint64_t tag   = address >> (offsetBits_ + indexBits_);

    std::vector<CacheLine>& set = sets_[index];

    // 1) HIT? search all ways in this set
    for (auto& line : set) {
        if (line.valid && line.tag == tag) {
            line.lastUsed = clock_;
            stats_.hits++;
            return true;
        }
    }

    // 2) MISS: place block in empty way, else evict LRU
    stats_.misses++;
    int victim = -1;
    uint64_t oldest = UINT64_MAX;
    for (int i = 0; i < (int)set.size(); i++) {
        if (!set[i].valid) { victim = i; break; }   // empty way
        if (set[i].lastUsed < oldest) {             // track LRU
            oldest = set[i].lastUsed;
            victim = i;
        }
    }
    set[victim] = {true, tag, clock_};
    return false;
}