import gi

gi.require_version("Gtk", "4.0")
from gi.repository import Gtk, Gdk  # noqa

from ui import main_menu  # noqa

TTT_GRID_SIZE = 150
SPACING = 5
MARGIN = 30
WINDOW_SIZE = (TTT_GRID_SIZE * 3) + (SPACING * 2) + (MARGIN * 2)

# Track current player
current_player = "O"

# 2D list to store references to each button in the 3x3 grid
buttons_list = []
board_state = ["", "", "", "", "", "", "", "", ""]


def main(app):
    """
    Entry point of the Tic Tac Toe application.
    Resets the board and current player before launching the game.
    """
    global board_state, current_player, buttons_list

    # Reset game state
    current_player = "O"
    board_state = [""] * 9
    buttons_list = []

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
        turn_label.set_label("Player X's Turn")
        turn_label.add_css_class("turn-x")
    else:
        turn_label.set_label("Player O's Turn")
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

            # Attach button to grid
            grid.attach(button, col, row, 1, 1)

            # Add to buttons_list in order (row-major)
            buttons_list.append(button)

    # Add the grid to the vertical box
    vbox.append(grid)


def control_buttons(vbox, app):
    """
    Adds 'Back to Main Menu' (bottom-left) and 'Quit Game' (bottom-right) buttons.
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

    window = button.get_root()
    if window is not None:
        app = window.get_application()
        main_menu.main_menu(app)
        window.close()


def on_blink(button):
    print("Blink Clicked")


def on_button_clicked(button):
    """
    Callback for when a grid button is clicked.
    """
    global current_player, turn_label, buttons_list, board_state
    board_state_index = buttons_list.index(button)
    print(board_state_index)

    button.set_label(current_player)
    button.set_sensitive(False)

    if current_player == "X":
        button.add_css_class("x-style")
        board_state[board_state_index] = "X"
        current_player = "O"
    else:
        button.add_css_class("o-style")
        board_state[board_state_index] = "O"
        current_player = "X"

    update_turn_label()


def apply_css():
    """
    Apply CSS styling for the X and O symbols.
    """
    css = b"""
    .x-style {
        font-size: 50px;
        color: red;
    }

    .o-style {
        font-size: 50px;
        color: blue;
    }

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
    """
    css_provider = Gtk.CssProvider()
    css_provider.load_from_data(css)
    display = Gdk.Display.get_default()
    Gtk.StyleContext.add_provider_for_display(
        display, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
    )


if __name__ == "__main__":
    main()
