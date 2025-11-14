cc = meson.get_compiler('c')
mathlib = cc.find_library('m', required: true)

gtk_dep = dependency('gtk4', required: true)

app_src = files(
    'app/main.c',
    'shared/board.c',
    'shared/minimax.c',
    'shared/naive_bayes.c',
)

executable('app', app_src, dependencies: [mathlib])

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
