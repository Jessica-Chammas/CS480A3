#ifndef MAP_H
#define MAP_H

#include <cstdint>

struct Map {
    uint32_t frame;
    bool valid;
    Map() : frame(static_cast<uint32_t>(-1)), valid(false) {}
};

#endif // MAP_H