import gi
from gi.repository import Gdk, Gtk

gi.require_version("Gtk", "4.0")

# Track current player
current_player = "O"

# 2D list to store references to each button in the 3x3 grid
buttons = []


def main():
    """
    Entry point of the Tic Tac Toe application.

    Sets up the GTK Application, grid layout, and connects signal handlers.
    """

    app = Gtk.Application(application_id="com.csc1103.TicTacToe")

    def on_activate(app):
        apply_css()
        window = Gtk.ApplicationWindow(application=app)
        window.set_title("Tic Tac Toe Grid")
        window.set_default_size(400, 400)
        window.set_resizable(False)

        grid = Gtk.Grid()
        grid.set_row_spacing(5)
        grid.set_column_spacing(5)
        grid.set_margin_top(10)
        grid.set_margin_bottom(10)
        grid.set_margin_start(10)
        grid.set_margin_end(10)

        # Create 3x3 grid of buttons
        for row in range(3):
            row_buttons = []
            for col in range(3):
                button = Gtk.Button()
                button.set_size_request(125, 125)
                button.connect("clicked", on_button_clicked)
                grid.attach(button, col, row, 1, 1)
                row_buttons.append(button)
            buttons.append(row_buttons)

        window.set_child(grid)
        window.present()

    app.connect("activate", on_activate)
    app.run()


def apply_css():
    """
    Apply CSS styling for the X and O symbols.

    - X will be red.
    - O will be blue.
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
    """
    css_provider = Gtk.CssProvider()
    css_provider.load_from_data(css)
    display = Gdk.Display.get_default()
    Gtk.StyleContext.add_provider_for_display(
        display, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
    )


def on_button_clicked(button):
    """
    Callback for when a grid button is clicked.

    Sets the button label to the current player's symbol,
    applies styling, disables the button, and switches players.

    Args:
        button: The button that was clicked.
    """
    global current_player

    button.set_label(current_player)
    button.set_sensitive(False)

    if current_player == "X":
        button.get_style_context().add_class("x-style")
        current_player = "O"
    else:
        button.get_style_context().add_class("o-style")
        current_player = "X"


if __name__ == "__main__":
    main()
