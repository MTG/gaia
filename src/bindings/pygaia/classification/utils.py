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



from gaia2 import transform, RealType, pmap, mergeDataSets


def addRCA(ds, groundTruth, dim, selectConfig = {}):
    #ds_rca = transform(ds, 'fixlength') # should be unnecessary
    ds_rca = ds
    if selectConfig:
        ds_rca = transform(ds_rca, 'select', selectConfig)

    ds_rca = transform(ds_rca, 'gaussianize')

    # if dimension is too high, we need to preprocess before with a PCA, otherwise RCA doesn't work
    l = ds_rca.layout()
    descdim = l.descriptorLocation(l.descriptorNames()).dimension(RealType)
    if descdim > 80:
        ds_rca = transform(ds_rca, 'pca', { 'resultName': 'pca%d' % 80,
                                            'dimension': 80 })

    ds_rca = transform(ds_rca, 'rca', { 'resultName': 'rca%d' % dim,
                                        'dimension': dim,
                                        'classMap': pmap(groundTruth) })

    return mergeDataSets(ds, ds_rca)
