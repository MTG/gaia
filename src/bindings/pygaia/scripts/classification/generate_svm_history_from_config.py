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



import sys, yaml
from os.path import *
from gaia2 import DataSet, transform
from gaia2.classification import GroundTruth


def trainSVM(dataset, groundTruth, descriptorNames = ['*'], exclude = [], type = 'c-svc', kernel = 'rbf', C = 1, gamma = 1):
    ds = dataset

    ds = transform(ds, 'select', { 'descriptorNames': descriptorNames,
                                   'except': exclude })

    ds = transform(ds, 'cleaner')

    ds = transform(ds, 'normalize', { 'independent': True })

    ds = transform(ds, 'addfield',  { 'string': groundTruth.className })

    for p in ds.points():
        p[groundTruth.className] = groundTruth[p.name()]

    ds = transform(ds, 'svmtrain', { 'className': groundTruth.className,
                                     'type': type,
                                     'kernel': kernel,
                                     'c': 2**C,
                                     'gamma': 2**gamma,
                                     'probability': True })

    ds = transform(ds, 'select', { 'descriptorNames': '%s*' % groundTruth.className })

    return ds.history()



def trainSVMHistory(configFilename, paramsFilename, outputHistoryFilename, className):
    config = yaml.load(open(configFilename).read())
    params = yaml.load(open(paramsFilename).read())['model']

    if params.pop('classifier') != 'svm':
        raise Exception('Can only use this script on SVM config parameters.')

    preproc = params.pop('preprocessing')

    ds = DataSet()
    ds.load(join(split(configFilename)[0], # base dir
                 config['datasetsDirectory'], # datasets dir
                 '%s-%s.db' % (config['className'], preproc))) # dataset name

    gt = GroundTruth.fromFile(config['groundtruth'])

    if className:
        gt.className = className

    # add 'highlevel.' in front of the descriptor, this is what will appear in the final Essentia sigfile
    gt.className = 'highlevel.' + gt.className

    # do the whole training
    h = trainSVM(ds, gt, **params)

    h.save(outputHistoryFilename)



if __name__ == '__main__':
    try:
        configFilename = sys.argv[1]
        paramsFilename = sys.argv[2]
        outputHistoryFilename = sys.argv[3]

    except:
        print 'usage: %s classification_project params_filename output_history [class_name]' % sys.argv[0]
        sys.exit(1)

    className = None
    if len(sys.argv) > 4:
        className = sys.argv[4]

    trainSVMHistory(configFilename, paramsFilename, outputHistoryFilename, className)
