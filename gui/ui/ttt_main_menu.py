import gi

gi.require_version("Gtk", "4.0")
from gi.repository import Gtk, Gdk  # noqa

from ui import tictactoe_grid  # noqa

"""
Main menu for the Tic Tac Toe.

Provides a simple main menu with 1 Player, 2 Player and Quit buttons.
"""

# --- Callback functions ---
"""Won't do Doxygen comments here for now until implemented"""

WINDOW_LENGTH = 300
WINDOW_WIDTH = 280
SPACING = 15
MARGIN = 30

app = Gtk.Application(application_id="com.csc1103.TicTacToeMenu")


def main_menu(app):
    """
    Build and present the main application window.

    Args:
        application: The Gtk.Application instance passed by the activate signal.
    """
    window = Gtk.ApplicationWindow(application=app)
    window.set_title("Tic Tac Toe")
    window.set_default_size(WINDOW_LENGTH, WINDOW_WIDTH)
    window.set_resizable(False)

    apply_css()

    vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=SPACING)

    main_container(vbox)
    welcome_label(vbox)
    player_buttons(vbox)
    spacer(vbox)
    quit_button(vbox)

    window.set_child(vbox)
    window.present()


def main_container(vbox):
    """
    Configures margin spacing for the main vertical box container.

    Parameters:
        vbox (Gtk.Box): The main vertical container to apply margins to.
    """
    vbox.set_margin_top(MARGIN)
    vbox.set_margin_bottom(MARGIN)
    vbox.set_margin_start(MARGIN)
    vbox.set_margin_end(MARGIN)


def welcome_label(vbox):
    """
    Adds a stylized welcome label to the vertical box container.

    Parameters:
        vbox (Gtk.Box): The container to which the label will be appended.
    """
    welcome_label = Gtk.Label(label="Welcome to Tic Tac Toe!")
    welcome_label.set_halign(Gtk.Align.CENTER)
    welcome_label.set_valign(Gtk.Align.START)
    welcome_label.set_name("welcome")
    vbox.append(welcome_label)


def player_buttons(vbox):
    """
    Adds buttons for selecting single or two-player mode to the UI.

    Parameters:
        vbox (Gtk.Box): The container to which the buttons will be appended.
    """
    button_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=SPACING)
    button_box.set_halign(Gtk.Align.FILL)

    btn_1p = Gtk.Button(label="1 Player")
    btn_1p.connect("clicked", on_one_player_clicked)
    button_box.append(btn_1p)

    btn_2p = Gtk.Button(label="2 Player")
    btn_2p.connect("clicked", on_two_player_clicked)
    button_box.append(btn_2p)

    vbox.append(button_box)


def spacer(vbox):
    """
    Adds an expandable spacer to the container to push content apart.

    Parameters:
        vbox (Gtk.Box): The container to which the spacer will be added.
    """
    spacer = Gtk.Box()
    spacer.set_hexpand(True)
    spacer.set_vexpand(True)
    vbox.append(spacer)


def quit_button(vbox):
    """
    Adds a quit button aligned to the bottom-right of the container.

    Parameters:
        vbox (Gtk.Box): The container to which the quit button will be added.
    """
    quit_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    quit_box.set_halign(Gtk.Align.END)

    quit_button = Gtk.Button(label="Quit Game")
    quit_button.connect("clicked", on_quit_clicked)
    quit_box.append(quit_button)

    vbox.append(quit_box)


def on_one_player_clicked(button):
    window = button.get_root()
    if window is not None:
        app = window.get_application()
        tictactoe_grid.main(app)
        window.close()


def on_two_player_clicked(button):
    window = button.get_root()
    if window is not None:
        app = window.get_application()
        tictactoe_grid.main(app)
        window.close()


def on_quit_clicked(button):
    """
    Handle the "Quit Game" button click by closing the application

    Args:
        button: The Button that was clicked.
    """
    window = button.get_root()
    if window is not None:
        app = window.get_application()
        app.quit()


def apply_css():
    """
    Applies custom CSS styling to the UI
    """
    css = b"""
    label#welcome {
        font-family: sans-serif, cursive, "Comic Sans MS";
        font-size: 28px;
        font-weight: bold;
        color: #ff6600;
        text-shadow: 1px 1px 2px #ffaa33;
    }
    """
    style_provider = Gtk.CssProvider()
    style_provider.load_from_data(css)
    Gtk.StyleContext.add_provider_for_display(
        Gdk.Display.get_default(),
        style_provider,
        Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION,
    )


if __name__ == "__main__":
    app.connect("activate", main_menu)
    app.run()
