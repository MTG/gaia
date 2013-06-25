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



from gaia2 import *
addImportPath('..')
import testutils as utils
import os
from os.path import join, split
from classification_utils import evaluate, evaluate_nfold

from classifier_1NN import train_1NN, train_1NN_mixed
from classifier_SVM import train_SVM
from classifier_SIMCA import train_SIMCA


config = { 'dataset': 'dortmund', # can be any of [ 'dataset', 'dortmund' ]

           # classifiers can contain any of [ 'baseline', 'KL-MFCC', 'mixed', 'SVM', 'SIMCA' ]
           'classifiers': [ 'SIMCA' ],

           # Feature selection: choose the descriptors you want to use for each classifier here
           'RCA descriptors': [ 'spectral*' ],
           'RCA descriptors except': [],

           'SVM descriptors': [ '*.mean', '*.var' ],
           'SVM descriptors except': [],

           'SIMCA descriptors': [ '*.mean', '*.var' ],
           'SIMCA descriptors except': []
           }


def loadDataSet():
    '''Load the dataset with the genre information put inside (as the "genre" descriptor).'''
    datadir = join(filedir(), '..', 'unittest', 'data')

    ds = DataSet()
    try:
        ds.load(join(datadir, 'dataset_small.db'))
    except Exception, e:
        print 'Error:', str(e)
        print 'You should first run the unittests to generate the dataset_small.db file before'
        print 'trying to run this example.'
        sys.exit(1)

    genreMap = utils.getClassMapFromDirectory(join(datadir, 'dataset_small'))

    # transform the dataset to add the genre information
    ds = transform(ds, 'addfield', { 'string': 'genre' })

    for p in ds.points():
        p['genre'] = genreMap[p.name()]

    # remove the 'Sample' category, which only contains duplicates and fucks everything up...
    ds.collection('dataset_small').removePoints([ p.name() for p in ds.points() if p['genre'] == 'Sample' ])

    # we're in a testing environment, prepare the dataset by cleaning it a bit more
    ds = transform(ds, 'removevl')

    return ds


def loadDortmundDataSet():
    try:
        MTGDB = os.environ['MTGDB_AUDIO']
    except KeyError:
        print 'ERROR: the environment variable MTGDB_AUDIO is not defined...'
        print '       You should set it to point to the mount point of the MTGDB audio share.'
        sys.exit(1)

    ds = DataSet()
    ds.load('/tmp/dortmund.db')
    genreMap = utils.getClassMapFromDirectory('%s/benchmarks/genre/Dortmund/descriptors/essentia_streaming_1.0.6_schizo' % MTGDB, sigfileExtension = '.neq.sig')

    # transform the dataset to add the genre information
    ds = transform(ds, 'addfield', { 'string': 'genre' })

    for p in ds.points():
        p['genre'] = genreMap[p.name()]

    # we're in a testing environment, prepare the dataset by cleaning it a bit more
    ds = transform(ds, 'removevl')

    return ds

hbar = '\n' + ' '*16 + '-'*80 + '\n'


def addRCA(ds, dim, config):
    ds_rca = transform(ds, 'fixlength')
    ds_rca = transform(ds_rca, 'select', { 'descriptorNames': config['RCA descriptors'],
                                           'except':          config['RCA descriptors except'] })
    ds_rca = transform(ds_rca, 'gaussianize')

    # if dimension is too high, we need to preprocess before with a PCA, otherwise RCA doesn't work
    l = ds_rca.layout()
    descdim = l.descriptorLocation(l.descriptorNames()).dimension(RealType, ds_rca.samplePoint())
    if descdim > 80:
        ds_rca = transform(ds_rca, 'pca', { 'dimension': 80 })

    ds_rca = mergeDataSets(ds_rca, transform(ds, 'select', { 'descriptorNames': '.genre' }))
    ds_rca = transform(ds_rca, 'rca', { 'dimension': dim, 'classLabel': 'genre' })

    return mergeDataSets(ds, ds_rca)


