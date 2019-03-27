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



from gaia2 import DataSet
from gaia2.utils import TextProgress
from groundtruth import GroundTruth
from confusionmatrix import ConfusionMatrix
import random
import logging

log = logging.getLogger('gaia2.classification.Evaluation')



def evaluate(classifier, dataset, groundTruth, confusion = None, nfold=None, verbose=True):
    """Evaluate the classifier on the given dataset and returns the confusion matrix.

    Uses only the points that are in the groundTruth parameter for the evaluation.

    Parameters
    ----------

    classifier  : a function which given a point returns its class
    dataset     : the dataset from which to get the points
    groundTruth : a map from the points to classify to their respective class
    """

    progress = TextProgress(len(groundTruth))
    done = 0

    confusion = confusion or ConfusionMatrix()

    for pointId, expected in groundTruth.items():
        try:
            found = classifier(dataset.point(pointId))
            if nfold is None:
                confusion.add(expected, found, pointId)
            else:
                confusion.addNfold(expected, found, pointId, nfold)

        except Exception, e:
            log.warning('Could not classify point "%s" because %s' % (pointId, str(e)))
            raise

        done += 1
        if verbose: progress.update(done)

    return confusion


def evaluateNfold(nfold, dataset, groundTruth, trainingFunc, *args, **kwargs):
    """Evaluate the classifier on the given dataset and returns the confusion matrix.

    The evaluation is performed using n-fold cross validation.
    Uses only the points that are in the groundTruth parameter for the evaluation.

    Parameters
    ----------

    nfold        : the number of folds to use for the cross-validation
    dataset      : the dataset from which to get the points
    groundTruth  : a map from the points to classify to their respective class
    trainingFunc : a function which will train and return a classifier given a dataset,
                   the groundtruth, and the *args and **kwargs arguments
    """
    log.info('Doing %d-fold cross validation' % nfold)
    classes = set(groundTruth.values())
    progress = TextProgress(nfold, 'Evaluating fold %(current)d/%(total)d')

    # get map from class to point names
    iclasses = {}
    for c in classes:
        iclasses[c] = [ p for p in groundTruth.keys() if groundTruth[p] == c ]
        random.shuffle(iclasses[c])

    # get folds
    folds = {}
    for i in range(nfold):
        folds[i] = []
        for c in iclasses.values():
            foldsize = (len(c)-1)//nfold + 1 # -1/+1 so we take all instances into account, last fold might have fewer instances
            folds[i] += c[ foldsize * i : foldsize * (i+1) ]

    # build sub-datasets and run evaluation on them
    confusion = None
    pnames = [ p.name() for p in dataset.points() ]

    for i in range(nfold):
        if log.isEnabledFor(logging.INFO):
            progress.update(i+1)

        trainds = DataSet()
        trainds.addPoints([ dataset.point(pname) for pname in pnames if pname not in folds[i] ])
        traingt = GroundTruth(groundTruth.className, dict([ (p, c) for p, c in groundTruth.items() if p not in folds[i] ]))

        testds = DataSet()
        testds.addPoints([ dataset.point(str(pname)) for pname in folds[i] ])
        testgt = GroundTruth(groundTruth.className, dict([ (p, c) for p, c in groundTruth.items() if p in folds[i] ]))

        classifier = trainingFunc(trainds, traingt, *args, **kwargs)
        confusion = evaluate(classifier, testds, testgt, confusion, nfold=i, verbose=False)

    return confusion
