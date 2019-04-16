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

import sys, os.path, shutil
from gaia2.fastyaml import yaml
from get_classification_results import ClassificationResults
from generate_svm_history_from_config import trainSVMHistory
from gaia2.classification import ConfusionMatrix
from optparse import OptionParser

def selectBestModel(project_file, results_model_file, n_ranking=10):
    f = open(results_model_file + '.results.html', 'w')

    project = yaml.load(open(project_file, 'r'))
    className = project['className']
    results_dir = project['resultsDirectory']

    if os.path.exists(results_dir):
        classifierType = None # all types

        cr = ClassificationResults()
        print('Loading all results...')
        cr.readResults(results_dir)

        topResults = cr.best(min(len(cr.results), n_ranking), classifierType)

        accuracy, stdev, normAccuracy, normStdev, filename, params = topResults[0]
        print('RESULT {}\tAcc:{:.3f}\tStd:{:.3f}\tnormalized Acc:{:.3f}\tnormalized Std:{:.3f}\t{}'.format(
            project_file, accuracy, stdev, normAccuracy, normStdev, filename))

        f.write('<h1>%s (%s)</h1>\nAccuracy: %s. Std: %s.\nNormalized accuracy: %s. Normalized std: %s.\n' %
                (className, project_file, accuracy, stdev, normAccuracy, normStdev))

        cm = ConfusionMatrix()
        cm.load(filename)
        f.write(cm.toHtml())

        filename = filename.replace('.result', '.param')

        trainSVMHistory(project_file, filename, results_model_file, className)
        shutil.copyfile(filename, results_model_file + '.param')

        topResultsDict = {idx: {'accuracy': entry[0], 'std': entry[1], 'normAccuracy': entry[2],
                                'normStd': entry[3], 'config': entry[4]}
                          for idx, entry in enumerate(topResults)}
        yaml.dump(topResultsDict, open(
            results_model_file + '.results.ranking', 'w'))

    else:
        print("RESULT " + "No results found for ", project_file, ": cannot build a model")
        f.write('<h1>%s (%s) </h1>\nResults not found\n' % (collection, project_file))


if __name__ == '__main__':
    parser = OptionParser(usage = '%prog [options] project_file results_model_file')

    options, args = parser.parse_args()

    try:
        project_file = args[0]
        results_model_file = args[1]
    except:
        parser.print_help()
        sys.exit(1)

    selectBestModel(project_file, results_model_file)
