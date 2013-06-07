#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import View, MetricFactory

def train_1NN(dataset, groundTruth, distance, params, dropBestResult = False):
    resultIndex = 1 if dropBestResult else 0

    def search(dataset, view, p):
        similarPoint = dataset.point(view.nnSearch(p).get(2)[resultIndex][0])
        return groundTruth[similarPoint.name()]

    view = View(dataset, MetricFactory.create(distance, dataset.layout(), params))
    classifier = lambda p: str(search(dataset, view, p))

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
