//Jessica Chammas- 826401167
//Baraa Erras - 825786501

#ifndef PAGETABLE_H
#define PAGETABLE_H
#include <vector>
#include <cstdint>
#include <map>
#include "Map.h"

class Level;

class PageTable {
public:
    int levelCount;
    std::vector<unsigned int> bitMask;
    std::vector<int> bitShift;
    std::vector<int> entryCount;
    Level* rootNodePtr;
    unsigned int pageHits;
    unsigned int frameCounter = 0;

    // Frame management attributes
    std::vector<int> frameVPN; // Track the VPN each frame maps to
    std::vector<unsigned short> frameBitstring; // Bitstring for aging
    std::vector<bool> frameValid; // Whether the frame is occupied
    std::vector<unsigned int> frameLastAccess; // Last access time for aging
    unsigned int currentTime; // Current time for "aging" the frames
    int totalFrames; // Total number of physical frames default
    int bitstring_interval;

     // Setter method for bitstring_interval
    void setBitstringInterval(int interval) {
        bitstring_interval = interval;
    }



    PageTable(int lc, const std::vector<unsigned int>& bitmasks, const std::vector<int>& shifts, const std::vector<int>& entryCounts);
    ~PageTable();

    //Replacement methods
    unsigned int accessPage(unsigned int vpn);
    void updateAging();
    int selectVictimFrame();
    void replacePage(int frameIndex, unsigned int vpn);


    void insertMapForVpn2Pfn(unsigned int virtualAddress);
    Map* searchMappedPfn(unsigned int virtualAddress);
    unsigned int extractVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift);
    unsigned int calculateTotalBytes() const;
    int selectFrameForMapping(unsigned int vpn);


};

#endif // PAGETABLE_H
