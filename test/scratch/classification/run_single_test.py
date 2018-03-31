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

from __future__ import division
from __future__ import print_function
from future import standard_library
standard_library.install_aliases()
from builtins import str
from past.utils import old_div
from config import *
import sys
from gaia2 import *
from gaia2.classification import evaluate, evaluateNfold
addImportPath('/home/nwack/dev/gaia/test/')
addImportPath('/home/nwack/dev/gaia/test/classification')
import testutils as utils
import os
from os.path import join, split
import threading, time

import pickle



# give it 1 minute to finish
import processwatcher
pw = processwatcher.ProcessTimeout(seconds = 15*60)



def addRCA(ds, dim):
    ds_rca = transform(ds, 'fixlength')
    ds_rca = transform(ds_rca, 'select', { 'descriptorNames': '*',
                                           'except':          [ '*mfcc*', '.'+className, '*spectral_contrast*' ] })
    ds_rca = transform(ds_rca, 'gaussianize')

    # if dimension is too high, we need to preprocess before with a PCA, otherwise RCA doesn't work
    l = ds_rca.layout()
    descdim = l.descriptorLocation(l.descriptorNames()).dimension(RealType, ds_rca.samplePoint())
    if descdim > 80:
        ds_rca = transform(ds_rca, 'pca', { 'dimension': 80, 'resultName': 'pca80' })

    ds_rca = mergeDataSets(ds_rca, transform(ds, 'select', { 'descriptorNames': '.'+className }))
    ds_rca = transform(ds_rca, 'rca', { 'dimension': dim, 'className': className, 'resultName': 'rca%d' % dim })

    return mergeDataSets(ds, ds_rca)


def writeResult(filename, confusion):
    result = old_div(float(confusion.correct()), confusion.total())
    open(filename + '.result', 'w').write(str(result))
    open(filename + '.cmatrix.html', 'w').write(confusion.toHtml())

def loadData(datasetName, featureSet):
    ds = DataSet()
    ds.load(datasetName[:-3] + '.feat%d.db' % featureSet)
    groundTruth = utils.GroundTruth(className, pickle.load(open(datasetName + '.groundtruth')))

    return ds, groundTruth

if __name__ == '__main__':
    cvar.verbose = False
    classifier = sys.argv[2]
    datasetName = sys.argv[3]
    basename = datasetName[:-3].split('/')[-1]
    featureSet = int(sys.argv[4])

    if classifier == 'simca':
        coveredVar = int(sys.argv[5])
        alpha = float(sys.argv[6])

        filename = resultsdir + basename + '_simca_%d_%d_%.1f' % (featureSet, coveredVar, alpha)

        if os.path.exists(filename + '.result'):
            print('File exists. Skipping Test.')
        else:
            ds, groundTruth = loadData(datasetName, featureSet)

            from classifier_SIMCA import train_SIMCA
            confusion = evaluateNfold(10, ds, groundTruth, train_SIMCA,
                                      descriptorNames = '*', exclude = [],
                                      alpha = alpha, coveredVariance = coveredVar,
                                      useBoundaryDistance = True)
            print(confusion.results())

            writeResult(filename, confusion)

    elif classifier == 'svm':
        svmtype = sys.argv[5]
        kernel = sys.argv[6]
        cexp = float(sys.argv[7])
        gammaexp = float(sys.argv[8])

        filename = resultsdir + basename + '_svm_%d_%s_%s_%.1f_%.1f' % (featureSet, svmtype, kernel, cexp, gammaexp)

        if os.path.exists(filename + '.result'):
            print('File exists. Skipping Test.')
        else:
            ds, groundTruth = loadData(datasetName, featureSet)

            from classifier_SVM import train_SVM
            confusion = evaluateNfold(10, ds, groundTruth, train_SVM,
                                      descriptorNames = [ '*.mean', '*.var' ], exclude = ['*.spectral_contrast.mean'],
                                      svmtype = svmtype, kernel = kernel, c = 2**cexp, gamma = 2**gammaexp)
            print(confusion.results())

            writeResult(filename, confusion)

    elif classifier == 'mixed':
        rcadim = int(sys.argv[5])
        ds, groundTruth = loadData(datasetName, featureSet)
        ds_rca = addRCA(ds, rcadim)

        alpha = float(sys.argv[6])
        print('\nUsing α =', alpha)

        filename = resultsdir + basename + '_mixed_%d_%d_%.2f' % (featureSet, rcadim, alpha)

        if os.path.exists(filename + '.result'):
            print('File exists. Skipping Test.')
        else:
            from classifier_1NN import train_1NN, train_1NN_mixed
            confusion = evaluateNfold(10, ds_rca, groundTruth, train_1NN_mixed,
                                      alpha, dropBestResult = False)
            print(confusion.results())

            writeResult(filename, confusion)

    elif classifier == 'rcasvm':
        rcadim = int(sys.argv[5])
        ds, groundTruth = loadData(datasetName, featureSet)
        ds_rca = addRCA(ds, rcadim)

        svmtype = sys.argv[6]
        kernel = sys.argv[7]
        cexp = float(sys.argv[8])
        gammaexp = float(sys.argv[9])

        filename = resultsdir + basename + '_mixed_%d_%d_%s_%s_%.1f_%.1f' % (featureSet, rcadim, svmtype, kernel, cexp, gammaexp)

        if os.path.exists(filename + '.result'):
            print('File exists. Skipping Test.')
        else:
            from classifier_SVM import train_SVM
            confusion = evaluateNfold(10, ds_rca, groundTruth, train_SVM,
                                      descriptorNames = [ '*.mean', '*.var', '*.value' ], exclude = ['*.spectral_contrast.mean'],
                                      svmtype = svmtype, kernel = kernel, c = 2**cexp, gamma = 2**gammaexp)
            print(confusion.results())

            writeResult(filename, confusion)
            
    elif classifier == 'mixedsegments':
        rcadim = int(sys.argv[5])
        ds, groundTruth = loadData(datasetName, featureSet)
        ds_rca = addRCA(ds, rcadim)

        alpha = float(sys.argv[6])
        func1 = sys.argv[7]
        func2 = sys.argv[8]
        print('\nUsing α =', alpha, ', func1 =', func1, ', func2 =', func2)

        filename = resultsdir + basename + '_mixed_%d_%d_%.2f_%s_%s' % (featureSet, rcadim, alpha, func1, func2)

        if os.path.exists(filename + '.result'):
            print('File exists. Skipping Test.')
        else:
            from classifier_1NN import train_1NN_segments
            confusion = evaluateNfold(10, ds_rca, groundTruth, train_1NN_segments,
                                      alpha, func1, func2, dropBestResult = False)
            print(confusion.results())

            writeResult(filename, confusion)

    else:
        print('ERROR: classifier needs to be one of: simca, svm, mixed')
        sys.exit(1)

