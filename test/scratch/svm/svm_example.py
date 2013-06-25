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
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see http://www.gnu.org/licenses/


# encoding: utf-8

# this file is only valid for the dataset_small.db file that is inside the
# unittest/ directory. It can probably be easily adapted to any dataset, though.

from gaia2 import *
addImportPath('..')
import testutils as utils
from os.path import join, split


import sys
if sys.platform in [ 'win32', 'cygwin' ]:
    tempdir = 'C:\\'
else:
    tempdir = '/tmp'

datadir = join(filedir(), '..', 'unittest', 'data')


def loadDataSet():
    '''this loads the dataset with the genre information put inside'''


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

    for pid, genre in genreMap.items():
        ds.point(pid).setLabel('genre', genre)

    # we're in a testing environment, prepare the dataset by cleaning it a bit more
    ds = transform(ds, 'removevl')

    # also remove the 'Sample' category, which only contains duplicates and fucks everything up...
    for p in ds.points():
        if p['genre'] == 'Sample':
            ds.removePoint(p.name())

    return ds


def writeGroundTruth(ds, filename):
    import yaml
    dsgt = DataSet()
    dsgt.load(join(datadir, 'dataset_small.db'))

    dsgt = ds.history().mapDataSet(dsgt)

    gt = {}
    for p in dsgt.points():
        gt[p.name()] = str(p['genre'])

    yaml.dump(gt, open(filename, 'w'))


def svmtrain():
    ds = loadDataSet()

    print '*'*100
    print 'creating svmtrain analyzer'
    #svm = AnalyzerFactory.create('svmtrain', { 'descriptorNames': '*mfcc.mean',
    #                                           'class': 'genre',
    #                                           'modelFilename': '/tmp/pipo.model'})

    #print 'launching analysis'
    #setDebugLevel(GAll)
    #svm.analyze(ds)
    cvar.verbose = True
    ds = transform(ds, 'normalize', { 'independent': False, 'except': '*cov' })
    ds = transform(ds, 'svmtrain', { 'descriptorNames': '*.mean',
                                     'class': 'genre' })

    ds.history().save('/tmp/test.history')


    ds = loadDataSet()
    ds = transform(ds, 'normalize', { 'independent': True, 'except': '*cov' })
    ds = transform(ds, 'extract', { 'dimensions': [ 'mfcc.mean[3]', 'spectral_centroid.var[0]' ] })
    ds = transform(ds, 'svmtrain', { 'class': 'genre',
                                     'descriptorNames': [ 'mfcc.mean[3]', 'spectral_centroid.var[0]' ] })
    ds.history().save('/tmp/test_svm_extractdim.history')
    writeGroundTruth(ds, '/tmp/test_svm_extractdim.gt.yaml')

    ds = loadDataSet()
    ds = transform(ds, 'normalize', { 'outliers': 0.05, 'except': '*cov' })
    ds = transform(ds, 'svmtrain', { 'class': 'genre',
                                     'descriptorNames': [ '*.mean' ],
                                     'probability': True })
    ds.history().save('/tmp/test_svm_probability.history')
    writeGroundTruth(ds, '/tmp/test_svm_probability.gt.yaml')



    '''
    ds = loadDataSet()
    ds = transform(ds, 'normalize', { 'independent': True, 'except': '*cov' })
    ds = transform(ds, 'extract', { 'dimensions': [ 'mfcc.mean[3]', 'spectral_centroid.var[0]' ] })
    ds = transform(ds, 'svmtrain', { 'descriptorNames': [ 'mfcc.mean[3]', 'spectral_centroid.var[0]' ],
                                     'probability': True })
    ds.history().save('/tmp/test_svm_extractdim.history')
    writeGroundTruth(ds, '/tmp/test_svm_extractdim.gt.yaml')
    '''




def svmlitetrain(filename):
    ds = readSVMLiteDataset(filename)
    '''
    print '*'*100
    print 'creating svmtrain analyzer'
    svm = AnalyzerFactory.create('svmtrain', { 'descriptorNames': 'value',
                                               'class': 'class',
                                               'modelFilename': filename + '.model'})

    print 'launching analysis'
    #setDebugLevel(GAll)
    svm.analyze(ds)
    '''

    ds = transform(ds, 'normalize', { 'independent': True })
    ds2 = transform(ds, 'svmtrain', { 'descriptorNames': 'value',
                                      'class': 'class',
                                      'modelFilename': filename + '.model'})

    ds2.history().save('/tmp/test.history')

    correct = 0
    for p in ds2.points():
        if p.label('class') == ds.point(p.name()).label('class'):
            correct += 1
    print correct, 'correct out of', ds2.totalPoints()


def readSVMLiteDataset(filename):
    data = [ l.split() for l in open(filename).readlines() ]
    minidx = maxidx = 1
    for l in data:
        for i in range(1, len(l)):
            dim, value = l[i].split(':')
            l[i] = (int(dim), float(value))
            minidx = min(minidx, int(dim))
            maxidx = max(maxidx, int(dim))

    print 'min =', minidx
    print 'max =', maxidx

    dimension = maxidx - minidx + 1

    layout = PointLayout()
    layout.add('class', StringType)
    layout.add('value', RealType)

    ds = DataSet()
    c = ds.addCollection('train')

    n = 0
    points = []
    for l in data:
        p = Point()
        p.setLayout(layout)
        p.setName('instance_%06d' % n)
        n += 1

        p.setLabel('class', l[0])
        desc = RealDescriptor(dimension, 0.0)
        for dim, value in l[1:]:
            desc[dim-minidx] = value
        p.setValue('value', desc)

        points.append(p)

    c.addPoints(points)

    return ds



if __name__ == '__main__':
    cvar.verbose = False

    svmtrain()
    trainingDataSet = None
    setDebugLevel(GAlgorithms)
    #svmlitetrain(join(basedir, 'data', 'train.1'))
