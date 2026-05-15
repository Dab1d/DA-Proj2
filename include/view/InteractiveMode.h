#ifndef DA_PROJ2_INTERACTIVEMODE_H
#define DA_PROJ2_INTERACTIVEMODE_H

/**
 * @class InteractiveMode
 * @brief Handles the interactive menu-driven execution of the register allocator.
 *
 * Provides a loop where the user can load files, build webs,
 * run allocation algorithms, and display results step by step.
 */
class InteractiveMode {
public:
    /**
     * @brief Launches the interactive command-line menu loop.
     *
     * Displays options and processes user input until exit is selected.
     *
     * @note Time complexity: depends on user actions and selected algorithms.
     */
    static void run();
};

#endif // DA_PROJ2_INTERACTIVEMODE_H