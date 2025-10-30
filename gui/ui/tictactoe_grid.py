"""
Missing Functionalities (Implement after logic combined)
- Updating of Scoreboard when a Player wins
- After the game ends, player can click on any of the buttons to start the next game
- When the player goes back to the main menu, the score resets back to 0
- Audio for when the game has a winner

Known Bugs
- When the Grid buttons blinks, players able to click it to change

"""

import random
import threading

import gi

gi.require_version("Gtk", "4.0")
gi.require_version("Gst", "1.0")
from gi.repository import Gtk, Gdk, GLib, Gst  # noqa
from ui import main_menu  # noqa
from contextlib import suppress  # noqa

TTT_GRID_SIZE = 150
SPACING = 5
MARGIN = 30
WINDOW_SIZE = (TTT_GRID_SIZE * 3) + (SPACING * 2) + (MARGIN * 2)

# Track current player, game mode & score
current_player = "O"
game_mode = "1P"
score_x_label = 0
score_tie_label = 0
score_o_label = 0

# 2D list to store references to each button in the 3x3 grid
buttons_list = []
board_state = ["", "", "", "", "", "", "", "", ""]

Gst.init(None)


def main(app, mode="1P"):
    """
    Entry point of the Tic Tac Toe application.
    Resets the board and current player before launching the game.
    """
    global game_mode
    game_mode = mode
    reset_game()
    launch_game(app)


def launch_game(app):
    global turn_label
    apply_css()
    window = Gtk.ApplicationWindow(application=app)
    window.set_title("Tic Tac Toe Grid")
    window.set_default_size(WINDOW_SIZE, WINDOW_SIZE)
    window.set_resizable(False)

    # Create main vertical layout
    vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=10)
    grid = Gtk.Grid()

    main_container(vbox)
    create_turn_label(vbox)
    set_game_grid(grid, vbox)
    create_scoreboard(vbox)
    control_buttons(vbox, app)

    window.set_child(vbox)
    window.present()


def main_container(vbox):
    vbox.set_margin_top(MARGIN)
    vbox.set_margin_bottom(MARGIN)
    vbox.set_margin_start(MARGIN)
    vbox.set_margin_end(MARGIN)


def create_turn_label(vbox):
    """Create the 'Current Player' label and add it to the top of the layout."""
    global turn_label

    turn_label = Gtk.Label(label=f"Player {current_player}'s Turn")
    turn_label.add_css_class("turn-o" if current_player == "O" else "turn-x")
    turn_label.set_halign(Gtk.Align.CENTER)
    vbox.append(turn_label)


def update_turn_label():
    """Update the turn label text and color based on current player."""
    if not turn_label:
        return

    # Remove previous style
    turn_label.remove_css_class("turn-x")
    turn_label.remove_css_class("turn-o")

    # Add the correct one
    if current_player == "X":
        if game_mode == "1P":
            turn_label.set_label("Computer's Turn")
        else:
            turn_label.set_label("Player 2's Turn")
        turn_label.add_css_class("turn-x")
    else:
        turn_label.set_label("Player 1's Turn")
        turn_label.add_css_class("turn-o")


def set_game_grid(grid, vbox):
    grid.set_row_spacing(SPACING)
    grid.set_column_spacing(SPACING)

    buttons_list.clear()

    # Create 3x3 grid of buttons
    for row in range(3):
        for col in range(3):
            button = Gtk.Button()
            button.set_size_request(TTT_GRID_SIZE, TTT_GRID_SIZE)
            button.connect("clicked", on_button_clicked)
            button.add_css_class("grid-button")

            # Attach button to grid
            grid.attach(button, col, row, 1, 1)

            # Add to buttons_list in order (row-major)
            buttons_list.append(button)

    # Add the grid to the vertical box
    vbox.append(grid)


def create_scoreboard(vbox):
    global game_mode, score_x_label, score_tie_label, score_o_label

    # Determine O player name
    opponent_label_text = "COMPUTER (X)" if game_mode == "1P" else "PLAYER 2 (X)"

    # Create scoreboard container
    scoreboard_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=40)
    scoreboard_box.set_halign(Gtk.Align.CENTER)

    # --- Labels for player names ---
    label_x = Gtk.Label(label="PLAYER (O)")
    label_tie = Gtk.Label(label="TIE")
    label_o = Gtk.Label(label=opponent_label_text)

    # Apply CSS for name labels
    label_x.add_css_class("scoreboard-label")
    label_x.add_css_class("score-o")

    label_tie.add_css_class("scoreboard-label")
    label_tie.add_css_class("score-tie")

    label_o.add_css_class("scoreboard-label")
    label_o.add_css_class("score-x")

    # --- Score labels (start at 0) ---
    score_x_label = Gtk.Label(label="0")
    score_x_label.add_css_class("score-value")
    score_x_label.add_css_class("score-o")

    score_tie_label = Gtk.Label(label="0")
    score_tie_label.add_css_class("score-value")
    score_tie_label.add_css_class("score-tie")

    score_o_label = Gtk.Label(label="0")
    score_o_label.add_css_class("score-value")
    score_o_label.add_css_class("score-x")

    # --- Combine each name and score in a vertical stack ---
    box_x = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)
    box_x.append(label_o)
    box_x.append(score_o_label)

    box_tie = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)
    box_tie.append(label_tie)
    box_tie.append(score_tie_label)

    box_o = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=5)
    box_o.append(label_x)
    box_o.append(score_x_label)

    # Add all to scoreboard container
    scoreboard_box.append(box_o)
    scoreboard_box.append(box_tie)
    scoreboard_box.append(box_x)

    # Append scoreboard to main layout
    vbox.append(scoreboard_box)


