#ifndef DA_PROJ2_RESULTVIEW_H
#define DA_PROJ2_RESULTVIEW_H

#include "controllers/RegisterAllocatorController.h"
#include <string>

/**
 * @file ResultView.h
 * @brief Provides functions for displaying allocation results and prompting for filenames.
 */

/**
 * @brief Prompts the user for a filename and resolves its path.
 * @param label The text to display to the user.
 * @param preferredSub Optional preferred subdirectory to look into.
 * @return The resolved absolute or relative path to the file.
 */
std::string promptFilename(const std::string& label, const char* preferredSub = nullptr);

/**
 * @brief Prints the register/memory assignments for each web.
 * @param result The allocation result to display.
 */
void printAllocationAssignments(const AllocationResult& result);

/**
 * @brief Prints a brief summary of the allocation (registers used and assignments).
 * @param result The allocation result to display.
 */
void printAllocationSummary(const AllocationResult& result);

/**
 * @brief Prints the full allocation results, including web details and feasibility.
 * @param ctrl The controller containing the allocation results.
 */
void printAllocationResult(const RegisterAllocatorController& ctrl);

#endif // DA_PROJ2_RESULTVIEW_H
