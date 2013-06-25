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



from gaia2 import *

DATASET_FILE = '/home/nwack/tmp/pv_tracks_10000chunk.db'
FROZEN_DATASET_FILE = '/home/nwack/tmp/pv_tracks_10000chunk_frozen.db'
POINTID = '05683a1cae064732801e4b1421306d3b'

ds = FrozenDataSet()
ds.load(FROZEN_DATASET_FILE)

print 'Dataset dimensions: points =', ds.size(), '- dim =', ds.dimension()
print 'Descriptor names:', ds.descriptorNames()

# frozen distances are not created through a factory, but instantiated directly
v = FrozenView(ds)
#dist = FrozenEuclideanDistance(ds, 'colfilter')
dist = FrozenDistanceFactory.create('euclidean', ds, { 'descriptorName': 'colfilter' })

print 'For point', POINTID
print v.nnSearch(POINTID, dist).get(10)

p = ds.point(POINTID)
print v.nnSearch(p, dist).get(10)

hybriddist = FrozenDistanceFactory.create('LinearCombination', ds,
                                        { 'CF': { 'distance': 'CosineAngle',
                                                  'params': { 'descriptorName': 'colfilter',
                                                              'defaultValue': 0.5 },
                                                  'weight': 0.4 },
                                          'CT': { 'distance': 'CosineAngle',
                                                  'params': { 'descriptorName': 'context',
                                                              'defaultValue': 0.5 },
                                                  'weight': 0.4 },
                                          'CN': { 'distance': 'euclidean',
                                                  'params': { 'descriptorName': 'pca' },
                                                  'weight': 0.2 }
                                          })

print v.nnSearch(p, hybriddist).get(10)

hybridexpdist = FrozenDistanceFactory.create('ExponentialCompress', ds,
                                           { 'distance': 'linearcombination',
                                             'params': { 'CF': { 'distance': 'CosineAngle',
                                                                 'params': { 'descriptorName': 'colfilter',
                                                                             'defaultValue': 0.5 },
                                                                 'weight': 0.4 },
                                                         'CT': { 'distance': 'CosineAngle',
                                                                 'params': { 'descriptorName': 'context',
                                                                             'defaultValue': 0.5 },
                                                                 'weight': 0.4 },
                                                         'CN': { 'distance': 'euclidean',
                                                                 'params': { 'descriptorName': 'pca' },
                                                                 'weight': 0.2 }
                                                         }
                                             })

print v.nnSearch(p, hybridexpdist).get(10)
