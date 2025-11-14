cc = meson.get_compiler('c')
mathlib = cc.find_library('m', required: true)

gtk_dep = dependency('gtk4', required: true)

src = [
    'app/main.c',
    'shared/board.c',
    'shared/minimax.c',
    'shared/naive_bayes.c',
]

executable('app', src, dependencies: [mathlib])
