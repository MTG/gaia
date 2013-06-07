#!/usr/bin/env python
# -*- coding: utf-8 -*-

from confusionmatrix import ConfusionMatrix
from groundtruth import GroundTruth, getGroundTruthFromDirectory, getGroundTruthFromLabel
from evaluation import evaluate, evaluateNfold
from classificationtaskmanager import ClassificationTaskManager
import logging

class SimpleFormatter(logging.Formatter):
    """Simple logging formatter that only prints the last part of a logger name."""
    def format(self, *args, **kwargs):
        logrec = args[0]
        logrec.name = logrec.name.split('.')[-1]
        return logging.Formatter.format(self, logrec)


loghandler = logging.StreamHandler()
loghandler.setFormatter(SimpleFormatter('%(levelname)-8s %(name)s  |  %(message)s'))

logging.getLogger('gaia2.classification').addHandler(loghandler)
logging.getLogger('gaia2.classification').setLevel(logging.INFO)
logging.getLogger('gaia2.classification.Evaluation').setLevel(logging.WARNING)
