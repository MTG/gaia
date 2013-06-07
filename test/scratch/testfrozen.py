#!/usr/bin/env python

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
