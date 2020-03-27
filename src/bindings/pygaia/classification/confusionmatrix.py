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

from __future__ import with_statement
from collections import defaultdict
import gaia2.fastyaml as yaml
from math import sqrt

class ConfusionMatrix:

    def __init__(self):
        self.matrix = defaultdict(lambda: defaultdict(list))
        self.folds = dict()

    def load(self, filename):
        with open(filename) as f:
            data = yaml.load(f.read())

        # convert to a defaultdict the data we just loaded
        self.matrix = defaultdict(lambda: defaultdict(list))
        for k, v in data['matrix'].items():
            self.matrix[k] = defaultdict(list, v)

        self.folds = data['fold']

    def save(self, filename):
        # convert to "normal" dicts before saving
        data = {
                'matrix': dict((k, dict(v)) for k, v in self.matrix.items()),
                'fold': self.folds
                }
        with open(filename, 'w') as f:
            yaml.dump(data, f)

    def add(self, expected, predicted, name = ''):
        self.matrix[expected][predicted] += [ name ]

    def addNfold(self, expected, predicted, name, nfold):
        self.matrix[expected][predicted] += [ name ]
        self.folds[name] = nfold

    def matrixNfold(self, nfold):
        nfoldDict = defaultdict(lambda: defaultdict(list))
        for c in self.matrix:
            for d in self.matrix[c]:
                for e in self.matrix[c][d]:
                    if self.folds[e] == nfold:
                        nfoldDict[c][d].append(e)
        return nfoldDict

    def stdNfold(self, normalizedAccuracies=False):
        """Return standard deviation of the accuracies across folds."""

        if normalizedAccuracies:
            accuracies = self.normalizedAccuraciesNFold()
        else:
            accuracies = self.accuraciesNFold()

        # TODO the following lines compute standard deviation. In
        # the future we can use stdev method from the statistics
        # package, shipped by default since Python 3.4
        acc_mean = sum(accuracies) / len(accuracies)

        return sqrt(sum([(x - acc_mean) * (x - acc_mean)
                    for x in accuracies]) / len(accuracies))

    def classes(self):
        allClasses = set()

        for c in self.matrix:
            allClasses.add(c)
            for d in self.matrix[c]:
                allClasses.add(d)

        return allClasses

    def total(self):
        """Return the total number of classification instances."""
        result = 0
        for c in self.matrix:
            for d in self.matrix[c]:
                result += len(self.matrix[c][d])
        return result

    def totalNfold(self, fold):
        """Return the total number of classification instances for a given fold."""
        matrix = self.matrixNfold(fold)
        result = 0
        for c in matrix:
            for d in matrix[c]:
                result += len(matrix[c][d])
        return result

    def correct(self):
        """Return the number of correctly classified instances."""
        result = 0
        for c in self.matrix:
            result += len(self.matrix[c][c])
        return result

    def correctNfold(self, fold):
        """Return the number of correctly classified instances for a given fold."""
        matrix = self.matrixNfold(fold)
        result = 0
        for c in matrix:
            result += len(matrix[c][c])
        return result

    def toDict(self):
        """Format nicely the confusion matrix as normal dict, replace list of
        instances by number of them."""
        allClasses = self.classes()

        # build resulting dict
        result = {}
        for c in allClasses:
            result[c] = {}
            for d in allClasses:
                result[c][d] = len(self.matrix[c][d])

        return result

    def results(self):
        good = self.correct()
        total = self.total()
        return 'Correctly classified: %d out of %d (%d%%)' % (good, total, 100*good//total)

    def accuraciesNFold(self):
        '''Return accuracies per fold.'''
        folds = set(self.folds.values())

        if not bool(folds):
            raise('This matrix does not contain information about folds')

        return [self.correctNfold(f) * 100. / self.totalNfold(f)
                for f in folds]

    def normalizedAccuraciesNFold(self):
        '''Returns the normalized accuracy.'''
        folds = set(self.folds.values())

        if not bool(folds):
            raise('This matrix does not contain information about folds')

        foldAccuracies = []

        for f in folds:
            classAccuracies = []
            matrix = self.matrixNfold(f)

            for c in matrix:
                classElements = 0
                for e in matrix[c]:
                    classElements += len(matrix[c][e])

                classAccuracies.append(len(matrix[c][c]) * 100. / classElements)

            foldAccuracies.append(sum(classAccuracies) / len(classAccuracies))

        return foldAccuracies

    def accuracy(self):
        accuracies = self.accuraciesNFold()
        return sum(accuracies) / len(accuracies)

    def normalizedAccuracy(self):
        accuracies = self.normalizedAccuraciesNFold()
        return sum(accuracies) / len(accuracies)

    def toHtml(self, standAlone = True, embedStyleSheet = True):
        html = '<table>'
        html += '<tr>'
        html += '<th><h3>Predicted (%)</h3></th>'
        html += '<td></td>'
        html += '</tr>'
        html += '<tr>'
        html += '<td><table>'
        html += '<tr>'

        html += '<td></td>'

        labels = sorted(self.classes())
        for predicted in labels:
            html += '<th>'+predicted+'</th>'

        html += '<td></td>'
        html += '<th>Proportion</th>'
        html += '</tr>'

        for actual in labels:
            html += '<tr>'
            html += '<th>' + actual + '</th>'

            classInstances = 0
            for predicted in self.matrix[actual].values():
                classInstances += len(predicted)

            proportion = 100.0 * classInstances / self.total()

            for predicted in labels:
                correct = len(self.matrix[actual][predicted])
                if classInstances:
                    percentage = correct * 100.0 / classInstances
                else:
                    percentage = 0

                if actual == predicted:
                    if percentage > 0:
                        html += '<td class="hi animate">'
                    else:
                        html += '<td class="animate">'
                else:
                    if percentage >= 10: # 10%, hard-coded value
                        html += '<td class="bad animate">'
                    else:
                        html += '<td class="animate">'

                html += '%.2f <span>%d %s (out of %d) classified as %s</span></td>' % (percentage, correct, actual, classInstances, predicted)
            html += '<th>' + actual + '</th>'
            html += '<td>' + "%.2f" % proportion + " %</td>"
            html += '</tr>'
        html += '</table></td>'
        html += '<th><h3>Actual (%)</h3></th>'
        html += '</tr>'
        html += '</table>'
        html += '<br/>'


        if standAlone:
            if embedStyleSheet:
                html = '''<html><head>
                <style type="text/css">body { font-family: sans-serif; font-size: small; } table { border: 1px solid black; border-collapse: collapse; font-size: small; } th { background-color: rgb(240, 240, 240); border: 1px solid black; text-align: left; padding: 5px; } td { border: 1px solid black; text-align: right; padding: 5px; } td.good, td.hi { background-color: #00ff33; } td.mediocre { background-color: #FF9900; color:#FFFFFF; } td.bad { background-color: #FF0000; color:#FFFFFF; } td.animate span { display:none } td.animate:hover span {display: block; position: absolute; padding: 5px; margin: 20px; z-index: 100; border: 1px solid black; color:black; background-color: AntiqueWhite; text-align: left;} .unroll { text-decoration:underline; color:blue; } .unroll:hover { cursor:pointer; } h2, h3 { padding: none; margin: 0.2em 0em 0em 0em; } #footer { text-align: right; clear: both; margin-top: 2em; border-top: dashed gray 1px; }</style>
                </head>
                <body>''' + html + '</body></html>'
            else:
                html = '''<html><head>
                <link rel="stylesheet" href="matrix.css" type="text/css" />
                </head>
                <body>''' + html + '</body></html>'

        return html
