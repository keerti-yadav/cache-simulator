#include "Cache.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: ./cache_sim <cache_size> <block_size> <associativity> <trace_file>\n"
                  << "  (sizes in bytes, powers of two)\n";
        return 1;
    }

    CacheConfig cfg;
    cfg.cacheSize     = std::stoi(argv[1]);
    cfg.blockSize     = std::stoi(argv[2]);
    cfg.associativity = std::stoi(argv[3]);
    std::string tracePath = argv[4];

    Cache cache(cfg);

    std::ifstream in(tracePath);
    if (!in) { std::cerr << "Cannot open trace: " << tracePath << "\n"; return 1; }

    std::string line;
    uint64_t total = 0;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        uint64_t addr;
        if (!(ss >> std::hex >> addr)) continue;   // accepts 0x... or plain hex
        cache.access(addr);
        total++;
    }

    const auto& s = cache.stats();
    double hitRate  = total ? 100.0 * s.hits   / total : 0;
    double missRate = total ? 100.0 * s.misses / total : 0;

    std::cout << "=== Cache Configuration ===\n";
    std::cout << "Cache size:     " << cfg.cacheSize     << " B\n";
    std::cout << "Block size:     " << cfg.blockSize     << " B\n";
    std::cout << "Associativity:  " << cfg.associativity << "-way\n";
    std::cout << "Number of sets: " << cache.numSets()   << "\n";
    std::cout << "Replacement:    LRU\n\n";
    std::cout << "=== Results ===\n";
    std::cout << "Accesses: " << total    << "\n";
    std::cout << "Hits:     " << s.hits   << "  (" << hitRate  << "%)\n";
    std::cout << "Misses:   " << s.misses << "  (" << missRate << "%)\n";
    return 0;
}