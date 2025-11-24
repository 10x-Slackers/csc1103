#ifndef AUDIO_H
#define AUDIO_H

#include <gst/gst.h>
#include <stdbool.h>

#define THEME_MUSIC_PATH "/org/csc1103/tictactoe/theme.ogg"
#define CLICK_SOUND_PATH "/org/csc1103/tictactoe/click.ogg"
#define WIN_SOUND_PATH "/org/csc1103/tictactoe/win.ogg"

typedef struct {
  GstElement* theme;
  GstElement* click;
  GstElement* win;
  gboolean enabled;
  gboolean initialised;
} AudioState;

typedef enum { SOUND_THEME, SOUND_CLICK, SOUND_WIN } SoundType;

/**
 * @brief Initialise the audio system.
 * @return 0 on success, -1 on failure.
 */
int init_audio();

/**
 * @brief Cleanup and free audio resources.
 */
void cleanup_audio();

/**
 * @brief Play a sound effect.
 * @param type The type of sound to play.
 */
void play_sound(SoundType type);

/**
 * @brief Stop a currently playing sound.
 * @param type The type of sound to stop.
 */
void stop_sound(SoundType type);

/**
 * @brief Enable or disable audio globally.
 * @param enabled True to enable audio, false to disable.
 */
void set_audio_enabled(bool enabled);

#endif  // AUDIO_H
