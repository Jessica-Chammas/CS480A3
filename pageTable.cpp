#include <iostream>
#include "log_helpers.h"
#include "pageTable.h"
#include "Levels.h"


// Define constants
const int INITIAL_BITSTRING_VALUE = 1 << 15; // Initial bitstring value for newly loaded pages

// Constructor
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

//NFU with Aging - Page Replacement code
void PageTable::accessPage(unsigned int vpn){
  bool pageInMemory = false;
 for (int i = 0; i < totalFrames; ++i) {
        if (frameValid[i] && frameVPN[i] == vpn) { // Page hit
            frameBitstring[i] |= (1 << 15); // Set MSB
            frameLastAccess[i] = currentTime++;
            pageInMemory = true;
            break;
        }
    }
    if (!pageInMemory) { // Page fault
        int victimIdx = selectVictimFrame();
        if (victimIdx != -1) { // Need to replace a page
            replacePage(victimIdx, vpn);
        } 
    }
}
void PageTable::updateAging(){
   for(int i = 0; i < totalFrames; ++i){
    if(frameValid[i]){
      frameBitstring[i] >>=1; //right shift for aging
      if (currentTime - frameLastAccess[i] <= bitstring_interval) {
                frameBitstring[i] |= (1 << 15); 
            }
   }
}
}

//Selects the victim frame based on NFU w aging
int PageTable::selectVictimFrame(){
    int victimIdx = -1; 
    unsigned short lowestBitstring = std::numeric_limits<unsigned short>::max();
    unsigned int oldestAccess = std::numeric_limits<unsigned int>::max();


    //iterates through the frames to find the victim
    for (int i = 0; i < totalFrames; ++i) {
        if (frameValid[i] && (frameBitstring[i] < lowestBitstring || 
            (frameBitstring[i] == lowestBitstring && frameLastAccess[i] < oldestAccess))) {
            
            //update the victims information
            lowestBitstring = frameBitstring[i];
            oldestAccess = frameLastAccess[i];
            victimIdx = i;
        }
    }

    return victimIdx;
}


//new VPN replace page at the frameIndex
void PageTable::replacePage(int frameIndex, unsigned int vpn){
    // Replace the page
    frameVPN[frameIndex] = vpn;
    frameValid[frameIndex] = true;
    frameBitstring[frameIndex] = (1 << 15); // Set MSB as it's a new page
    frameLastAccess[frameIndex] = currentTime;

    //now we have to go through Page table/ level tree  to invalidate old mapping

   Level* currentLevel = rootNodePtr;

    // Traverse through the levels to find and update the mapping
    for (int i = 0; i < levelCount; ++i) {
        Map* mapping = currentLevel->searchMappedPfn(vpn);
        if (mapping != nullptr) {
            // Found the mapping, update it
            mapping->frame = frameIndex; // Update frame number
            mapping->valid = true; // Mark as valid
            // No need to update bitstring or last access time for internal levels
            return;
        } else {
            // Move to the next level
            unsigned int index = extractVPNFromVirtualAddress(vpn, bitMask[i], bitShift[i]);
            if (currentLevel->nextLevels[index] != nullptr) {
                currentLevel = currentLevel->nextLevels[index];
            } else {
                // Level does not exist, create a new one
                currentLevel->nextLevels[index] = new Level(this, i + 1, entryCount[i + 1]);
                currentLevel = currentLevel->nextLevels[index];
            }
        }
    }
}