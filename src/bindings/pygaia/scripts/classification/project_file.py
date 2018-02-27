# This file should contain all the information relevant to conducting a set of evaluations
#
# This information includes:
#  - location of the files on hard disk (audio files, datasets, results, ...)
#  - list of preprocessing steps that need to be applied to the original data
#  - list of classifiers that need to be trained and evaluated
#  - list of evaluations that need to be performed

# name of the feature one wants to classify (genre, mood, artist...)
className: genre_tzanetakis


# Path to the directory where the preprocessed datasets are going to be stored
# This can be absolute or relative to where you launch the main classification script
datasetsDirectory: "~/gaia/scripts/classification"

# Path to the directory where the results are going to be stored
# This can be absolute or relative to where you launch the main classification script
resultsDirectory: "~/gaia/scripts/classification"


# Path to the yaml file containing the list of files to be merged in the original dataset
filelist: "~/gaia/scripts/classification/filelist_example.yaml"

# Path to the groundtruth file, containing a serialized gaia2.classification.GroundTruth object
groundtruth: "~/gaia/scripts/classification/groundtruth_example.yaml"


# List of preprocessed datasets to build
preprocessing:
    # it is possible to not apply any processing, although this is of
    # of little value in real-life tests and evaluations
    raw: []

    basic:
        - transfo: remove
          params: { descriptorNames: &unusedDescs [ 'metadata.*', '*dmean*', '*dvar*',
                                                    '*.min', '*.max', '*cov',
                                                    'tonal.thpcp', # because of division by zero
                                                    'lowlevel.spectral_energyband_high.*', # 0 for low samplerate
                                                    'lowlevel.silence_rate*' # funky behavior in general
                                                    ] }
        - transfo: enumerate
          params: { descriptorNames: &stringDescs [ # 'rhythm.perceptual_tempo', # removed from new extractor
                                                    'tonal.chords_key', 'tonal.chords_scale',
                                                    'tonal.key_key', 'tonal.key_scale' ] }


    lowlevel:
        # note that the order of the transformations is important!
        - transfo: remove
          params: { descriptorNames: *unusedDescs }
        - transfo: enumerate
          params: { descriptorNames: *stringDescs }
        - transfo: select
          params: { descriptorNames: 'lowlevel*' }

    # sfx descriptors are removed from new extractor as it is focused on music, not sfx
    # nosfx:
    #     - transfo: remove
    #       params: { descriptorNames: *unusedDescs }
    #     - transfo: enumerate
    #       params: { descriptorNames: *stringDescs }
    #     - transfo: remove
    #       params: { descriptorNames: [ 'sfx*' ] }

    nobands:
        - transfo: remove
          params: { descriptorNames: *unusedDescs }
        - transfo: enumerate
          params: { descriptorNames: *stringDescs }
        - transfo: remove
          params: { descriptorNames: [ 'barkbands*', '*energyband*', 'melbands*', 'erbbands*' ] }

    normalized:
        - transfo: remove
          params: { descriptorNames: *unusedDescs }
        - transfo: enumerate
          params: { descriptorNames: *stringDescs }
        - transfo: normalize

    gaussianized:
        - transfo: remove
          params: { descriptorNames: *unusedDescs }
        - transfo: enumerate
          params: { descriptorNames: *stringDescs }
        - transfo: normalize
        - transfo: gaussianize
          params: { descriptorNames: 'lowlevel.*' }


# List of classifiers to be trained
classifiers:
    svm:
        # first svm test combinations
        - preprocessing: [ 'basic', 'lowlevel', 'nobands', 'normalized', 'gaussianized' ] # 'nosfx'
          type: [ 'C-SVC' ]
          kernel: [ 'poly', 'RBF' ]
          C:     [ -5, -3, -1, 1, 3, 5, 7, 9, 11 ] # will actually be 2**x
          gamma: [ 3, 1, -1, -3, -5, -7, -9, -11 ] # will actually be 2**x
        #  descriptorNames: [ ['*.mean', '*.var'] ]
        # more svm params combinations
        # ...

    # deactivated those because we can only import SVM models in Essentia at the moment
    #mixed:
    #    - preprocessing: [ normalized, gaussianized ]
    #      rcadim: [ 2, 4, 6, 8, 9, 10, 15, 20 ]
    #      alpha: [ 0.00, 0.02, 0.04, 0.06, 0.08, 0.9, 0.1, 0.11, 0.12, 0.14, 0.16, 0.18, 0.2, 0.3, 0.4, 0.5, 0.75, 1.0 ]

# List of evaluations to be performed
evaluations:
    nfoldcrossvalidation:
        - nfold: [ 5 ]
