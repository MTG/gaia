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



import sys
import os
from os.path import join, exists, dirname
import subprocess
import gaia2
import gaia2.fastyaml as yaml
import gaia2.mtgdb
import gaia2.essentia
from gaia2.classification import ClassificationTaskManager
from gaia2.classification.classificationtaskmanager import taskhash


def usage():
    print 'Usage:'
    print
    print '%s essentia_extractor results_dir' % sys.argv[0]
    sys.exit(1)


def makedir(path):
    subprocess.call([ 'mkdir', '-p', path ])

def isExecutable(path):
    try:
        usermode = (os.stat(path).st_mode / 64) % 8
        return os.path.isfile(path) and (usermode % 2) == 1

    except:
        return False

def evaluateModels(extractor, resultsDir):
    if not isExecutable(extractor):
        print '%s does not seem to be an executable extractor... Exiting...' % extractor
        sys.exit(1)

    makedir(resultsDir)

    collections_file = join(gaia2.rootdir(), 'mtgdb', 'mtgdb_collections.yaml')
    collections = [ c['name'] for c in yaml.load(open(collections_file)) ]

    # do all the evaluations
    for collection in collections:
        evaluateCollection(collection, extractor, resultsDir)

    # print a report of the evaluations
    for collection in collections:
        checkErrors(collection, resultsDir)


def evaluateCollection(collectionName, extractor, resultsDir):
    print '-' * 120
    print 'Evaluating SVM classifier for:', collectionName
    print

    collection = gaia2.mtgdb.MtgdbCollection(collectionName)

    # compute the descriptors for all the files in the DB
    print 'Computing descriptors for %s DB...' % collection.name
    print

    collecDir = join(resultsDir, collectionName)
    sigsDir = join(collecDir, 'descriptors')

    makedir(sigsDir)

    subprocess.call([ 'python', '-mgaia2.essentia', extractor, collection.audioDirectory(), sigsDir ])


    # generate the classification project and run the evaluation
    print 'Running evaluation of SVM parameters for %s' % collection.name
    print

    evalDir = join(collecDir, 'evaluation')
    evalProject = join(evalDir, collection.name + '.yaml')
    makedir(evalDir)

    subprocess.call([ 'python', '-mgaia2.scripts.classification.generate_classification_project',
                      collection.name, sigsDir, evalProject ])

    subprocess.call([ 'python', '-mgaia2.scripts.classification.run_tests', evalProject ])


def checkErrors(collectionName, resultsDir):
    collec = gaia2.mtgdb.MtgdbCollection(collectionName)
    collecDir = join(resultsDir, collectionName)
    sigsDir = join(collecDir, 'descriptors')
    evalDir = join(collecDir, 'evaluation')
    evalProject = join(evalDir, collectionName + '.yaml')

    # 1- check that all sig files could be correctly computed
    sigfiles = [ p[1] for p in gaia2.essentia.sigfileList(collec, sigsDir) ]

    missingSigs = []
    for sig in sigfiles:
        if not os.path.exists(sig):
            missingSigs.append(sig)


    # 2- check that all eval combinations could be computed
    missingEvals = []
    cm = ClassificationTaskManager(evalProject)

    nec = len(cm.evalconfigs)

    for conf in cm.allconfigs:
        resultFile = join(dirname(evalProject),
                          cm.conf['resultsDirectory'],
                          cm.conf['className'] + str(taskhash(conf)) + '_%d.result')

        for i in range(nec):
            if not exists(resultFile % i):
                missingEvals.append(conf)


    # 3- summary
    print '-' * 120
    print 'For collection %s' % collectionName
    print
    if missingSigs:
        print 'Some files failed to be analyzed:'
        for sig in sorted(missingSigs):
            print ' - %s' % sig
    else:
        print 'All sigfiles computed successfully!'

    if missingEvals:
        print 'Some evaluations failed:'
        for evaluation in missingEvals:
            print ' - %s' % evaluation
    else:
        print 'All evaluations completed successfully!'

    print '-' * 120



if __name__ == '__main__':
    try:
        extractor = sys.argv[1]
        resultsDir = sys.argv[2]
    except:
        usage()

    evaluateModels(extractor, resultsDir)

