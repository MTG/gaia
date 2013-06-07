#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import DataSet, transform

def train_SVM(dataset, groundTruth, descriptorNames, exclude = [], svmtype = 'c-svc', kernel = 'rbf', c = 1, gamma = 1):
    # recreate a copy of the given dataset without history
    ds = dataset.copy()
    ds.forgetHistory()

    ds = transform(ds, 'select', { 'descriptorNames': descriptorNames,
                                   'except': exclude })

    ds = transform(ds, 'cleaner')

    ds = transform(ds, 'normalize', { 'independent': True })

    ds = transform(ds, 'addfield',  { 'string': groundTruth.className })

    for p in ds.points():
        p[groundTruth.className] = groundTruth[p.name()]

    ds = transform(ds, 'svmtrain', { 'className': groundTruth.className,
                                     'type': svmtype,
                                     'kernel': kernel,
                                     'c': c,
                                     'gamma': gamma })

    h = ds.history()
    return lambda p: h.mapPoint(p)[groundTruth.className]

