#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

ESSENTIA_VERSION = 'essentia_streaming_1.2_schizo'
GENERATE_PROJECT = 'python ~/dev/gaia/src/scripts/classification/generate_classification_project.py %s /raid/audio/%s %s.yaml'
RUN_PROJECT = 'python ~/dev/gaia/src/scripts/classification/run_tests.py %s.yaml'



# map: name -> (type, location)
ALL_DBS = { # GENRE DATABASES
            'tzanetakis':  ('genre', 'benchmarks/genre/Tzanetakis/descriptors/%s/22kmono/'),
            'dortmund':    ('genre', 'benchmarks/genre/Dortmund/descriptors/%s/'),
            'rosamerica':  ('genre', 'benchmarks/genre/rosamerica_train/descriptors/%s/mp3/'),
            # we ignore rosamerica_test for now
            'rentfrow':    ('genre', 'benchmarks/genre/rentfrow/descriptors/%s/mp3/'),
            'electronica': ('genre', 'benchmarks/genre/electronica/descriptors/%s/'),

            # CULTURE DATABASES
            'culture':     ('culture', 'culture/descriptors/%s/'),
            'culture_old': ('culture', 'benchmarks/culture_old/descriptors/%s/'),

            # RHYTHM DATABASES
            'mirex-ballroom':   ('rhythm', 'benchmarks/mirex-ballroom/descriptors/%s/wav/'),
            # note for perceptual speed, the groundtruth is not in the name of dir, but in some
            # separate metadata file
            #'perceptual_speed': ('rhythm', '')

            'danceability':
            }

for name, (classType, location) in ALL_DBS.items():
    location = location % ESSENTIA_VERSION
    gencmd = GENERATE_PROJECT % (classType, location, name)
    runcmd = RUN_PROJECT % name

    os.system('mkdir -p %s' % name)
    os.chdir(name)
    os.system(gencmd)
    os.chdir('..')
    #os.system(runcmd)




# moods 
for m in mood_acoustic mood_blue mood_electronic mood_furious mood_happy mood_party mood_relaxed; do mkdir -p $m; cd $m; python

for m in `ls`; do cd $m; python ../../../gaia/src/scripts/classification/run_tests.py $m.yaml; cd ..; done
