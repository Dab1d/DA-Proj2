#include "utils/PathUtils.h"
#include <string>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

static bool isAllDigits(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

fs::path findProjectRoot() {
    fs::path current = fs::current_path();
    while (true) {
        if (fs::exists(current / "datasets") && fs::is_directory(current / "datasets"))
            return current;
        fs::path parent = current.parent_path();
        if (parent == current) break;
        current = parent;
    }
    return {};
}

std::string resolveFilePath(const std::string& input, const char* preferredSub) {
    static const fs::path root = findProjectRoot();

    static const char* kAllSubDirs[] = {
        "datasets/basic/ranges",
        "datasets/basic/registers",
        "datasets/input",
        "",
    };

    fs::path p(input);
    std::vector<fs::path> names = { p };
    if (!p.has_extension()) {
        names.push_back(fs::path(input + ".txt"));
        names.push_back(fs::path("ranges"    + input + ".txt"));
        names.push_back(fs::path("registers" + input + ".txt"));
    }
    if (isAllDigits(input)) {
        names.push_back(fs::path("ranges"    + input + ".txt"));
        names.push_back(fs::path("registers" + input + ".txt"));
    }

    std::vector<const char*> subdirs;
    if (preferredSub) subdirs.push_back(preferredSub);
    for (const char* s : kAllSubDirs)
        if (!preferredSub || std::string(s) != preferredSub)
            subdirs.push_back(s);

    for (const char* sub : subdirs) {
        for (const fs::path& name : names) {
            fs::path candidate = fs::path(sub) / name;
            if (fs::exists(candidate)) return candidate.string();
            if (!root.empty()) {
                fs::path abs = root / sub / name;
                if (fs::exists(abs)) return abs.string();
            }
        }
    }
    return input;
}
