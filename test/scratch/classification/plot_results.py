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



from config import *
from common import *
from collections import defaultdict
from matplotlib.font_manager import FontProperties


def createCurves(points, xaxis):
    # assumes that y value is the result value
    curves = defaultdict(list)

    for k, value in points: # all points on all curves
        xpos = list(k._fields).index(xaxis) # NB: starting with python 2.6, the tuple.index method exists
        ckey = k[:xpos] + (None,) + k[xpos+1:] # the X axis is not part of the key (curve name) anymore
        curves[toParam(ckey)] += [ (k[xpos], value) ] # tuple of point (x, y)

    # get curves sorted by alphanumerical order, and their points sorted by X
    for c, values in curves.items():
        curves[c] = sorted(values)
    curves = sorted(curves.items())

    return curves


def getCurves(db, xaxis, parms = {}, cond = lambda x: True):
    pattern = toParam(parms)
    points = []

    for key, value in db.items():
        if keyMatch(pattern, key) and cond(key):
            points.append((key, value))

    return createCurves(points, xaxis)


def plotCurves(curves, xaxis, legendPos = 'upper right', lineStyleVar = None):
    '''
    lineStyleVar is the name of the variable which should be used to choose the style for the
    plotting of the lines in the curve, eg: choose 'eqloud' if you want eqloud to be plain lines,
    and neqloud to be dotted.
    '''
    fig = plt.figure()
    ax = fig.add_subplot(111)

    # draw curves
    lineStyles = [ '-', '--', '-.', ':' ]
    if lineStyleVar:
        possibleValues = sorted(list(set(getattr(x, lineStyleVar) for x, values in curves)))

    for k, values in curves:
        if lineStyleVar:
            styleIdx = possibleValues.index(getattr(k, lineStyleVar))
            style = lineStyles[styleIdx % len(lineStyles)]
            ax.plot(*(zip(*values) + [ style ]))
        else:
            ax.plot(*zip(*values))


    ax.set_xlabel(xaxis)
    ax.set_ylabel('correct classification')
    ax.set_title('%s Classification Results' % className)

    # add legend
    fp = FontProperties(size = 8)

    # curve name = all params without the one used for the X axis
    def curveName(k, xaxis):
        cname = 'f:%d %s' % (k.featset, k.eqloud)
        otherp = [ (p, value) for p, value in zip(k._fields, k)[3:] if p != xaxis ]
        for p, value in otherp:
            cname += ' %s=%s' % (p, str(value))

        return cname

    legend = [ curveName(k, xaxis) for k, values in curves ]
    ax.legend(legend, legendPos, shadow=True, prop = fp, pad = 0.1)

    return fig


def loadEvaluationData():
    import cPickle

    cached = False
    if cached:
        cpick = cPickle.load(open('results.db'))
        c = dict([ (toParam(k), v) for k, v in cpick ])
    else:
        from readresults import readResults
        c = readResults()
        cpick = [ (tuple(k), v) for k, v in c.items() ]
        cPickle.dump(cpick, open('results.db', 'w'))

    return c

if __name__ == '__main__':
    import matplotlib.pyplot as plt

    db = loadEvaluationData()

    def plotc(evalType, featset, xaxis, eargs = {}, cond = lambda x: True, lineStyleVar = None, legendPos = 'upper right'):
        args = { 'evalType': evalType,
                 'featset': featset }
        args.update(eargs)

        return plotCurves(getCurves(db, xaxis, args, cond), xaxis, lineStyleVar = lineStyleVar, legendPos = legendPos)


    featset = 3
    plots = [ #plotc('mixed', 1, 'alpha'),
              plotc('mixed', featset, 'alpha', lineStyleVar = 'eqloud'),
              plotc('simca', featset, 'alpha', cond = lambda x: x.coveredVariance >= 50, lineStyleVar = 'eqloud', legendPos = 'lower right'),
              plotc('simca', featset, 'coveredVariance', lineStyleVar = 'eqloud', legendPos = 'lower right'),
              plotc('svm', featset, 'gamma', { 'type': 'C-SVC', 'kernel': 'RBF' }, lineStyleVar = 'eqloud', legendPos = 'lower left'),
              plotc('svm', featset, 'gamma', { 'type': 'nu-SVC', 'kernel': 'RBF' }, lineStyleVar = 'eqloud', legendPos = 'lower left'),
              plotc('svm', featset, 'gamma', { 'type': 'C-SVC', 'kernel': 'poly' }, lineStyleVar = 'eqloud', legendPos = 'lower right'),
              plotc('svm', featset, 'gamma', { 'type': 'nu-SVC', 'kernel': 'poly' }, lineStyleVar = 'eqloud', legendPos = 'lower right'),
              ]

    plt.show()


