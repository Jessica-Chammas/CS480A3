#ifndef PAGETABLE_H
#define PAGETABLE_H
#include <vector>
#include <cstdint>
#include "Map.h"

class Level;

class PageTable {
public:
    int levelCount;
    std::vector<unsigned int> bitMask;
    std::vector<int> bitShift;
    std::vector<int> entryCount;
    Level* rootNodePtr;

    PageTable(int lc, const std::vector<unsigned int>& bitmasks, const std::vector<int>& shifts, const std::vector<int>& entryCounts);
    ~PageTable();

    void insertMapForVpn2Pfn(unsigned int virtualAddress, int frame);
    Map* searchMappedPfn(unsigned int virtualAddress);
    unsigned int extractVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift);
};

#endif // PAGETABLE_H