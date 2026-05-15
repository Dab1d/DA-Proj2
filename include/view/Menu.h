#ifndef DA_PROJ2_MENU_H
#define DA_PROJ2_MENU_H

/**
 * @class Menu
 * @brief UI utilities for the interactive command-line interface.
 */
class Menu {
public:
    /**
     * @brief Displays the main menu options.
     * @note Time complexity: O(1).
     */
    static void showMenu();

    /**
     * @brief Reads a valid integer from user input.
     *
     * Repeatedly prompts until a valid integer is entered.
     *
     * @return The validated integer entered by the user.
     * @note Time complexity: O(k), where k is the number of invalid attempts.
     */
    static int getSafeInteger();

    /**
     * @brief Waits for the user to press Enter to return to the main menu.
     * @note Time complexity: O(k), where k is the number of attempts until
     *       the user presses Enter.
     */
    static void waitForReturnToMenu();
};

#endif // DA_PROJ2_MENU_H