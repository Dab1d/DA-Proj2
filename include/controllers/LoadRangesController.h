#ifndef DA_PROJ2_LOADRANGESCONTROLLER_H
#define DA_PROJ2_LOADRANGESCONTROLLER_H

#include "structures/Structures.h"
#include <vector>
#include <string>

class LoadRangesController {
public:
    std::vector<LiveRange> liveRanges;
    RegisterConfig         config;
    bool                   rangesReady = false;
    bool                   configReady = false;

    void loadRangesFromFile(const std::string& filename);
    void loadConfigFromFile(const std::string& filename);

    size_t getRangeCount()   const { return liveRanges.size(); }
    int    getNumRegisters() const { return config.numRegisters; }
    bool   isReady()         const { return rangesReady && configReady; }
};

#endif // DA_PROJ2_LOADRANGESCONTROLLER_H
