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



from ntuple import namedtuple
from config import *

# this variable should be filled with the namedtuples corresponding to the parameters
# that can be used for each evaluation type
paramType = {}

for evalType, params in orderedEvalConfig.items():
    # evalType needs to be the first field so we always know where to get it from
    allparams = [ 'evalType' ] + commonConfig.keys() + [ p.keys()[0] for p in params ]
    paramType[evalType] = namedtuple('Param_' + evalType, ' '.join(allparams))


def toParam(values):
    if isinstance(values, dict):
        return paramType[values['evalType']](**values)
    else:
        # values[0] is hardcoded here: it's a fixed pos field, and we know it always has to be there
        return paramType[values[0]](*values)

def keyMatch(pattern, key):
    for p, k in zip(pattern, key):
        if p is not None and p != k:
            return False

    return True

def findMatchingKeys(query, db):
    seedKey = toParam(query)
    return sorted([ key for key, value in db.items() if keyMatch(seedKey, key) ])



def combinations(vrange):
    '''From a list of possible values in each position, return a list of all possible tuples.'''
    if not vrange:
        return [()]

    result = []
    subcombs = combinations(vrange[1:])
    for first in vrange[0]:
        for subcomb in subcombs:
            result.append((first,) + subcomb)

    return result


def getAllConfigs():
    '''Return all possible configurations given the parameter ranges contained in the config file.'''
    configs = []
    commonParams = combinations ([ d.values()[0] for d in orderedCommonConfig ])

    # first get all possible evaluations
    for evalType, conf in evalConfig.items():
        evalParams = []
        for p in orderedEvalConfig[evalType]:
            evalParams.append(conf[p.keys()[0]])

        configs += [ toParam((evalType,) + cp + ep) for cp in commonParams for ep in combinations(evalParams) ]

    return configs

