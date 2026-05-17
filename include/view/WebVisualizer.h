#pragma once
#include "controllers/RegisterAllocatorController.h"
#include "structures/Structures.h"
#include <vector>

/**
 * @brief Runs buildWebs + buildInterferenceGraph with a coloured terminal animation,
 *        then calls alloc.build() to populate controller state for allocation.
 */
void buildWithVisualization(RegisterAllocatorController& alloc,
                            const std::vector<LiveRange>& ranges,
                            const RegisterConfig& cfg);
