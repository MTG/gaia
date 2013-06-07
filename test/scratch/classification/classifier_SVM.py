#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import DataSet, transform

def train_SVM(dataset, groundTruth, descriptorNames, exclude = [], svmtype = 'c-svc', kernel = 'rbf', c = 1, gamma = 1):
    # recreate a copy of the given dataset without history
    ds = DataSet()
    ds.addPoints([ p for p in dataset.points() ])

    ds = transform(ds, 'normalize', { 'descriptorNames': descriptorNames,
                                      'except': exclude,
                                      'independent': True })

    ds = transform(ds, 'svmtrain', { 'descriptorNames': descriptorNames,
                                     'except': exclude,
                                     'className': groundTruth.className,
                                     'type': svmtype,
                                     'kernel': kernel,
                                     'c': c,
                                     'gamma': gamma})

    h = ds.history()
    return lambda p: str(h.mapPoint(p)[groundTruth.className])

