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
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see http://www.gnu.org/licenses/


# encoding: utf-8

'''
This example shows the distance matrix between 12 songs, 2 of each genre,
for 3 different setups:
 - Kullback-Leibler distance on MFCC
 - Euclidean distance on MFCC.mean
 - Euclidean distance on MFCC.mean and MFCC.var
'''

from gaia2 import *
from numpy import *
from pylab import *
from testutils import *


cvar.verbose = False


def distance_matrix(list_songs, dist):
    matrix = []
    for song1 in list_songs:
        row = []
        for song2 in list_songs:
            row.append(dist(song1, song2))
        matrix.append(row)
    return matrix


def plot_matrix(dist_matrix):
    i = 0
    for row in dist_matrix:
        most_similar = array(row[:i]+row[i+1:]).argmin()
        if most_similar >= i: most_similar += 1
        print i, 'is most similar to', most_similar
        i += 1
    
    imshow(dist_matrix, interpolation='nearest')




dataset = DataSet()
dataset.load('unittest/dataset_small.db')
dataset = addVarFromCov(dataset, 'mfcc')

list_names = [
    # Jazz
    '01 Cantaloupe Island.mp3',
    '02 Watermelon Man.mp3',
    # R&B
    '20 Ms Jackson - Outkast.mp3',
    '02 Mustang Sally.mp3',
    # Metal
    '02 War Pigs - Black Sabbath.mp3',
    '01 Number Of The Beast - Iron Maiden.mp3',
    # Reggae
    '03 Spanish Harlem.mp3',
    '05 Gangster Rock.mp3',
    # Pop
    '10 Wonderwall - Oasis.mp3',
    '07 Dreams - Gabrielle.mp3',
    # Folk & New Age
    '07 Festival of Flowers.mp3',
    '03 Oh, Had I a Golden Thread.mp3'
    ]

list_songs = [ dataset.point(pname) for pname in list_names ]


# set a gray colormap for everyone. Gray isn't so funny, but it shows songs
# clusters better. Another good colormap here is jet() (default matplotlib one)
gray()


# Plot for Kullback-Leibler distance
print '\ncomputing distance matrix with Kullback-Leibler distance'

kl_dist = MetricFactory.create('kullbackleibler', dataset.layout(),
                               { 'descriptorName': 'mfcc' })

dist_matrix = distance_matrix(list_songs, kl_dist)
subplot(131)
title('KL distance on mfcc')
plot_matrix(dist_matrix)


# Plot for Euclidean distance on MFCC.mean
print '\ncomputing distance matrix with euclidean distance on mfcc.mean'

euc_dist = MetricFactory.create('euclidean', dataset.layout(),
                                { 'descriptorNames': [ 'mfcc.mean' ] })

dist_matrix = distance_matrix(list_songs, euc_dist)
subplot(132)
title('Euclidean dist on mfcc.mean')
plot_matrix(dist_matrix)


# Plot for Euclidean distance on MFCC.mean and MFCC.var
print '\ncomputing distance matrix with euclidean distance on mfcc.mean and mfcc.var'

euc_dist = MetricFactory.create('euclidean', dataset.layout(),
                                { 'descriptorNames': [ 'mfcc.mean', 'mfcc.var' ] })

dist_matrix = distance_matrix(list_songs, euc_dist)
subplot(133)
title('Euclidean dist on mfcc.{mean,var}')
plot_matrix(dist_matrix)



show()
