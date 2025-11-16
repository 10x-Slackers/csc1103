cc = meson.get_compiler('c')
mathlib = cc.find_library('m', required: true)

# GTK 4
gtk_dep = dependency('gtk4', required: true)

# GResource bundle
gnome = import('gnome')
gresource = gnome.compile_resources(
    'resources',
    '../resources/resources.gresource.xml',
    source_dir: '../resources',
    c_name: 'resources',
)

gui_src = files(
    'app/board_gui.c',
    'app/game.c',
    'app/main.c',
    'app/main_menu.c',
    'app/option_menu.c',
    'app/utils.c',
    'shared/board.c',
    'shared/minimax.c',
    'shared/naive_bayes.c',
)

executable('tictactoe', [gui_src, gresource], dependencies: [gtk_dep, mathlib])

ml_src = files(
    'ml/cli.c',
    'ml/dataset.c',
    'ml/main.c',
    'ml/statistics.c',
    'ml/training.c',
    'shared/board.c',
    'shared/naive_bayes.c',
)

executable('ml-cli', ml_src, dependencies: [mathlib])
