#ifndef DA_PROJ2_LOADRANGESCONTROLLER_H
#define DA_PROJ2_LOADRANGESCONTROLLER_H

#include "structures/Structures.h"
#include <vector>
#include <string>

/**
 * @file LoadRangesController.h
 * @brief Controller responsible for loading live ranges and register configurations from files.
 */

/**
 * @class LoadRangesController
 * @brief Manages the state of loaded data for the register allocator.
 *
 * This class provides methods to parse input files (live ranges and register counts)
 * and store them for further processing by the allocation algorithms.
 */
class LoadRangesController {
public:
    std::vector<LiveRange> liveRanges; ///< Vector of parsed live ranges.
    RegisterConfig         config;      ///< Loaded register allocation configuration.
    bool                   rangesReady = false; ///< Flag indicating if ranges are loaded.
    bool                   configReady = false; ///< Flag indicating if config is loaded.

    /**
     * @brief Loads and parses live ranges from a text file.
     * @param filename Path to the file containing live range data.
     * @note Time complexity: O(L) where L is the number of lines in the file.
     */
    void loadRangesFromFile(const std::string& filename);

    /**
     * @brief Loads and parses register configuration from a text file.
     * @param filename Path to the file containing register/algorithm settings.
     * @note Time complexity: O(L) where L is the number of lines in the file.
     */
    void loadConfigFromFile(const std::string& filename);

    /** @return Number of live ranges currently loaded. */
    size_t getRangeCount()                        const { return liveRanges.size(); }

    /** @return Number of available physical registers from the configuration. */
    int    getNumRegisters()                      const { return config.numRegisters; }

    /** @return True if both ranges and configuration have been successfully loaded. */
    bool   isReady()                              const { return rangesReady && configReady; }

    /** @return Reference to the vector of loaded live ranges. */
    const std::vector<LiveRange>& getLiveRanges() const { return liveRanges; }

    /** @return Reference to the loaded register configuration. */
    const RegisterConfig&         getConfig()     const { return config; }
};

#endif // DA_PROJ2_LOADRANGESCONTROLLER_H