def descnames(config, name):
    dnames = '%s descriptors' % name
    result = config[dnames]
    if config[dnames + ' except']:
        result += ', except %s' % config[dnames + ' except']
    return result


if __name__ == '__main__':
    cvar.verbose = False

    if config['dataset'] == 'dataset':
        ds = loadDataSet()
        ds = utils.addVarFromCov(ds, 'mfcc')

    elif config['dataset'] == 'dortmund':
        ds = loadDortmundDataSet()
        ds = utils.addVarFromCov(ds, 'lowlevel.mfcc')

    else:
        print 'Unknown dataset:', config['dataset']

    groundTruth = utils.getGroundTruthFromLabel(ds, 'genre')

    # NN-classifier: euclidean distance on MFCC mean/var
    if 'baseline' in config['classifiers']:
        print hbar
        print 'Evaluating 1-NN genre classification using euclidean distance on MFCC.mean and MFCC.var:'

        '''
        classifier = train_1NN(ds, groundTruth,
                               distance = 'euclidean',
                               params = { 'descriptorNames': [ 'mfcc.mean', 'mfcc.var' ] },
                               dropBestResult = True) # because we use training dataset = test dataset

        confusion = evaluate(classifier, ds, groundTruth)
        print confusion.results()
        '''

        confusion = evaluate_nfold(10, ds, groundTruth, train_1NN,
                                   distance = 'euclidean',
                                   params = { 'descriptorNames': [ 'mfcc.mean', 'mfcc.var' ] },
                                   dropBestResult = False) # because we use training dataset != test dataset
        print confusion.results()
        open('/tmp/confusion_1NN_mfcc_meanvar.html', 'w').write(confusion.toHtml())


    # NN-classifier: KullbackLeibler distance on MFCC
    if 'KL-MFCC' in config['classifiers']:
        print hbar
        print 'Evaluating 1-NN genre classification using Kullback-Leibler distance on MFCC:'

        confusion = evaluate_nfold(10, ds, groundTruth, train_1NN,
                                   distance = 'KullbackLeibler',
                                   params = { 'descriptorName': 'mfcc' },
                                   dropBestResult = False) # because we use training dataset != test dataset

        print confusion.results()
        open('/tmp/confusion_1NN_KL_mfcc.html', 'w').write(confusion.toHtml())

    # NN-classifier: mixture of KL-MFCC & Gauss-RCA
    if 'mixed' in config['classifiers']:
        print hbar
        print '''Evaluating 1-NN genre classification using a mixture of:
   α  * Kullback-Leibler distance on MFCC
  1-α * euclidean distance on a gaussianized-RCA-ized dataset from %s''' % descnames(config, 'RCA')

        ds_rca = addRCA(ds, 10, config)

        alpha = 0.0
        while alpha <= 1.0:
            print '\nUsing α =', alpha
            confusion = evaluate_nfold(10, ds_rca, groundTruth, train_1NN_mixed,
                                       alpha, dropBestResult = False)
            print confusion.results()
            open('/tmp/confusion_1NN_mixed_alpha%.01f.html' % alpha, 'w').write(confusion.toHtml())

            alpha += 0.1


    # SVM classifier
    if 'SVM' in config['classifiers']:
        print hbar
        print 'Evaluating SVM classifier on %s:' % descnames(config, 'SVM')
        confusion = evaluate_nfold(10, ds, groundTruth, train_SVM,
                                   config['SVM descriptors'], config['SVM descriptors except'])
        print confusion.results()
        open('/tmp/confusion_SVM.html', 'w').write(confusion.toHtml())


    # SIMCA classifier
    if 'SIMCA' in config['classifiers']:
        print hbar
        print 'Evaluating SIMCA classifier on %s:' % descnames(config, 'SIMCA')
        confusion = evaluate_nfold(10, ds, groundTruth, train_SIMCA,
                                   config['SIMCA descriptors'], config['SIMCA descriptors except'],
                                   useBoundaryDistance = True)

        print confusion.results()

        open('/tmp/confusion_SIMCA.html', 'w').write(confusion.toHtml())
