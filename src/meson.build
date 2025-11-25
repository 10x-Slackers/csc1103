# Math library
cc = meson.get_compiler('c')
mathlib = cc.find_library('m', required: true)

# GTK 4 dependency
gtk_dep = dependency('gtk4', required: true)

# GStreamer dependency
gst_dep = dependency('gstreamer-1.0', required: true)

app_src = files(
    'app/audio.c',
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

ml_src = files(
    'ml/benchmark.c',
    'ml/cli.c',
    'ml/dataset.c',
    'ml/main.c',
    'ml/statistics.c',
    'ml/training.c',
    'shared/board.c',
    'shared/minimax.c',
    'shared/naive_bayes.c',
)

if nb_weights_exist
    app_src += [nb_header]
    ml_src += [nb_header]
endif

executable(
    'tictactoe',
    [app_src, gresource],
    dependencies: [mathlib, gst_dep, gtk_dep],
)
executable('ml-cli', ml_src, dependencies: [mathlib])
