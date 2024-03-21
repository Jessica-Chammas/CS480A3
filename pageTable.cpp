#include <iostream>
#include "log_helpers.h"
#include "pageTable.h"
#include "Levels.h"

// PageTable Constructor
PageTable::PageTable(int levelC, const std::vector<unsigned int>& mask, const std::vector<int>& shifts, const std::vector<int>& entryC){
    levelCount = levelC;
    bitMask = mask;
    bitShift = shifts;
    entryCount = entryC;
    rootNodePtr = new Level(this, 0, entryCount[0]);
}

// PageTable Destructor
PageTable::~PageTable() {
    delete rootNodePtr;
}

// Insert function
void PageTable::insertMapForVpn2Pfn(unsigned int virtualAddress, int frame) {
    // Call on Levels Insert
    rootNodePtr -> insertMapForVpn2Pfn(virtualAddress, frame);
}

// Search function
Map* PageTable::searchMappedPfn(unsigned int virtualAddress) {
  // Call on Levels Search
  return rootNodePtr->searchMappedPfn(virtualAddress);
}

// Extract function
unsigned int PageTable::extractVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift) {
  return (virtualAddress & mask) >> shift;
}
