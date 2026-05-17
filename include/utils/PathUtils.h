#ifndef DA_PROJ2_PATHUTILS_H
#define DA_PROJ2_PATHUTILS_H

#include <filesystem>
#include <string>

/**
 * @file PathUtils.h
 * @brief Utility functions for project-relative path resolution.
 */

/**
 * @brief Locates the project root directory by searching for the "datasets" folder.
 * 
 * Walks up from the current working directory until a directory containing 
 * a "datasets/" subdirectory is found.
 * 
 * @return The filesystem path to the project root, or an empty path if not found.
 */
std::filesystem::path findProjectRoot();

/**
 * @brief Resolves a filename or partial path to a full path within the project.
 * 
 * Searches for the input filename in various project subdirectories 
 * (basic ranges, registers, etc.) to make input more user-friendly.
 * 
 * @param input The filename or path entered by the user.
 * @param preferredSub Optional subdirectory to prioritize in the search.
 * @return A string representing the resolved path to the file.
 */
std::string resolveFilePath(const std::string& input, const char* preferredSub = nullptr);

#endif // DA_PROJ2_PATHUTILS_H
