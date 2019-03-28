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



from __future__ import with_statement

import cPickle
import os, sys
import gaia2.fastyaml as yaml
from gaia2 import DataSet, cvar
from gaia2.classification import GroundTruth, evaluate, evaluateNfold, utils
from os.path import exists
import logging
import json

log = logging.getLogger('gaia2.classification.ClassificationTask')

def getTrainer(classifier, param, ds):
    if classifier == 'simca':
        coveredVar = int(param['coveredVar'])
        alpha = float(param['alpha'])

        from gaia2.classification.classifier_SIMCA import train_SIMCA
        trainer = train_SIMCA

        descriptorNames = param.get('descriptorNames', '*')
        exclude = param.get('exclude', [])

        trainingparam = { 'descriptorNames': descriptorNames, 'exclude': exclude,
                          'alpha': alpha, 'coveredVariance': coveredVar,
                          'useBoundaryDistance': True }
        newds = ds

    elif classifier == 'svm':
        svmtype = param['type']
        kernel = param['kernel']
        cexp = param['C']
        gammaexp = param['gamma']

        descriptorNames = param.get('descriptorNames', '*')
        exclude = param.get('exclude', [])

        from gaia2.classification.classifier_SVM import train_SVM
        trainer = train_SVM

        trainingparam = { 'descriptorNames':  descriptorNames,
                          'exclude': exclude, 'svmtype': svmtype, 'kernel': kernel,
                          'c': 2**cexp, 'gamma': 2**gammaexp}
        newds = ds

    elif classifier == 'NN':
        from gaia2.classification.classifier_1NN import train_1NN
        trainer = train_1NN

        distance = param.get('distance', 'Euclidean')
        params = param.get('params', {})

        trainingparam = {'distance': distance, 'params': params}
        newds = ds

    elif classifier == 'mixed':
        rcadim = param['rcadim']

        alpha = float(param['alpha'])

        #filename = resultsdir + basename + '_mixed_%d_%d_%.2f' % (featureSet, rcadim, alpha)

        from gaia2.classification.classifier_1NN import train_1NN_mixed
        # NB: This is cheating, as we train the RCA on the whole dataset, before actually doing the folds
        #trainer = train_1NN_mixed
        #newds = utils.addRCA(ds, rcadim)
        #trainingparam = { 'dropBestResult' : False, 'alpha': alpha }

        def rcaTrainer(dataset, groundTruth, alpha, dropBestResult):
            dataset = utils.addRCA(dataset, groundTruth, rcadim)
            classifier = train_1NN_mixed(dataset, groundTruth, alpha, dropBestResult)
            return lambda p: classifier(dataset.history().mapPoint(p))

        trainer = rcaTrainer

        trainingparam = { 'dropBestResult' : False, 'alpha': alpha }
        newds = ds

    elif classifier == 'rcasvm':
        rcadim = param['rcadim']
        svmtype = param['type']
        kernel = param['kernel']
        cexp = param['C']

        descriptorNames = param.get('descriptorNames', '*')
        exclude = param.get('exclude', [])

        trainingparam = { 'descriptorNames':  descriptorNames,
                          'exclude':  exclude, 'svmtype': svmtype, 'kernel': kernel,
                          'c': 2**cexp, 'gamma': 2**gammaexp
                          }

        gammaexp = param['gamma']
        from gaia2.classification.classifier_SVM import train_SVM
        trainer = train_SVM

        newds = addRCA(ds, rcadim)

    elif classifier == 'mixedsegments':
        rcadim = param['rcadim']
        alpha = param['alpha']
        func1 = param['func1']
        func2 = param['func2']
        print '\nUsing α =', alpha, ', func1 =', func1, ', func2 =', func2
        trainer = train_1NN_segments
        trainingparam = { 'alpha': alpha, 'func1': func1, 'func2': func2, 'dropBestResult': False }
        newds = addRCA(ds, rcadim)

    else:
        raise 'ERROR: classifier needs to be one of: simca, svm, mixed'
        sys.exit(1)

    return trainer, trainingparam, newds


class ClassificationTask(object):
    def run(self, className, outfilename, param, dsname, gtname, evalconfig, seed):

        try:
            classifier = param['classifier']
            gt = GroundTruth(classifier)
            gt.load(gtname)

            # force the GroundTruth class name to be the one specified by our project file, not
            # the one in the original groundtruth file
            gt.className = className

            ds = DataSet()
            ds.load(dsname)

            # some points may have failed to be analyzed, remove those from the GroundTruth
            pnames = ds.pointNames()
            for pid in list(gt.keys()):
                if pid not in pnames:
                    log.warning('Removing %s from GroundTruth as it could not be found in the merged dataset' % pid)
                    del gt[pid]

            trainerFun, trainingparam, newds = getTrainer(classifier, param, ds)

            # run all the evaluations specified in the evaluation config
            for i, evalparam in enumerate(evalconfig):
                # if we already ran this evaluation, no need to run it again...
                resultFilename = outfilename + '_%d.result' % i
                if exists(resultFilename):
                    log.info('File %s already exists. Skipping evaluation...' % resultFilename)
                    continue

                log.info('Running evaluation %d for: %s with classifier %s and dataset %s' % (i, outfilename,
                                                                                              param['classifier'],
                                                                                              param['preprocessing']))
                log.info('    PID: %d, parameters: %s' % (os.getpid(), json.dumps(param)))

                # run evaluation
                confusion = evaluateNfold(evalparam['nfold'], ds, gt, trainerFun, seed=seed, **trainingparam)
                # write evaluation params & result
                with open(outfilename + '_%d.param' % i, 'w') as f:
                    yaml.dump({ 'model': param, 'evaluation': evalparam }, f)

                confusion.save(resultFilename)

        except Exception:
            log.error('While doing evaluation with param = %s\nevaluation = %s' % (param, evalconfig))
            raise


if __name__ == '__main__':
    config = cPickle.load(sys.stdin)

    cvar.verbose = False
    task = ClassificationTask()
    task.run(*config)
