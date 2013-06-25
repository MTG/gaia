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



from gaia2 import View, MetricFactory

def train_1NN(dataset, groundTruth, distance, params, dropBestResult = False):
    resultIndex = 1 if dropBestResult else 0

    def search(dataset, view, p, dist):
        similarPoint = dataset.point(view.nnSearch(p, dist).get(2)[resultIndex][0])
        return groundTruth[similarPoint.name()]

    dist = MetricFactory.create(distance, dataset.layout(), params)
    view = View(dataset)
    classifier = lambda p: str(search(dataset, view, p, dist))

    return classifier


def train_1NN_mixed(dataset, groundTruth, alpha, dropBestResult):
    classifier = train_1NN(dataset,
                           groundTruth,
                           distance = 'linearcombination',
                           params = { 'mfcc_kl': { 'weight': alpha,
                                                   'distance': 'kullbackleibler',
                                                   'params': { 'descriptorName': 'mfcc' } },

                                      'spectral_euclidean': { 'weight': 1-alpha,
                                                              'distance': 'euclidean',
                                                              'params': { 'descriptorNames': 'rca*' } }
                                      },
                           dropBestResult = dropBestResult)
    return classifier

def train_1NN_segments(dataset, groundTruth, alpha, func1, func2, dropBestResult):
    classifier = train_1NN(dataset,
                           groundTruth,
                           distance = 'crosssegment',
                           params = { 'func1': func1,
                                      'func2': func2,

                                      'distance': 'linearcombination',
                                      'mfcc_kl': { 'weight': alpha,
                                                   'name': 'kullbackleibler',
                                                   'params': { 'descriptorName': 'mfcc' } },

                                      'spectral_euclidean': { 'weight': 1-alpha,
                                                              'name': 'euclidean',
                                                              'params': { 'descriptorNames': 'rca*' } }
                                      },
                           dropBestResult = dropBestResult)
    return classifier
