#!/usr/bin/env python
# -*- coding: utf-8 -*-

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



from cyclops import Cyclops

c = Cyclops('localhost:8090')

c.load('ds', '/tmp/test.db')

try:
    # fail because of not fixed-length descriptors
    c.createDistance('euc', 'ds', 'euclidean')
except: pass

c.createDistance('euc2', 'ds', 'euclidean', { 'descriptorNames': '*.mean' })

print 'datasets loaded:', c.datasetNames()
print 'distances created:', c.distanceNames()

print list(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                          'ds', { 'distance': 'euclidean', 'params': { 'descriptorNames': '*.mean' }}).get(10))

print list(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                          'ds', { 'distance': 'euclidean',
                                  'params': { 'descriptorNames': '*.mean' }}).get(10, keys = [ 'tempotap_bpm' ]))


try: c.getPoints([ 'ds', 'gloup' ])
except: pass

try: c.nnSearchById([ 'ds', 'gloup' ])
except: pass

try:
    print list(c.nnSearchById("gloup.mp3",
                              'ds', { 'distance': 'euclidean',
                                      'params': { 'descriptorNames': '*.mean' }}).get(10, keys = [ 'tempotap_bpm' ]))
except:
    pass

print list(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                          'ds', 'euc2').get(10))

print list(c.nnSearchByExample(c.getPoint('ds', "(Sittin' On) The Dock Of The Bay.mp3"),
                               'ds', 'euc2').get(10))

rset = c.nnSearchByIdWithFilter("(Sittin' On) The Dock Of The Bay.mp3",
                                'ds', 'euc2', 'where value.tempotap_bpm > 130')

result =  c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                         'ds', 'euc2', inputSpace = rset).get(10)

for pid, dist, values in result:
    print pid, '- bpm =', c.getPoint('ds', pid)['tempotap_bpm']


#######################################################################
print '*'*100


from gaia2 import *

ds = DataSet()
ds.load('/tmp/test.db')

v = View(ds)
dist = MetricFactory.create('euclidean', ds.layout(), { 'descriptorNames': '*.mean' })

print v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3", dist).get(10)

rset = v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3",
                  'where value.tempotap_bpm > 130',
                  dist)

result = v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3", rset, dist).get(10)

for pid, dist in result:
    print pid, '- bpm =', c.getPoint('ds', pid)['tempotap_bpm']
