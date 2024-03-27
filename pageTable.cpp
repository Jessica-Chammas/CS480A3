//Jessica Chammas- 826401167
//Baraa Erras -

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
void PageTable::insertMapForVpn2Pfn(unsigned int virtualAddress) {
    // Check if vpn already exists in physical memory 
    if(searchMappedPfn(virtualAddress) != nullptr){
        pageHits++;
        return;
    }

    // If not then find proper frame 
    unsigned int frameIndex = selectFrameForMapping(virtualAddress);

    // No frames left page replacement 
    if(frameIndex == -1) return;

    // Call on Levels insert 
    rootNodePtr -> insertMapForVpn2Pfn(virtualAddress, frameIndex);
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
unsigned int PageTable::accessPage(unsigned int vpn){
  bool pageInMemory = false;
unsigned int pfn = 0;
 for (int i = 0; i < totalFrames; ++i) {
        if (frameValid[i] && frameVPN[i] == vpn) { // Page hit
            frameBitstring[i] |= (1 << 15); // Set MSB
            frameLastAccess[i] = currentTime++;
pfn = i;
            pageInMemory = true;
            break;
        }
    }
    if (!pageInMemory) { // Page fault
        int victimIdx = selectVictimFrame();
        if (victimIdx != -1) { // Need to replace a page
            replacePage(victimIdx, vpn);
pfn = victimIdx;
        } 
    }
return pfn;
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

int PageTable::selectFrameForMapping(unsigned int vpn) {
    // Check for a free frame
    for (int i = 0; i < totalFrames; i++) {
        if (!frameValid[i]) {
            // Free frame found 
            frameValid[i] = true;
            frameCounter++;
            return i; 
        }
    }

    // Keep -1 for now if no frame was found 
    return -1;
}

unsigned int PageTable::calculateTotalBytes() const {
    unsigned int totalBytes = 0;
    
    // Include the size of the root node itself
    totalBytes += sizeof(Level);
    
    // Recursive function to calculate 
    std::function<void(const Level*)> calculateSize = [&](const Level* level) {
        if (!level) return;
        
        // If it's a leaf node, calculate the size based on the number of mappings
        if (level->depth == levelCount - 1) {
            totalBytes += sizeof(Map) * entryCount[level->depth];
        } else {
            // For non-leaf nodes, include the size of pointers to next levels
            totalBytes += sizeof(Level*) * entryCount[level->depth];
            // Recursively calculate the size for each next level
            for (int i = 0; i < entryCount[level->depth]; i++) {
                if (level->nextLevels[i] != nullptr) {
                    totalBytes += sizeof(Level);
                    calculateSize(level->nextLevels[i]);
                }
            }
        }
    };

    // Start the recursive calculation from the root node
    calculateSize(rootNodePtr);
    
    return totalBytes;
}
