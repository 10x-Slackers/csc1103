#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <gtk/gtk.h>

/**
 * @brief Initialize the main menu UI and connect signals.
 *
 * Sets up the main menu UI components including one-player and two-player
 * buttons, and audio toggle button. Connects signal handlers for user
 * interactions.
 *
 * @param builder Pointer to the GtkBuilder instance.
 * @param stack Pointer to the GtkStack for navigation.
 * @return 0 on success, -1 on failure.
 */
int main_menu(GtkBuilder* builder, GtkStack* stack);

#endif  // MAIN_MENU_H
