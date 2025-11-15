cc = meson.get_compiler('c')
mathlib = cc.find_library('m', required: true)
gtk_dep = dependency('gtk4', required: true)

gui_src = files(
    'app/board_gui.c',
    'app/game.c',
    'app/gui.c',
    'app/main_menu.c',
    'app/option_menu.c',
    'shared/board.c',
    'shared/minimax.c',
    'shared/naive_bayes.c',
)

executable('gui', gui_src, dependencies: [gtk_dep, mathlib])

ml_src = files(
    'ml/cli.c',
    'ml/dataset.c',
    'ml/main.c',
    'ml/statistics.c',
    'ml/training.c',
    'shared/board.c',
    'shared/naive_bayes.c',
)

executable('ml', ml_src, dependencies: [mathlib])
