#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <gtk/gtk.h>

/**
 * @brief Initalise main menu UI and connect signals.
 *
 * Sets up the main menu UI components including one-player and two-player
 * buttons, and audio toggle button.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 */
void main_menu(GtkBuilder* builder, GtkStack* stack);

#endif  // MAIN_MENU_H
