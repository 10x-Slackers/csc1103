#ifndef OPTION_MENU_H
#define OPTION_MENU_H

#include <gtk/gtk.h>

/**
 * @brief Update player AI hint visibility based on game mode.
 * @param builder Pointer to the GtkBuilder.
 */
void update_ai_hint_visibility(GtkBuilder* builder);

/**
 * @brief Initalise player selection UI and connect signals.
 *
 * Sets up the player selection components for choosing whether to play as X or
 * O.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 */
void player_select(GtkBuilder* builder, GtkStack* stack);

/**
 * @brief Initalise difficulty selection UI and connect signals.
 *
 * Sets up the difficulty selection components for choosing easy, medium, or
 * hard difficulty.
 *
 * @param builder Pointer to the GtkBuilder.
 * @param stack Pointer to the GtkStack.
 */
void difficulty_select(GtkBuilder* builder, GtkStack* stack);

#endif  // OPTION_MENU_H
