#!/usr/bin/env python

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


# encoding: utf-8

from gaia2 import RealDescriptor, transform, utils
import glob
from os.path import join, split, exists


def addVarFromCov(ds, desc):
    '''Adds the .var aggregate descriptor to the specified descriptor using its
    .cov aggregate, for all the points in the dataset.'''

    ds = transform(ds, 'addfield', { 'real': desc + '.var' })

    # add the .var descriptor using .cov (it's the diagonal of the matrix)
    for p in ds.points():
        m = utils.toMatrix(p.value(desc + '.cov'))
        dim = len(m)
        diag = RealDescriptor(dim, 0.0)
        for i in range(dim):
            diag[i] = m[i][i]
        p.setValue(desc + '.var', diag)

    return ds


def getClassMapFromDirectory(basedir, sigfileExtension = '.sig'):
    '''Given a hierarchy where the .sig files are contained in directories
    which name is the class they belong to, return a map from filename -> class.
    '''
    if not exists(basedir):
        raise IOError("Directory '%s' doesn't exist" % basedir)

    classes = glob.glob(join(basedir, '*'))
    classes = [ split(s)[-1] for s in classes ]

    # get class map: point id -> class
    classMap = {}
    for cls in classes:
        files = glob.glob(join(basedir, cls, '*'))

        # keep only the base filename
        files = [ split(f)[-1] for f in files ]

        # remove the trailing '.sig'
        files = [ f[:-len(sigfileExtension)] for f in files ]

        for f in files:
            classMap[f] = cls

    return classMap


class GroundTruth(dict):
    def __init__(self, className, *args, **kwargs):
        super(GroundTruth, self).__init__(*args, **kwargs)
        self.className = className


def getGroundTruthFromLabel(ds, labelName):
    '''Returns a ground truth map (eg: point ID -> class) for all the points in
    the dataset using the specified descriptor as class.'''

    groundTruth = GroundTruth(labelName)

    for p in ds.points():
        groundTruth[p.name()] = unicode(p.label(labelName))

    return groundTruth

