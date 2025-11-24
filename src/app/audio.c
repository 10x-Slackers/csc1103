/**
 * @file audio.c
 * @brief Audio system using GStreamer for sound effects and theme music.
 * @authors commit2main
 * @date 2025-11-15
 */
#include "audio.h"

#include <gio/gio.h>

/* Global audio playback state */
static AudioState g_audio = {0};

/**
 * @brief Loop the music on end-of-stream if enabled.
 * @param bus The GstBus pointer.
 * @param msg The GstMessage pointer.
 * @param data Pointer to a gboolean indicating if looping is enabled.
 * @return TRUE to continue receiving messages.
 */
static gboolean on_eos(GstBus* bus, GstMessage* msg, gpointer data) {
  const gboolean* loop = (gboolean*)data;
  // Loop the music on end of stream if enabled
  if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS && *loop) {
    GstElement* playbin = GST_ELEMENT(GST_MESSAGE_SRC(msg));
    gst_element_seek_simple(playbin, GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT, 0);
    gst_element_set_state(playbin, GST_STATE_PLAYING);
  }

  return TRUE;
}

/**
 * @brief Create a GStreamer player for a specified resource.
 * @param resource_path The resource path to the audio file.
 * @param loop Whether to loop the audio when it ends.
 * @return Pointer to the created GstElement player, or NULL on failure.
 */
static GstElement* create_player(const char* resource_path, gboolean loop) {
  // Load audio data from GResource
  GBytes* audio_data = g_resources_lookup_data(resource_path, 0, NULL);
  if (!audio_data) return NULL;
  // Create GStreamer playbin element
  GstElement* playbin = gst_element_factory_make("playbin", NULL);
  if (!playbin) {
    g_bytes_unref(audio_data);
    return NULL;
  }
  // Set the URI to the base64-encoded audio data
  gsize data_size;
  gconstpointer data = g_bytes_get_data(audio_data, &data_size);
  gchar* uri = g_strdup_printf("data:audio/mpeg;base64,%s",
                               g_base64_encode((const guchar*)data, data_size));
  g_object_set(playbin, "uri", uri, NULL);
  g_free(uri);
  g_bytes_unref(audio_data);

  if (loop) {
    // Set up bus message handler for looping
    GstBus* bus = gst_element_get_bus(playbin);
    static gboolean loop_flag = TRUE;
    gst_bus_add_watch(bus, on_eos, &loop_flag);
    gst_object_unref(bus);
  }

  return playbin;
}

static GstElement* get_player(SoundType type) {
  switch (type) {
    case SOUND_THEME:
      return g_audio.theme;
    case SOUND_CLICK:
      return g_audio.click;
    case SOUND_WIN:
      return g_audio.win;
    default:
      return NULL;
  }
}

int init_audio() {
  if (g_audio.initialised) return 0;
  gst_init(NULL, NULL);
  // Create audio players
  g_audio.theme = create_player(THEME_MUSIC_PATH, TRUE);
  g_audio.click = create_player(CLICK_SOUND_PATH, FALSE);
  g_audio.win = create_player(WIN_SOUND_PATH, FALSE);
  if (!g_audio.theme || !g_audio.click || !g_audio.win) {
    cleanup_audio();
    return -1;
  }
  // Set initial states
  g_audio.enabled = TRUE;
  g_audio.initialised = TRUE;
  return 0;
}

void cleanup_audio() {
  if (!g_audio.initialised) return;
  // Cleanup audio players
  gst_element_set_state(g_audio.theme, GST_STATE_NULL);
  gst_object_unref(g_audio.theme);
  gst_element_set_state(g_audio.click, GST_STATE_NULL);
  gst_object_unref(g_audio.click);
  gst_element_set_state(g_audio.win, GST_STATE_NULL);
  gst_object_unref(g_audio.win);
  // Unset initialised flag
  g_audio.initialised = FALSE;
}

void play_sound(SoundType type) {
  if (!g_audio.initialised || !g_audio.enabled) return;
  // Get the appropriate player
  GstElement* player = get_player(type);
  if (!player) return;
  // Play sound from start if not theme music
  if (type != SOUND_THEME) {
    gst_element_set_state(player, GST_STATE_NULL);
    gst_element_seek_simple(player, GST_FORMAT_TIME,
                            GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT, 0);
  }
  gst_element_set_state(player, GST_STATE_PLAYING);
}

void stop_sound(SoundType type) {
  if (!g_audio.initialised) return;
  GstElement* player = get_player(type);
  if (!player) return;
  gst_element_set_state(player, GST_STATE_PAUSED);
}

void set_audio_enabled(bool enabled) {
  g_audio.enabled = enabled;
  if (!enabled) {
    stop_sound(SOUND_THEME);
  } else {
    play_sound(SOUND_THEME);
  }
}
