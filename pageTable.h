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

    // Frame management attributes
    std::vector<int> frameVPN; // Track the VPN each frame maps to
    std::vector<unsigned short> frameBitstring; // Bitstring for aging
    std::vector<bool> frameValid; // Whether the frame is occupied
    std::vector<unsigned int> frameLastAccess; // Last access time for aging
    unsigned int currentTime; // Current time for "aging" the frames
    int totalFrames; // Total number of physical frames
    int bitstring_interval;

     // Setter method for bitstring_interval
    void setBitstringInterval(int interval) {
        bitstring_interval = interval;
    }

    PageTable(int lc, const std::vector<unsigned int>& bitmasks, const std::vector<int>& shifts, const std::vector<int>& entryCounts);
    ~PageTable();

    //Replacement methods
    void accessPage(unsigned int vpn);
    void updateAging();
    int selectVictimFrame();
    void replacePage(int frameIndex, unsigned int vpn);


    void insertMapForVpn2Pfn(unsigned int virtualAddress, int frame);
    Map* searchMappedPfn(unsigned int virtualAddress);
    unsigned int extractVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift);
};

#endif // PAGETABLE_H