project(
    'csc1103',
    'c',
    version: '1.0',
    default_options: ['buildtype=release', 'c_std=c99'],
)

# Naive Bayes model weights
fs = import('fs')
nb_weights_exist = fs.exists(
    meson.current_source_dir() / 'artifacts' / 'naive_bayes.bin',
)
if nb_weights_exist
    nb_header = custom_target(
        'nb_model.h',
        output: 'nb_model.h',
        input: 'artifacts/naive_bayes.bin',
        command: ['xxd', '-i', '-n', 'nb_model_bin', '@INPUT@', '@OUTPUT@'],
        install: false,
    )
    add_project_arguments('-DEMBED_NB_MODEL', language: 'c')
else
    warning(
        'Naive Bayes model weights not found. The GUI and ML CLI will be built without the embedded model.',
    )
endif

subdir('src')
