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



import sys, os, logging
import yaml
from gaia2 import *
import cPickle
import subprocess
import multiprocessing
import cStringIO
from os.path import join, split, exists
from gaia2.utils import makedir, tuplify, dictcombinations
from gaia2.classification import GroundTruth

log = logging.getLogger('gaia2.classification.ClassificationTaskManager')

validClassifiers = [ 'NN', 'svm', 'mixed', 'rcasvm',  'mixedsegments' ]
validEvaluations = [ 'nfoldcrossvalidation' ]

def taskhash(config):
    return hash(str(tuplify(config)))

class ClassificationTaskManager:
    """Main class used for classification that contains all the possible configurations we want to run.

    It will read a full project configuration from a yaml project file, and generate all the corresponding
    configurations, then its run() method should be called.

    When the run() method is called, it will spawn a single process for each configuration to be run,
    in order to be more fault-tolerant."""

    def __init__(self, yamlfile):
        try:
            conf = yaml.load(open(yamlfile).read())
        except Exception, e:
            print 'Unable to open project file:', e
            raise

        self.conf = conf

        # make sure that these directories exist
        makedir(conf['datasetsDirectory'])
        makedir(conf['resultsDirectory'])

        conf['datasetFilename'] = join(conf['datasetsDirectory'], conf['className'] + '.db')

        if 'preprocessing' not in conf:
            log.warning('No preprocessing defined in %s.' % yamlfile)
            log.warning('Setting preprocessing to default: fixlength, removeVL, cleaner')
            conf['preprocessing'] = { 'default': [ { 'transfo': 'fixlength' },
                                                   { 'transfo': 'removeVL' },
                                                   { 'transfo': 'cleaner' } ]
                                      }


        # make sure that each classifier has a list of valid preprocessed datasets specified, otherwise add them
        preprocessingSteps = conf['preprocessing'].keys()
        for trainer, configs in conf['classifiers'].items():
            for config in configs:
                if 'preprocessing' not in config:
                    log.warning('Preprocessing for classifier ' + trainer +
                                ' is not specified, using all possibilities (' + str(preprocessingSteps) + ')...')
                    config['preprocessing'] = preprocessingSteps

                # replace possible wildcards
                if config['preprocessing'] == '*':
                    config['preprocessing'] = preprocessingSteps


        if 'evaluations' not in conf:
            log.warning('No evaluations defined in %s.' % yamlfile)
            log.warning('Setting default evaluation to 10-fold cross-validation')
            conf['evaluations'] = { 'nfoldcrossvalidation': [ { 'nfold': [10] } ] }


        log.debug('Generating all possible configs...')
        self.updateAllConfigs()


    def updateAllConfigs(self):
        """Compute all the possible config combinations for training & evaluations."""
        self.allconfigs = []
        self.evalconfigs = []

        for classifier, configs in self.conf['classifiers'].items():
            if classifier not in validClassifiers:
                log.warning('Not a valid classifier: %s' % classifier)
                continue

            for conf in configs:
                dconfig = { 'classifier': [ classifier ] }
                dconfig.update(conf)
                self.allconfigs += list(dictcombinations(dconfig))

        for evaluation, configs in self.conf['evaluations'].items():
            if evaluation not in validEvaluations:
                log.warning('Not a valid evaluation: %s' % evaluation)
                continue

            for conf in configs:
                dconfig = { 'type': [ evaluation ] }
                dconfig.update(conf)
                self.evalconfigs += list(dictcombinations(dconfig))


    def loadBaseDataSet(self):
        try:
            ds = DataSet()
            ds.load(self.conf['datasetFilename'])
        except Exception, e:
            raise ValueError('Could not open the dataset file "%s" because: %s' % (dsfilename, str(e)))

        return ds


    def applyPreprocessing(self):
        """Make sure the preprocessed datasets already exist. If they don't, create them using the
        preprocessing steps specified in the project config file."""

        c = self.conf

        # only load the original dataset on demand, we won't need it if all the preprocessed
        # datasets have already been prepared
        origDataSet = None

        for name, steps in c['preprocessing'].items():
            dsfilename = join(c['datasetsDirectory'], '%s-%s.db' % (c['className'], name))

            if exists(dsfilename) :
                log.info('Preprocessed dataset %s-%s.db already exists.' % (c['className'], name))
            else:
                log.info('Doing "%s" preprocessing...' % name)
                if not origDataSet:
                    origDataSet = self.loadBaseDataSet()

                ds = origDataSet
                for step in steps:
                    ds = transform(ds, step['transfo'], step.get('params', {}))

                ds.save(dsfilename)


    def mergeDataSet(self):
        if exists(self.conf['datasetFilename']):
            log.info('Base dataset already exists!')
            return

        log.info('Merging original base dataset...')

        from gaia2.fusion import mergeAll
        mergeAll(self.conf['filelist'], self.conf['datasetFilename'], 5000, transfoFile = None)

        log.info('Original dataset successfully created!')


    def run(self):
        """Run all the evaluations in separate processes."""
        self.mergeDataSet()
        self.applyPreprocessing()

        alljobs = []
        className = self.conf['className']
        resultsdir = self.conf['resultsDirectory']
        datasetdir = self.conf['datasetsDirectory']

        while len(self.allconfigs) > 0:
            trainingparam = self.allconfigs.pop()
            taskid = taskhash(trainingparam)
            outfilename = join(resultsdir, className + str(taskid))
            dsname = join(datasetdir, '%s-%s.db' % (className, trainingparam['preprocessing']))
            gtname = self.conf['groundtruth'] #join(self.datasetdir, self.className + '.gt')

            alljobs += [ (className, outfilename, trainingparam, dsname, gtname, self.evalconfigs) ]

        log.info('-'*80)
        log.info('Setup finished, starting classification tasks.')
        log.info('Will use %d concurrent jobs.' % multiprocessing.cpu_count())

        pool = multiprocessing.Pool()
        pool.map(runSingleTest, alljobs, chunksize = 1) # use chunksize=1 to minimize the effect of memory leaks


def runSingleTest(args):
    #className, outfilename, trainingparam, dsname, gtname, evalconfig = args

    CLUSTER_MODE = True

    if CLUSTER_MODE:
        singleTaskFile = join(split(__file__)[0], 'classificationtask.py')
        proc = subprocess.Popen([ 'python', singleTaskFile ],
                                stdin  = subprocess.PIPE)
                                # do not capture stdout nor stderr, so that we have logging messages in "real-time"
                                #stdout = subprocess.PIPE,
                                #stderr = subprocess.PIPE)

        inputs = cStringIO.StringIO()
        #cPickle.dump((className, outfilename, trainingparam, dsname, gtname, evalconfig), inputs)
        cPickle.dump(args, inputs)

        stdout, stderr = proc.communicate(inputs.getvalue())

        # TODO: how do we check for errors here?
        # we can't use stderr, because that's where we log the messages...

    else:
        from classificationtask import ClassificationTask

        task = ClassificationTask()
        try:
            #task.run(className, outfilename, trainingparam, dsname, gtname, evalconfig)
            task.run(*args)
        except Exception, e:
            log.error('Running task failed: %s' % e)

