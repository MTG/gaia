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
# You should have received a copy of the Affero GNU General Public License     
# version 3 along with this program. If not, see http://www.gnu.org/licenses/


from __future__ import print_function
from gaia2 import *
from numpy import array, dot, linalg
import math

# we can optimize this by computing the icov outside of this function
def mahal_slow(plist, subds):
    cov = covarianceMatrix(subds)
    icov = linalg.inv(cov)

    dists = []
    for p in plist:
        v = array(p['pca'].toList())
        dists += [ math.sqrt(dot(dot(icov, v), v)) ]

    return dists


def mahal(p, icov):
    v = array(p['pca'].toList())
    return math.sqrt(dot(dot(icov, v), v))


def model_SIMCA(ds, groundTruth, descriptorNames, exclude = [], coveredVariance = 95., verbose = False):
    allClasses = set(groundTruth.values())

    model = {}

    for c in allClasses:
        if verbose: print ('Training SIMCA for class', c)

        # get sub-space containing all the points of a given class
        points = [ p for p in ds.points() if p[groundTruth.className] == c ]
        subds = DataSet()
        subds.addPoints(points)

        # center subspace around its mean and get its PCA
        centerds = transform(subds, 'center', { 'descriptorNames': descriptorNames,
                                                'except': exclude })

        subds = transform(centerds, 'pca', { 'resultName': 'pca',
                                             'coveredVariance': coveredVariance,
                                             'descriptorNames': descriptorNames,
                                             'except': exclude })

        retainedDimensions = subds.samplePoint()['pca'].size()

        # project the original points on the PCA subspace
        projds = transform(subds, 'InverseProjection')

        # merge the points from the original dataset into a single descriptor to be able to
        # compute the distance with the projected ones
        centerds = transform(centerds, 'MergeRegion', { 'resultName': 'pca',
                                                        'descriptorNames': descriptorNames,
                                                        'except': exclude })

        assert(projds.layout(), centerds.layout())

        euclideanDist = MetricFactory.create('euclidean', projds.layout())

        # compute OD (orthogonal distance) and its cutoff limit
        OD = [ euclideanDist(p, projds.point(p.name())) for p in centerds.points() ]

        OD = array(OD)**(2./3)
        OD_cutoff = norminv(0.975, OD.mean(), OD.std()) ** 1.5


        # compute the cutoff limit for the BD (boundary distance)
        # we don't need to compute the boundary distance for the training test... Hmmm??
        #BD = mahal([ p for p in subds.points() ], subds)
        BD_cutoff = math.sqrt(chi2inv(0.975, retainedDimensions))


        # save parameters for the model of this class
        model[c] = { 'centerHist': centerds.history(),
                     'projHist': projds.history(),
                     'subHist': subds.history(),
                     'icov': linalg.inv(covarianceMatrix(subds)),
                     'OD': lambda p1, p2: euclideanDist(p1, p2) / OD_cutoff,
                     'OD_cutoff': OD_cutoff,
                     # we can't do this here because otherwise strange things happen with the closure...
                     #'BD': lambda pt: mahal(pt, linalg.inv(covarianceMatrix(subds))) / BD_cutoff,
                     'BD_cutoff': BD_cutoff
                     }

    return model


def train_SIMCA(ds, groundTruth, descriptorNames, exclude = [], alpha = 0.5, coveredVariance = 95., useBoundaryDistance = True, verbose = False):
    if verbose: print ('------------------ TRAINING SIMCA')
    ds = ds.copy()
    ds.forgetHistory()
    ds = transform(ds, 'normalize', { 'descriptorNames': descriptorNames, 'except': exclude })
    preprocess = ds.history()


    model = model_SIMCA(ds, groundTruth, descriptorNames, exclude, coveredVariance = coveredVariance, verbose = verbose)

    if verbose: print ('------------------ TESTING SIMCA')

    def classifier(p):
        p = preprocess.mapPoint(p)
        #for p in ds.points():
        bestd = 1e100 # should be enough, right? ;-)
        for c, cm in model.items():
            pp1 = cm['centerHist'].mapPoint(p)
            pp2 = cm['projHist'].mapPoint(p)
            #print 'distance to', c, '=', cm['OD'](pp1, pp2), '//', mahal(pp3, model[c]['icov']) / cm['BD_cutoff'] #cm['BD'](pp3)
            dist = cm['OD'](pp1, pp2)
            if useBoundaryDistance:
                pp3 = cm['subHist'].mapPoint(p)
                bdist = mahal(pp3, cm['icov']) / cm['BD_cutoff']
                # looks like it works slightly better than α·OD² + (1-α)·BD²
                dist = alpha*dist + (1-alpha)*bdist

            if dist < bestd:
                bestd = dist
                bestc = c

        return bestc

    return classifier
