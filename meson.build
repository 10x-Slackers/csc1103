project(
    'csc1103',
    'c',
    version: '1.0',
    default_options: ['buildtype=release', 'c_std=c99'],
)

gtk_dep = dependency('gtk4', required: true)

src = files(
    'gui/example.c',
)

executable('example_gui', src, dependencies: [gtk_dep])
