import gi
from gi.repository import Gdk, Gtk

gi.require_version("Gtk", "4.0")

"""
Main menu for the Tic Tac Toe.

Provides a simple main menu with 1 Player, 2 Player and Quit buttons.
"""

# --- Callback functions ---
"""Won't do Doxygen comments here for now until implemented"""


def on_one_player_clicked(button):
    print("1 Player mode selected")


def on_two_player_clicked(button):
    print("2 Player mode selected")


def on_quit_clicked(button):
    """
    Handle the "Quit Game" button click by closing the application

    Args:
        button: The Button that was clicked.
    """
    app.quit()


app = Gtk.Application(application_id="com.csc1103.TicTacToeMenu")


def main_menu(application):
    """
    Build and present the main application window.

    Args:
        application: The Gtk.Application instance passed by the activate signal.
    """
    window = Gtk.ApplicationWindow(application=application)
    window.set_title("Tic Tac Toe")
    window.set_default_size(300, 220)
    window.set_resizable(False)

    # CSS Styling
    css = b"""
    label#welcome {
        font-family: "Comic Sans MS", cursive, sans-serif;
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

    # Main vertical container
    vbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=15)
    vbox.set_margin_top(30)
    vbox.set_margin_bottom(30)
    vbox.set_margin_start(30)
    vbox.set_margin_end(30)

    # Welcome label
    welcome_label = Gtk.Label(label="Welcome to Tic Tac Toe!")
    welcome_label.set_halign(Gtk.Align.CENTER)
    welcome_label.set_valign(Gtk.Align.START)
    welcome_label.set_name("welcome")  # Set widget ID for CSS
    vbox.append(welcome_label)

    # Button box (1P, 2P)
    button_box = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=15)
    button_box.set_halign(Gtk.Align.FILL)

    btn_1p = Gtk.Button(label="1 Player")
    btn_1p.connect("clicked", on_one_player_clicked)
    button_box.append(btn_1p)

    btn_2p = Gtk.Button(label="2 Player")
    btn_2p.connect("clicked", on_two_player_clicked)
    button_box.append(btn_2p)

    vbox.append(button_box)

    # Spacer to push quit button to bottom
    spacer = Gtk.Box()
    spacer.set_hexpand(True)
    spacer.set_vexpand(True)
    vbox.append(spacer)

    # Quit button aligned to bottom right
    quit_box = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL)
    quit_box.set_halign(Gtk.Align.END)

    quit_button = Gtk.Button(label="Quit Game")
    quit_button.connect("clicked", on_quit_clicked)
    quit_box.append(quit_button)

    vbox.append(quit_box)

    window.set_child(vbox)
    window.present()


if __name__ == "__main__":
    app.connect("activate", main_menu)
    app.run()
