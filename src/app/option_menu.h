#ifndef OPTION_MENU_H
#define OPTION_MENU_H

#include <gtk/gtk.h>

/**
 * @brief Initialize the player selection UI and connect signals.
 *
 * Sets up the player selection UI for choosing whether to play as X or O.
 * Connects signal handlers for player selection buttons.
 *
 * @param builder Pointer to the GtkBuilder instance.
 * @param stack Pointer to the GtkStack for navigation.
 * @return 0 on success, -1 on failure.
 */
int player_select(GtkBuilder* builder, GtkStack* stack);

/**
 * @brief Initialize the difficulty selection UI and connect signals.
 *
 * Sets up the difficulty selection UI for choosing easy, medium, or hard
 * difficulty. Connects signal handlers for difficulty selection buttons.
 *
 * @param builder Pointer to the GtkBuilder instance.
 * @param stack Pointer to the GtkStack for navigation.
 * @return 0 on success, -1 on failure.
 */
int difficulty_select(GtkBuilder* builder, GtkStack* stack);

#endif  // OPTION_MENU_H
