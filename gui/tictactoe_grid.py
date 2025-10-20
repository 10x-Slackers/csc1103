import gi
from gi.repository import Gdk, Gtk

gi.require_version("Gtk", "4.0")

"""@file
Displays a 3x3 grid of buttons for playing Tic-Tac-Toe
"""


class TicTacToeGrid(Gtk.Application):
    """@brief Display a Tic-Tac-Toe grid.

    @details
    Manages application lifecycle, the GTK window, a 3x3 grid of buttons,
    simple player-turn switching and CSS styling for X and O.
    """

    def __init__(self):
        """@brief Initialize the application.

        @details
        Calls Gtk.Application.__init__ with an application_id, connects
        the "activate" signal to on_activate and sets the starting player.
        """
        super().__init__(application_id="com.csc1103.TicTacToe")
        self.connect("activate", self.ttt_grid)
        self.current_player = "X"

    def ttt_grid(self, app):
        """@brief Activate handler: build and show the main window.

        @param app The Gtk.Application instance passed by the activate signal.
        @type app: Gtk.Application

        @details
        Creates the application window, configures layout and CSS, creates a
        3x3 grid of Gtk.Button widgets and attaches click handlers.
        """
        self.window = Gtk.ApplicationWindow(application=app)
        self.window.set_title("Tic Tac Toe Grid")
        self.window.set_default_size(400, 400)
        self.window.set_resizable(False)

        # CSS styling setup
        self.apply_css()

        self.grid = Gtk.Grid()
        self.grid.set_row_spacing(5)
        self.grid.set_column_spacing(5)
        self.grid.set_margin_top(10)
        self.grid.set_margin_bottom(10)
        self.grid.set_margin_start(10)
        self.grid.set_margin_end(10)

        self.buttons = []

        for row in range(3):
            row_buttons = []
            for col in range(3):
                button = Gtk.Button()
                button.set_size_request(125, 125)
                button.connect("clicked", self.on_button_clicked)
                self.grid.attach(button, col, row, 1, 1)
                row_buttons.append(button)
            self.buttons.append(row_buttons)

        self.window.set_child(self.grid)
        self.window.present()

    def on_button_clicked(self, button):
        """@brief Handle a grid button click.

        @param button The Button that was clicked.

        @details
        Sets the clicked button's label to the current player's mark ("X" or "O"),
        disables the button so it cannot be clicked again, applies a CSS class
        for styling based on the mark, and toggles the current player.
        """
        button.set_label(self.current_player)
        button.set_sensitive(False)

        if self.current_player == "X":
            button.get_style_context().add_class("x-style")
        else:
            button.get_style_context().add_class("o-style")

        # Switch player
        self.current_player = "O" if self.current_player == "X" else "X"

    def apply_css(self):
        """@brief Apply CSS styles for X and O marks.

        @details
        Modifies the 'X' and 'O' for styling
        """
        # Create CSS provider
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

        # Apply CSS to default screen
        display = Gdk.Display.get_default()
        Gtk.StyleContext.add_provider_for_display(
            display, css_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION
        )


if __name__ == "__main__":
    """@brief Module entry point: create and run the program."""
    app = TicTacToeGrid()
    app.run()
