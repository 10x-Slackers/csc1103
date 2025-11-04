import gi

gi.require_version("Gtk", "4.0")
from gi.repository import Gtk  # noqa
from ui import main_menu  # noqa


def on_activate(app):
    main_menu.main(app)


if __name__ == "__main__":
    app = Gtk.Application(application_id="com.csc1103.TicTacToeMenu")
    app.connect("activate", on_activate)
    app.run()