def control_buttons(vbox, app):
    """
    Adds 'Back to Main Menu' (bottom-left) and 'Blink' (bottom-right) buttons.
    """
    bottom_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    bottom_box.set_halign(Gtk.Align.FILL)
    bottom_box.set_valign(Gtk.Align.END)
    bottom_box.set_spacing(SPACING)

    # Back to Main Menu button (left)
    back_button = Gtk.Button(label="Back to Main Menu")
    back_button.connect("clicked", on_back_clicked, app)

    # Spacer to push Quit button to the right
    spacer = Gtk.Box()
    spacer.set_hexpand(True)

    blink_button = Gtk.Button(label="Blink!")
    blink_button.connect("clicked", on_blink)

    # Add to bottom_box
    bottom_box.append(back_button)
    bottom_box.append(spacer)
    bottom_box.append(blink_button)

    # Add to main vbox
    vbox.append(bottom_box)


def on_back_clicked(button, app):
    """
    Handle the 'Back to Main Menu' button click.
    """
    reset_game()

    global score_x_label, score_tie_label, score_o_label
    for reset_score in (score_x_label, score_tie_label, score_o_label):
        with suppress(Exception):
            reset_score.set_label("0")

    window = button.get_root()
    if window is not None:
        main_menu.main(app)
        window.close()


# Start of blink button features
def on_blink(button):
    """
    Randomly selects 3 buttons that have X or O and makes them blink.
    If fewer than 3 are filled, prints "Insufficient Buttons".
    """
    global board_state, buttons_list
    # For Understanding the line below. Will remove in future
    # filled_buttons = []
    # for index, xoro in enumerate(board_state):
    #     if xoro != "":
    #         filled_buttons.append(index)
    filled_buttons = [
        buttons_list[index] for index, xoro in enumerate(board_state) if xoro != ""
    ]

    if len(filled_buttons) < 3:
        print("Insufficient Buttons")
        return

    selected_buttons = random.sample(filled_buttons, 3)
    blink_buttons(selected_buttons)


def blink_buttons(selected_buttons):
    """
    Kick off blinking for the selected Gtk.Buttons.
    We temporarily enable them so GTK repaints the background for the disabled buttons,
    then toggle the 'blink' css class a few times and restore original sensitivity.
    """
    # remember original sensitivity so we can restore it later
    for btn in selected_buttons:
        # store attribute for restore (attach to object)
        try:
            btn._was_sensitive = btn.get_sensitive()
        except Exception:
            btn._was_sensitive = False
        # temporarily enable so style changes are visible
        with suppress(Exception):
            btn.set_sensitive(True)

    # start toggling
    toggle_blink(selected_buttons, 0)


def toggle_blink(selected_buttons, count=0):
    """
    Toggle blink class on/off. Using queue_draw()
    to force repaint so theme updates show.
    """
    for btn in selected_buttons:
        try:
            if count % 2 == 0:
                btn.add_css_class("blink")
            else:
                btn.remove_css_class("blink")
            # force a redraw of the widget so style changes show immediately
            with suppress(Exception):
                btn.queue_draw()
        except Exception as e:
            print("DEBUG: toggle_blink error for btn:", e)

    # schedule next toggle
    if count < 5:
        GLib.timeout_add(300, lambda: toggle_blink(selected_buttons, count + 1))
    else:
        GLib.timeout_add(150, lambda: finalize_blink(selected_buttons))


def finalize_blink(selected_buttons):
    """
    Clear blink class and restore original sensitivity. Force a final redraw.
    """
    for btn in selected_buttons:
        with suppress(Exception):
            btn.remove_css_class("blink")
            was = getattr(btn, "_was_sensitive", False)
            btn.set_sensitive(was)
            btn.queue_draw()


# End of blink button features


def on_button_clicked(button):
    """
    Callback for when a grid button is clicked.
    """
    global current_player, turn_label, buttons_list, board_state
    board_state_index = buttons_list.index(button)

    button.set_label(current_player)
    button.set_sensitive(False)

    if current_player == "X":
        button.add_css_class("x-style")
        board_state[board_state_index] = "X"
        play_click_sound(True)
        current_player = "O"
    else:
        button.add_css_class("o-style")
        board_state[board_state_index] = "O"
        play_click_sound(False)
        current_player = "X"

    update_turn_label()


