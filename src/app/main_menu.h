#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <gtk/gtk.h>

#define AUDIO_ON_RESOURCE "/org/csc1103/tictactoe/volume-up-rounded.svg"
#define AUDIO_OFF_RESOURCE "/org/csc1103/tictactoe/volume-off-rounded.svg"

/**
 * @brief Initialise main menu UI and connect signals.
 *
 * Sets up the main menu UI components including one-player and two-player
 * buttons, and audio toggle button.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 */
void main_menu(GtkBuilder* builder, GtkStack* stack);

#endif  // MAIN_MENU_H
