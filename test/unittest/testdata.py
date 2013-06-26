#!/usr/bin/env python

# Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
#
# This file is part of Gaia
#
# Gaia is free software: you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation (FSF), either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the Affero GNU General Public License     
# version 3 along with this program. If not, see http://www.gnu.org/licenses/



from gaia2 import *
from os.path import join

TEST_DATABASE_FILES = 'data/dataset_small.dbfiles.yaml'
TEST_DATABASE = 'data/dataset_small.db'

TEST_DATABASE_VARLENGTH_REAL = [ 'chords_changes.value', 'onsets.value',
                                 'tempotap_intervals*', 'tempotap_rubato*', 'tempotap_ticks*' ]
TEST_DATABASE_VARLENGTH_STRING = [ 'chords_progression*' ]


TEST_SMALLDB = 'data/verysmall.db'
TEST_SMALLDB_VARLENGTH = [ 'tempotap_intervals*', 'tempotap_rubato*', 'tempotap_ticks*', 'key_key*' ]


# variable used to determine whether we should use fixed-length descriptors
# whenever possible. This shouldn't change nothing to the tests being performed
useFixedLength = False

# variable used to determine whether we should use fixed-length descriptors
# whenever possible. This shouldn't change nothing to the tests being performed
useEnumerate = False


def resetSettings():
    global useFixedLength, useEnumerate
    useFixedLength = False
    useEnumerate = False

def fixLength(dataset, exclude = []):
    return transform(dataset, 'fixlength')

def enumerateStrings(dataset, exclude = []):
    return transform(dataset, 'enumerate', { 'descriptorNames': dataset.layout().descriptorNames(StringType),
                                             'except': exclude })


# create simple artificial layout
def createSimpleLayout():
    l = PointLayout()
    l.add('a.1', RealType)
    l.add('a.2', RealType)
    l.add('c', RealType)
    l.add('b', RealType)
    l.add('d', StringType)
    return l


# create simple artificial dataset
def createSimpleDataSet():
    l = createSimpleLayout()
    ds = DataSet()
    p = Point()
    p.setName('p')
    p.setLayout(l)
    ds.addPoint(p)

    if useFixedLength:
        ds = fixLength(ds)

    if useEnumerate:
        ds = enumerateStrings(ds)

    return ds



# load real-world dataset
def loadTestDB():
    ds = DataSet()
    ds.load(TEST_DATABASE)

    if useFixedLength:
        ds = fixLength(ds)

    if useEnumerate:
        ds = enumerateStrings(ds, exclude = 'chords_progression_hash.value')

    return ds


def loadSmallDB():
    ds = DataSet()
    ds.load(TEST_SMALLDB)

    if useFixedLength:
        ds = fixLength(ds)

    if useEnumerate:
        ds = enumerateStrings(ds)

    return ds


SVM_TRAINING_SET = 'data/train.1'
SVM_TESTING_SET = 'data/test.1'
SVM_RESULT = 'data/test.1.predict'

def readLibSVMDataSet(filename):
    data = [ l.split() for l in open(filename).readlines() ]
    minidx = maxidx = 1
    for l in data:
        for i in range(1, len(l)):
            dim, value = l[i].split(':')
            l[i] = (int(dim), float(value))
            minidx = min(minidx, int(dim))
            maxidx = max(maxidx, int(dim))

    dimension = maxidx - minidx + 1

    layout = PointLayout()
    layout.add('class', StringType)
    layout.add('value', RealType)

    ds = DataSet()
    n = 0
    points = []

    for l in data:
        p = Point()
        p.setLayout(layout)
        p.setName('instance_%06d' % n)
        n += 1

        p['class'] = l[0]
        desc = RealDescriptor(dimension, 0.0)
        for dim, value in l[1:]:
            desc[dim-minidx] = value
        p['value'] = desc

        points.append(p)

    ds.addPoints(points)

    return ds


SMALL_DB_RAW_RESULTS = (
    ('1_ethno.wav', 0.0),
    ('5_ethno.wav', 2570553393152.0),
    ('3_ethno.wav', 7932531965952.0),
    ('2_ethno.wav', 9843669729280.0),
    ('4_ethno.wav', 10462388289536.0))

SMALL_DB_CLEAN_RESULTS = (
    ('1_ethno.wav', 0.0),
    ('5_ethno.wav', 2570553393152.0),
    ('3_ethno.wav', 7932531965952.0),
    ('2_ethno.wav', 9843669729280.0),
    ('4_ethno.wav', 10462388289536.0))

SMALL_DB_NORM_RESULTS = (
    ('1_ethno.wav', 0.0),
    ('3_ethno.wav', 3.6515905857086182),
    ('2_ethno.wav', 3.6936297416687012),
    ('5_ethno.wav', 4.9693379402160645),
    ('4_ethno.wav', 5.6257767677307129))

# PCA is the same as norm, probably because there are only 5 songs (< dimension)
SMALL_DB_PCA_RESULTS = (
    ('1_ethno.wav', 0.0),
    ('3_ethno.wav', 3.6515908241271973),
    ('2_ethno.wav', 3.6936302185058594),
    ('5_ethno.wav', 4.9693384170532227),
    ('4_ethno.wav', 5.6257772445678711))


# gaia 2.0 backwards compatibility search results
GAIA_20_BACKWARDS_COMPAT_DATASET = join('data', 'gaia_2_0.db')
GAIA_20_BACKWARDS_COMPAT_PCA_DATASET = join('data', 'gaia_2_0_pca.db')
GAIA_20_BACKWARDS_COMPAT_HISTORY = join('data', 'gaia_2_0_pca.hist')
GAIA_20_BACKWARDS_COMPAT_RESULTS = (
    ('01 Respect.mp3', 0.0),
    ('09 El Bombon De Elena - Cortijo, Rafael & Ismael Rivera.mp3', 0.33242055773735046),
    ('06 Life On Mars - Bowie, David.mp3', 0.36091315746307373),
    ('20 Dust My Blues - James, Elmore & His Broom Dusters.mp3', 0.37942451238632202),
    ('17 Dust My Broom.mp3', 0.39593410491943359))


# RCA genre classification
RCA_GENRE_GT = 'data/rca_genre_groundtruth.yaml'
RCA_GENRE_RESULTS = (
    ('01 Cigarettes And Alcohol - Oasis.mp3', 0.0),
    ('04 Ace Of Spades - Motorhead.mp3', 2.75613355637),
    ('05 So Here We Are - Bloc Party.mp3', 2.91703510284),
    ('10 Brassneck - Wedding Present.mp3', 3.0742483139),
    ('09 Tied Up Too Tight - Hard-Fi.mp3', 3.16019511223),
    ('05 Sidewalking - Jesus & Mary Chain.mp3', 3.33982801437),
    ('16 Year 3000 - Busted.mp3', 3.47880768776),
    ('14 Don\'t Look Back In Anger - Oasis.mp3', 3.53692793846),
    ('07 Honest Mistake - Bravery.mp3', 3.59466409683),
    ('20 Chocolate - Snow Patrol.mp3', 3.65769457817))