def reset_game():
    """
    Fully reset game-related variables and button visuals.
    """
    global board_state, current_player, buttons_list

    current_player = "O"
    board_state = [""] * 9

    # Reset each button if it still exists
    for btn in buttons_list:
        try:
            btn.set_label("")
            btn.set_sensitive(True)
            btn.remove_css_class("x-style")
            btn.remove_css_class("o-style")
            btn.remove_css_class("blink")
        except Exception:
            pass  # Ignore if button no longer exists

    buttons_list.clear()


# Start of Audio Functions
def play_click_sound(is_x):
    """
    Plays a different sound depending on whether the player is X or O.
    This runs in a separate thread so the GUI doesn't freeze.
    """
    path = "/workspaces/csc1103/gui/ui/"
    sound_file = path + "x.mp3" if is_x else path + "o.mp3"
    threading.Thread(target=play_sound, args=(sound_file,), daemon=True).start()


def play_sound(file_path):
    """
    Initializes a GStreamer player to play an audio file (blocking in its own thread).
    """
    player = Gst.ElementFactory.make("playbin", "player")
    if not player:
        print("❌ Failed to create GStreamer playbin element.")
        return

    sink = create_audio_sink()
    player.set_property("audio-sink", sink)

    uri = GLib.filename_to_uri(file_path)
    player.set_property("uri", uri)
    player.set_state(Gst.State.PLAYING)

    # Run a loop to wait until the sound finishes playing
    loop = GLib.MainLoop()
    bus = player.get_bus()
    bus.add_signal_watch()
    bus.connect("message", handle_gst_message, player, loop)

    print(f"🎵 Playing sound: {file_path}")
    loop.run()


def create_audio_sink():
    """
    Creates and returns an appropriate audio sink for the system.
    """
    sink = Gst.ElementFactory.make("autoaudiosink", "audio_sink")
    if not sink:
        sink = Gst.ElementFactory.make("directsoundsink", "audio_sink")
    if not sink:
        print("⚠️ Warning: Could not create audio sink; playback may fail.")
    return sink


def handle_gst_message(bus, message, player, loop):
    """
    Handles messages from the GStreamer bus.
    """
    msg_type = message.type

    if msg_type == Gst.MessageType.EOS:
        player.set_state(Gst.State.NULL)
        loop.quit()
    elif msg_type == Gst.MessageType.ERROR:
        err, debug = message.parse_error()
        print("❌ GStreamer Error:", err.message)
        print("Debug info:", debug)
        player.set_state(Gst.State.NULL)
        loop.quit()


# End of Audio Functions


def apply_css():
    """
    Apply CSS styling for the entire Tic Tac Toe interface:
    - X and O symbol styles
    - Turn indicator styles
    - Grid button visuals + blinking animation
    - Scoreboard colors (Player X, Tie, Player O)
    - General button cleanup (no default GTK shadows)
    """
    css = b"""
    /* =============================
       SYMBOL STYLES (X / O in grid)
       ============================= */
    .x-style {
        font-size: 50px;
        color: red;
        font-weight: bold;
    }

    .o-style {
        font-size: 50px;
        color: blue;
        font-weight: bold;
    }

    /* =============================
       TURN LABEL STYLES
       ============================= */
    .turn-x {
        font-size: 24px;
        color: red;
        font-weight: bold;
    }

    .turn-o {
        font-size: 24px;
        color: blue;
        font-weight: bold;
    }

    /* =============================
       GRID BUTTON STYLES
       ============================= */
    .grid-button {
        background-color: white;
        border: 2px solid #ccc;
        border-radius: 8px;
        padding: 0;
        transition: background-color 0.25s ease-in-out, border 0.25s ease-in-out;
    }

    .grid-button:hover {
        border-color: #888;
        background-color: #f2f2f2;
    }

    /* Blinking effect for winning tiles */
    .grid-button.blink {
        background-color: yellow;
        border: 2px solid orange;
        box-shadow: 0 0 15px 5px yellow;
        transition: background-color 0.25s ease-in-out, box-shadow 0.25s ease-in-out;
    }

    /* Remove GTK default shadows */
    button {
        background-image: none;
        box-shadow: none;
    }

    /* =============================
       SCOREBOARD LABEL STYLES
       ============================= */
    .scoreboard-label {
        font-size: 18px;
        font-weight: bold;
    }

    .score-value {
        font-size: 24px;
        font-weight: bold;
    }

    .score-x {
        color: red;
    }

    .score-tie {
        color: black;
    }

    .score-o {
        color: blue;
    }
    """

    css_provider = Gtk.CssProvider()
    css_provider.load_from_data(css)
    display = Gdk.Display.get_default()
    Gtk.StyleContext.add_provider_for_display(
        display, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
    )


if __name__ == "__main__":
    main()
