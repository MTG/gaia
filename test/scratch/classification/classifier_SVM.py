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

