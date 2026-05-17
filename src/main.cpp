#include <iostream>
#include <string>

#include "view/BatchMode.h"
#include "view/InteractiveMode.h"

static void printUsage(const char* program) {
    std::cerr << "Usage: " << program << " -b ranges.txt registers.txt allocation.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        InteractiveMode::run();
        return 0;
    }

    if (argc == 5 && std::string(argv[1]) == "-b") {
        return BatchMode::run(argv[2], argv[3], argv[4]) ? 0 : 1;
    }

    printUsage(argv[0]);
    return 1;
}
