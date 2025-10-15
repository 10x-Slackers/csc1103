# Program Outputs a Window with a Button "Click Me" and when
# clicked on, outputs "Hello, World!" in the console
import gi
from gi.repository import Gtk

gi.require_version("Gtk", "3.0")


class MyWindow(Gtk.Window):
    def __init__(self):
        super().__init__(title="Hello GTK")
        self.set_border_width(10)

        button = Gtk.Button(label="Click Me")
        button.connect("clicked", self.on_button_clicked)
        self.add(button)

    def on_button_clicked(self, widget):
        print("Hello, World!")


win = MyWindow()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()

# Below is codes for a blank window

# import gi

# gi.require_version("Gtk", "3.0")
# from gi.repository import Gtk

# win = Gtk.Window()
# win.connect("destroy", Gtk.main_quit)
# win.show_all()
# Gtk.main()
