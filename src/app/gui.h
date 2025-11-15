#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define APP_ID "com.csc1103.tictactoe"
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 900
#define CSS_PATH "resources/style.css"
#define BUILDER_PATH "resources/main_menu.ui"
#define APP_TITLE "Group 3 Tic-Tac-Toe"

/* UI Constants */
#define AI_MOVE_DELAY_MS 500
#define SCORE_BUFFER_SIZE 32
#define BUTTON_NAME_SIZE 20

/**
 * @brief Navigate to a specific page in the stack.
 * @param stack Pointer to the GtkStack.
 * @param page_name Name of the page to navigate to.
 */
void navigate_to(GtkStack* stack, const char* page_name);

#endif  // GUI_H
