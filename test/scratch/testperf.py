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

# <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
# 
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>

from __future__ import print_function
from builtins import str
from gaia2 import *
cvar.verbose = True
try:
    setDebugLevel(GPerf)
    searchSpaceBranch = True
except:
    searchSpaceBranch = False

print('Loading PCA dataset...')
ds = DataSet()
ds.load('/var/data/data_nico/itunes/data_merged/itunes_ess0.5.norm.pca1M.db')

print('Loading reference dataset...')
dsref = DataSet()
dsref.load('/var/data/data_nico/itunes/data_merged/itunes_ess0.5.bpm_key.db')

dsref.setReferenceDataSet(dsref)
ds.setReferenceDataSet(dsref)

dist = MetricFactory.create('euclidean', ds.layout())
v = View(ds, dist)

indexView = False
indexView = True
if searchSpaceBranch and indexView:
    v.indexOn('tempotap_bpm')
    v.indexOn('tonal_key_key')
    v.indexOn('tonal_key_mode')

p = ds.samplePoint().name()
p2 = ds.collection('itunes_ess0.5').pointNames()[123456]

pnames = [ '100006', '1000146', '1000246', '1000314' ]
filterByID = 'WHERE point.id IN (%s)' % str(pnames)[1:-1].replace("'", '"')

queries = [ (p, 10, ''),
            (p2, 10, ''),
            #(p, 1200, ''),
            #(p, 10, filterByID),
            #(p2, 10, filterByID),
            #(p, 10, filterByID + ' AND True'),
            #(p, 10, filterByID + ' AND point.id IN ("528430", "737230")'),
            (p, 10, 'WHERE (True AND True) and (true and TRUE)'),
            (p, 10, 'WHERE (false AND True) OR (true and false)'),
            (p2, 10, 'WHERE value.tempotap_bpm.value > 140'),
            (p, 10, 'WHERE true AND value.tempotap_bpm.value > 140'),
            (p, 10, 'WHERE value.tempotap_bpm > 110'),
            (p, 10, 'WHERE value.tempotap_bpm > 120 AND value.tempotap_bpm < 130'),
            (p, 10, 'WHERE value.tempotap_bpm BETWEEN 120 AND 130'),
            (p, 10, 'WHERE label.tonal_key_key = "C"'),
            (p2, 10, '''WHERE ((label.tonal_key_key = "A" AND label.tonal_key_mode = "major") OR
                               (label.tonal_key_key = "E" AND label.tonal_key_mode = "minor"))
                              AND value.tempotap_bpm < 90''' )
            ]


for pid, nn, filt in queries:
    print('*'* 100)
    print('Performing query for %d NN with filter = \'%s\' (point.id = %s)' % (nn, filt, pid))
    s = v.nnSearch(pid, nn, filt)
    for r in s:
        p = dsref.point(r[0])
        print(r, 'bpm=', p['tempotap_bpm'], ' - key =', p['tonal_key_key'], p['tonal_key_mode'])

