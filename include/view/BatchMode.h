#ifndef DA_PROJ2_BATCHMODE_H
#define DA_PROJ2_BATCHMODE_H

#include <string>
using std::string;

/**
 * @class BatchMode
 * @brief Handles batch (non-interactive) execution of the register allocator.
 *
 * Invoked via: ./myProg -b ranges.txt registers.txt allocation.txt
 */
class BatchMode {
public:
    /**
     * @brief Runs the full register allocation pipeline in batch mode.
     *
     * Parses inputs, builds webs, constructs the interference graph,
     * runs the selected coloring algorithm, and writes the result.
     *
     * @param ranges Path to the live ranges input file.
     * @param registers Path to the register configuration file.
     * @param allocation Path for the output allocation file.
     *
     * @throws std::runtime_error if input files cannot be opened.
     * @throws std::invalid_argument if input format is invalid.
     *
     * @note Time complexity: O(W^2 * P) dominated by interference graph
     *       construction, where W = number of webs and P = max program points per web.
     */
    static void run(const string& ranges,const string& registers,const string& allocation);
};

#endif // DA_PROJ2_BATCHMODE_H