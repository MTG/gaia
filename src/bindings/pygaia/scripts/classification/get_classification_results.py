#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys, glob
from os.path import join, splitext
from gaia2.classification import ConfusionMatrix
import gaia2.fastyaml as yaml
from gaia2.utils import TextProgress

class ClassificationResults(object):
    def __init__(self):
        self.results = []

    def readResults(self, dir):
        """Reads all the results file contained in the given directory, and generates the
        associated ConfusionMatrix for each one."""

        resultFiles = glob.glob(join(dir, '*.result'))
        progress = TextProgress(len(resultFiles))

        for i, filename in enumerate(resultFiles):
            cm = ConfusionMatrix()
            cm.load(filename)

            paramFile = splitext(filename)[0] + '.param'
            params = yaml.load(open(paramFile).read())

            self.results += [ (filename, cm, params) ]

            progress.update(i+1)

    def best(self, n = 10, classifierType = None):
        if classifierType is not None:
            r = [ (cm.correct() * 100. / cm.total(), filename, param)
                  for (filename, cm, param) in self.results if param['model']['classifier'] == classifierType ]
        else:
            r = [ (cm.correct() * 100. / cm.total(), filename, param)
                  for (filename, cm, param) in self.results ]

        r = sorted(r)[::-1][:n]

        return r

    def filterResults(self, filters):
        result = self.results
        for param, value in filters.items():
            result = [ (filename, cm, param) for (filename, cm, param) in result if param['model'][param] == value ]

        return result


if __name__ == '__main__':
    try:
        resultsdir = sys.argv[1]
    except:
        print 'Usage: %s results_dir [classifierType]' % sys.argv[0]
        exit(1)

    try:
        classifierType = sys.argv[2]
    except:
        classifierType = None

    cr = ClassificationResults()
    print 'Loading all results...'
    cr.readResults(resultsdir)

    print 'Best parameters:'
    for r, filename, params in cr.best(10, classifierType):
        print '*'*100
        print 'Correct classification: %2f%%' % r
        print 'Filename:', filename

        model = params['model']
        print 'Classifier:', model['classifier']
        print 'Parameters:'
        del model['classifier']
        print '    ' + yaml.dump(model).replace('\n', '\n    ')[:-4]
