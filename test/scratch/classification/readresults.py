#!/usr/bin/env python
# -*- coding: utf-8 -*-

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
            print 'Could not find result file for', c

    print 'results read'
    return results


def missing():
    missing = 0
    filenames = getAllFilenames()

    for c, f in filenames:
        if not os.path.exists(f):
            missing += 1

    print 'Missing: %d out of %d' % (missing, len(filenames))
