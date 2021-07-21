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


from __future__ import print_function, absolute_import

import logging
import os

import yaml
from gaia2 import *
from multiprocessing import cpu_count
from concurrent.futures import ProcessPoolExecutor, as_completed
from six.moves import cStringIO
from os.path import join, split, exists
from gaia2.utils import makedir, tuplify, dictcombinations
from gaia2.classification import GroundTruth
from time import time
import sys


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

    def __init__(self, yamlfile, callback=None):
        try:
            conf = yaml.load(open(yamlfile).read())
        except Exception as e:
            print('Unable to open project file:', e)
            raise

        self.conf = conf
        self.callback = callback

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
        preprocessingSteps = list(conf['preprocessing'].keys())
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

    def gaia_cpu_count(self):
        """Find the number of processes that should be used concurrently when training a model.
        First, look in the environment variable `GAIA_NUM_TRAINING_PROCESSES`. If the value is
        "all", then use the full number of cores available on the CPU, otherwise interpret it
        as a number. If the value isn't a number, use all cores.
        If this value isn't set, look at the numberProcesses option in the project file, with
        the same behaviour.
        If the environment variable isn't set and no config option is in the project file, use
        the full number of cores available on the CPU."""
        config_env = "GAIA_NUM_TRAINING_PROCESSES"
        environ_cpu_count = os.environ.get(config_env)
        if environ_cpu_count:
            if environ_cpu_count == "all":
                return cpu_count(), "environ"
            try:
                return int(environ_cpu_count), "environ"
            except ValueError:
                return cpu_count(), "default - error reading environ"

        config_cpu_count = self.conf.get('numberProcesses', False)
        if config_cpu_count:
            if config_cpu_count == "all":
                return cpu_count(), "config"
            try:
                return int(config_cpu_count), "config"
            except ValueError:
                return cpu_count(), "default - error reading config"

        return cpu_count(), "default"

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
        except Exception as e:
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
        # exclude tags fields as they may not be consistent among tracks
        # and they don't contain relevant information for classification
        mergeAll(self.conf['filelist'], self.conf['datasetFilename'], 5000, transfoFile = None,
                 exclude = self.conf['excludedDescriptors'], failOnUnmatched = self.conf['failOnUnmatched'])

        log.info('Original dataset successfully created!')

    def estimateETA(self, durationEMA, duration, jobcount, jobidx, concurrentJobs):
        # use a moving average to estimate the duration of a job.
        if not durationEMA:
            durationEMA = duration
        else:
            durationEMA = (durationEMA + duration) / 2.0

        # Estimated Time of Arrival is computed considering remaining
        # number of jobs and the amount of parallel workers.
        return durationEMA * (jobcount - jobidx) / concurrentJobs

    def run(self):
        """Run all the evaluations in separate processes."""
        self.mergeDataSet()
        self.applyPreprocessing()

        alljobs = []
        className = self.conf['className']
        resultsdir = self.conf['resultsDirectory']
        datasetdir = self.conf['datasetsDirectory']
        seed = self.conf['seed']

        jobidx = 1
        jobcount = len(self.allconfigs)

        while len(self.allconfigs) > 0:
            trainingparam = self.allconfigs.pop()
            taskid = taskhash(trainingparam)
            outfilename = join(resultsdir, className + str(taskid))
            dsname = join(datasetdir, '%s-%s.db' % (className, trainingparam['preprocessing']))
            gtname = self.conf['groundtruth'] #join(self.datasetdir, self.className + '.gt')

            alljobs += [ (className, outfilename, trainingparam, dsname, gtname,
                          self.evalconfigs, seed, jobidx, jobcount) ]

            jobidx += 1

        concurrentJobs, numJobsSource = self.gaia_cpu_count()

        log.info('-'*80)
        log.info('Setup finished, starting classification tasks.')
        log.info('Will use %d concurrent jobs (value from %s).' % (concurrentJobs, numJobsSource))

        with ProcessPoolExecutor(max_workers=concurrentJobs) as executor:
            futures = [executor.submit(runSingleTest, job,
                                       cluster_mode=self.conf['clusterMode']) for job in alljobs]

            jobidx = 1
            elapsedTime = 0
            durationEMA = None

            for job_result in as_completed(futures):
                if self.callback is not None:
                    duration, params = job_result.result()
                    elapsedTime += duration
                    eta = self.estimateETA(durationEMA, duration, jobcount,
                                           jobidx, concurrentJobs)

                    self.callback(jobidx, jobcount, elapsedTime, eta, params)

                    jobidx += 1

    @staticmethod
    def classificationTaskCallback(jobidx, jobcount, elapsed_time, eta, params):
        """ ClassificationTaskManager example callback function.

        This callback prints the results of each job to the
        classificationTaskManager logger.

        Arguments:

        jobidx: Current job index.
        jobcount: Total number of jobs.
        elapsed_time: Elapsed time in seconds.
        eta: Estimated Time of Arrival in seconds.
        params: Combination of parameters for the last job.
        """
        jobdigits = len(str(jobcount))

        log.info('Job: %.*d/%d. Elapsed time: %.3fs. ETA: %.3fs' % (jobdigits, jobidx,
                                                                    jobcount, elapsed_time,
                                                                    eta))
        log.info('Params: %s' % (params))


def runSingleTest(args, cluster_mode=False):

    start_time = time()

    if cluster_mode:
        from six.moves import cPickle
        import subprocess

        singleTaskFile = join(split(__file__)[0], 'classificationtask.py')
        proc = subprocess.Popen([sys.executable, singleTaskFile],
                                stdin  = subprocess.PIPE)
                                # do not capture stdout nor stderr, so that we have logging messages in "real-time"
                                #stdout = subprocess.PIPE,
                                #stderr = subprocess.PIPE)

        inputs = cStringIO()
        cPickle.dump(args, inputs)

        stdout, stderr = proc.communicate(inputs.getvalue())

        # TODO: how do we check for errors here?
        # we can't use stderr, because that's where we log the messages...

    else:
        from .classificationtask import ClassificationTask

        task = ClassificationTask()
        try:
            #task.run(className, outfilename, trainingparam, dsname, gtname, evalconfig)
            task.run(*args)
        except Exception as e:
            log.error('Running task failed: %s' % e)

    end_time = time()

    # test duration, params
    return end_time - start_time, args[2]
