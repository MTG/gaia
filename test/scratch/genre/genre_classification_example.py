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


# encoding: utf-8

# this file is only valid for the dataset_small.db file that is inside the
# unittest/ directory. It can probably be easily adapted to any dataset, though.

from gaia2 import *
addImportPath('..')
import testutils as utils
from os.path import join, split
import nnclassifier


import sys
if sys.platform in [ 'win32', 'cygwin' ]:
    tempdir = 'C:\\'
else:
    tempdir = '/tmp'



def loadDataSet():
    '''this loads the dataset with the genre information put inside'''

    basedir = join(filedir(), '..', 'unittest', 'data')

    ds = DataSet()
    try:
        ds.load(join(basedir, 'dataset_small.db'))
    except Exception, e:
        print 'Error:', str(e)
        print 'You should first run the unittests to generate the dataset_small.db file before'
        print 'trying to run this example.'
        sys.exit(1)

    genreMap = utils.getClassMapFromDirectory(join(basedir, 'dataset_small'))

    # transform the dataset to add the genre information
    ds = transform(ds, 'addfield', { 'string': 'genre' })

    for pid, genre in genreMap.items():
        ds.point(pid).setLabel('genre', genre)

    # we're in a testing environment, prepare the dataset by cleaning it a bit more
    ds = transform(ds, 'removevl')

    # also remove the 'Sample' category, which only contains duplicates and fucks everything up...
    for p in ds.points():
        if p.label('genre') == 'Sample':
            ds.removePoint(p.name())

    return ds




def testMFCC():
    '''Does a simple test by comparing which of MFCC-SG + Kullback-Leibler distance or
    MFCC-mean/var + euclidean distance gives the best results.'''
    ds = loadDataSet()

    ds = utils.addVarFromCov(ds, 'mfcc')

    groundTruth = utils.getGroundTruthFromLabel(ds, 'genre')


    print 'Evaluating 1-NN genre classification using euclidean distance on mfcc.mean and mfcc.var:'
    confusion = nnclassifier.evaluate_1NN(ds, groundTruth,
                                          'euclidean',
                                          { 'descriptorNames': [ 'mfcc.mean', 'mfcc.var' ] })
    print confusion.results()

    cmfile = join(tempdir, 'confusion_meanvar.html')
    open(cmfile, 'w').write(confusion.toHtml())
    print '(wrote confusion matrix to %s)' % cmfile
    print


    print 'Evaluating 1-NN genre classification using Kullback-Leibler distance on mfcc:'
    confusion = nnclassifier.evaluate_1NN(ds, groundTruth,
                                          'kullbackleibler',
                                          { 'descriptorName': 'mfcc' })
    print confusion.results()
    cmfile = join(tempdir, 'confusion_singlegaussian.html')
    open(cmfile, 'w').write(confusion.toHtml())
    print '(wrote confusion matrix to %s)' % cmfile
    print


def testClassify():
    ds = loadDataSet()


    # get ground truth
    groundTruth = utils.getGroundTruthFromLabel(ds, 'genre')

    ds_genre = transform(ds, 'select', { 'descriptorNames': 'genre' })
    ds_mfcc = transform(ds, 'select', { 'descriptorNames': 'mfcc*' })

    # transform dataset
    ds = transform(ds, 'select', { 'descriptorNames': [ 'spectral*', 'genre' ] })
    ds_base = transform(ds, 'fixlength', { 'descriptorNames': '*' })
    ds = transform(ds_base, 'gaussianize')
    ds = transform(ds, 'rca', { 'dimension': 10, 'classLabel': 'genre' })
    ds = mergeDataSets(ds, ds_genre)
    ds = mergeDataSets(ds, ds_mfcc)


    # launch classification/evaluation
    from nnclassifier import evaluate_1NN

    results = []
    alpha = 0.0
    while alpha <= 1.0:
        print 'alpha =', alpha
        confusion = evaluate_1NN(ds, groundTruth, 'linearcombination',
                                 { 'mfcc_kl': { 'name': 'kullbackleibler',
                                                'weight': alpha,
                                                'params': { 'descriptorName': 'mfcc' } },

                                   'spectral_euclidean': { 'name': 'euclidean',
                                                           'weight': 1-alpha,
                                                           'params': { 'descriptorNames': 'rca*' } }
                                   })
        good = confusion.correct()
        total = confusion.total()
        print 'correctly classified:', good, 'out of', total, '(%d%%)' % int(100*good/total)
        results.append(confusion.correct())
        alpha += 0.1

    # display results
    plotResults = False
    if plotResults:
        import pylab
        pylab.plot(results)
        pylab.show()




def example1():
    print '-'*100
    print '    Example 1'
    print
    print 'Comparing results of genre classification using:'
    print ' - MFCC, mean & var, euclidean distance'
    print ' - MFCC single gaussian, symmetric Kullback-Leibler divergence distance'
    print

    testMFCC()


def example2():
    print '-'*100
    print '    Example 2'
    print
    print 'Comparing results of genre classification using KL distance, gaussianize + RCA + euclidean distance,'
    print 'and mixtures of both using a parameter alpha: alpha*KL + (1-alpha)*RCA:'
    testClassify()


if __name__ == '__main__':
    cvar.verbose = False

    example1()
    example2()
