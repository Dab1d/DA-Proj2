#ifndef DA_PROJ2_PATHUTILS_H
#define DA_PROJ2_PATHUTILS_H

#include <filesystem>
#include <string>

// Walk up from CWD to find the directory that contains a "datasets/" folder.
std::filesystem::path findProjectRoot();

// Resolve a user-typed filename to a full path by searching dataset directories.
// preferredSub: search this subdirectory first (e.g. "datasets/basic/ranges").
std::string resolveFilePath(const std::string& input, const char* preferredSub = nullptr);

#endif // DA_PROJ2_PATHUTILS_H
