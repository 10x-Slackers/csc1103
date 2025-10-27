import gi

gi.require_version("Gtk", "4.0")
from gi.repository import Gtk, Gdk  # noqa

TTT_GRID_SIZE = 150
SPACING = 5
MARGIN = 30
WINDOW_SIZE = (TTT_GRID_SIZE * 3) + (SPACING * 2) + (MARGIN * 2)

# Track current player
current_player = "O"

# 2D list to store references to each button in the 3x3 grid
buttons_list = []
board_state = ["", "", "", "", "", "", "", "", ""]


def main():
    """
    Entry point of the Tic Tac Toe application.
    """
    app = Gtk.Application(application_id="com.csc1103.TicTacToe")
    app.connect("activate", launch_game)
    app.run()


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
    control_buttons(vbox)

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

    # Create buttons manually (3x3 grid)
    b00 = Gtk.Button()
    b01 = Gtk.Button()
    b02 = Gtk.Button()
    b10 = Gtk.Button()
    b11 = Gtk.Button()
    b12 = Gtk.Button()
    b20 = Gtk.Button()
    b21 = Gtk.Button()
    b22 = Gtk.Button()

    buttons_list.extend([b00, b01, b02, b10, b11, b12, b20, b21, b22])

    # Set button sizes
    for b in buttons_list:
        b.set_size_request(TTT_GRID_SIZE, TTT_GRID_SIZE)
        b.connect("clicked", on_button_clicked)

    # Attach to grid manually
    grid.attach(b00, 0, 0, 1, 1)
    grid.attach(b01, 1, 0, 1, 1)
    grid.attach(b02, 2, 0, 1, 1)

    grid.attach(b10, 0, 1, 1, 1)
    grid.attach(b11, 1, 1, 1, 1)
    grid.attach(b12, 2, 1, 1, 1)

    grid.attach(b20, 0, 2, 1, 1)
    grid.attach(b21, 1, 2, 1, 1)
    grid.attach(b22, 2, 2, 1, 1)

    # Add grid to vbox
    vbox.append(grid)


def control_buttons(vbox):
    """
    Adds 'Back to Main Menu' (bottom-left) and 'Quit Game' (bottom-right) buttons.
    """
    bottom_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    bottom_box.set_halign(Gtk.Align.FILL)
    bottom_box.set_valign(Gtk.Align.END)
    bottom_box.set_spacing(SPACING)

    # Back to Main Menu button (left)
    back_button = Gtk.Button(label="Back to Main Menu")
    back_button.connect("clicked", on_back_clicked)

    # Spacer to push Quit button to the right
    spacer = Gtk.Box()
    spacer.set_hexpand(True)

    # Add to bottom_box
    bottom_box.append(back_button)
    bottom_box.append(spacer)

    # Add to main vbox
    vbox.append(bottom_box)


def on_back_clicked(button):
    """
    Handle the 'Back to Main Menu' button click.
    """
    print("Back to Main Menu clicked")


def on_quit_clicked(button):
    """
    Handle the 'Quit Game' button click.
    """
    app = button.get_root().get_application()
    app.quit()


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
        board_state[int(board_state_index)] = "X"
        current_player = "O"
    else:
        button.add_css_class("o-style")
        board_state[int(board_state_index)] = "O"
        current_player = "X"

    update_turn_label()
    # Checking Board State
    print(board_state)


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
