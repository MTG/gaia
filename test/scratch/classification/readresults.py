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

# <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
# 
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>

from __future__ import print_function
from config import *
from common import *
import os.path
import sys


def fileTemplate(evalType):
    if evalType == 'mixed':
        return 'results/%(className)s_%(eqloud)s_mixed_%(featset)d_%(rcadim)d_%(alpha).2f.result'

    elif evalType == 'simca':
        return 'results/%(className)s_%(eqloud)s_simca_%(featset)d_%(coveredVariance)d_%(alpha).1f.result'

    elif evalType == 'svm':
        return 'results/%(className)s_%(eqloud)s_svm_%(featset)d_%(type)s_%(kernel)s_%(C).1f_%(gamma).1f.result'

    elif evalType == 'mixedsegments':
        return 'results/%(className)s_%(eqloud)s_mixed_%(featset)d_%(rcadim)d_%(alpha).2f_%(func1)s_%(func2)s.result'

    elif evalType == 'rcasvm':
        return 'results/%(className)s_%(eqloud)s_mixed_%(featset)d_%(rcadim)d_%(type)s_%(kernel)s_%(C).1f_%(gamma).1f.result'

    else:
        raise ValueError('invalid evalType: %s' % evalType)


def getAllFilenames():
    filenames = []

    for c in getAllConfigs():
        d = c._asdict()
        d.update({ 'className': className })

        filenames += [ (c, fileTemplate(c[0]) % d) ]

    return filenames


def getMissingConfigs():
    configs = []
    for c in getAllConfigs():
        d = c._asdict()
        d.update({ 'className': className })

        filename = fileTemplate(c[0]) % d
        if not os.path.exists(filename):
            configs.append(c)

    return configs


def readResults():
    results = {}

    # try to load all the results
    for c, filename in getAllFilenames():
        try:
            results[c] = float(open(filename).read())
        except:
            pass
            print('Could not find result file for', c)

    print('results read')
    return results


def missing():
    missing = 0
    filenames = getAllFilenames()

    for c, f in filenames:
        if not os.path.exists(f):
            missing += 1

    print('Missing: %d out of %d' % (missing, len(filenames)))
