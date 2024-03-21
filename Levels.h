#ifndef LEVELS_H
#define LEVELS_H
#include "Map.h"

class PageTable;

class PageTable;

class Level {
public:
    Level** nextLevels;
    Map* maps;
    int depth;
    PageTable* pageTablePtr;

    Level(PageTable* pt, int d, int entryCount);
    ~Level();

    void insertMapForVpn2Pfn( unsigned int vpn, int frame);
    Map* searchMappedPfn(unsigned int virtualAddress);
};

#endif //LEVELS_H