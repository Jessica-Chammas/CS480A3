#include <iostream>
#include "log_helpers.h"
#include "Levels.h"
#include "pageTable.h"

// Level Constructor
Level::Level(PageTable* pt, int dep, int entryCount){
    // Initialization
    pageTablePtr = pt;
    depth = dep;
    nextLevels = nullptr;
    maps = nullptr;

    // Leaf
    if (depth == pt->levelCount - 1) {
        maps = new Map[entryCount];
    } 
    // else non-leaf
    else { 
        nextLevels = new Level*[entryCount];
        for (int i = 0; i < entryCount; i++) {
            nextLevels[i] = nullptr;
        }
    }
}

// Level Destructor
Level::~Level() {
    if (depth == pageTablePtr->levelCount - 1) {
        delete[] maps;
    } else {
        for (int i = 0; i < pageTablePtr->entryCount[depth]; i++) {
            delete nextLevels[i];
        }
        delete[] nextLevels;
    }
}

void Level::insertMapForVpn2Pfn(unsigned int vpn, int frame) {
    // Calculate the index at this level using bit masking and shifting
    uint32_t index = (vpn & pageTablePtr->bitMask[depth]) >> pageTablePtr->bitShift[depth];

    // Leaf node
    if (depth == pageTablePtr->levelCount - 1) { 
        maps[index].frame = frame;
        maps[index].valid = true;
    } 
    // Non-leaf node
    else { 
        // If the next level doesn't exist, create it
        if (nextLevels[index] == nullptr) { 
            nextLevels[index] = new Level(pageTablePtr, depth + 1, pageTablePtr->entryCount[depth + 1]);
        }
        // Recursively call insert on the next level
        nextLevels[index]->insertMapForVpn2Pfn(vpn, frame);
    }
}

Map* Level::searchMappedPfn(unsigned int virtualAddress) {
    // Calculate the index at this level using bit masking and shifting
    uint32_t index = (virtualAddress & pageTablePtr->bitMask[depth]) >> pageTablePtr->bitShift[depth];

    // Leaf node
    if (depth == pageTablePtr->levelCount - 1) { 
        if (maps[index].valid) {
            // Return the mapping if valid
            return &maps[index]; 
        }
        // No valid mapping found
        return nullptr; 
    } 
    // Non-leaf node
    else {
        if (nextLevels[index] == nullptr) {
            return nullptr; // No further levels, so no mapping
        }
        // Recursively search the next level
        return nextLevels[index]->searchMappedPfn(virtualAddress);
    }
}
