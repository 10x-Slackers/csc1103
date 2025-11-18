# Math library
cc = meson.get_compiler('c')
mathlib = cc.find_library('m', required: true)

# GTK 4 dependency
gtk_dep = dependency('gtk4', required: true)

# GStreamer dependency
gst_dep = dependency('gstreamer-1.0', required: true)

# GResource assets bundle
gnome = import('gnome')
gresource = gnome.compile_resources(
    'resources',
    '../resources/resources.gresource.xml',
    source_dir: '../resources',
    c_name: 'resources',
)

# Naive Bayes model weights
fs = import('fs')
nb_weights_exist = fs.exists(
    meson.current_source_dir() / 'ml' / 'naive_bayes.bin',
)
if nb_weights_exist
    nb_header = custom_target(
        'nb_model.h',
        output: 'nb_model.h',
        input: 'ml/naive_bayes.bin',
        command: ['xxd', '-i', '-n', 'nb_model_bin', '@INPUT@', '@OUTPUT@'],
        install: false,
    )
    add_project_arguments('-DEMBED_NB_MODEL', language: 'c')
else
    warning(
        'Naive Bayes model weights not found. The GUI and ML CLI will be built without the embedded model.',
    )
endif

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

if nb_weights_exist
    app_src += [nb_header]
endif

executable(
    'tictactoe',
    [app_src, gresource],
    dependencies: [gtk_dep, mathlib, gst_dep],
)

ml_src = files(
    'ml/benchmark.c',
    'ml/cli.c',
    'ml/dataset.c',
    'ml/main.c',
    'ml/statistics.c',
    'ml/training.c',
    'shared/board.c',
    'shared/naive_bayes.c',
    'shared/minimax.c',
)

executable('ml-cli', ml_src, dependencies: [mathlib])
