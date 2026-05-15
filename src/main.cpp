#include <iostream>

#include "view/BatchMode.h"
#include "view/InteractiveMode.h"
#include "view/Menu.h"
using std::cout;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (argc == 5 && string(argv[1]) == "-b") BatchMode::run(argv[2], argv[3], argv[4]);
        else {
            cout << "usage:" << argv[0] << " -b ranges.txt registers.txt allocation.tx\n";
            return 1;
        }
        return 0;
    }
    InteractiveMode::run();
    return 0;
}
