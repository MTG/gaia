#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2006-2019  Music Technology Group - Universitat Pompeu Fabra
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

import os.path, csv, argparse
from gaia2.fastyaml import yaml
from get_classification_results import ClassificationResults
from optparse import OptionParser


def generateParamsReport(project_file):
    project = yaml.load(open(project_file, 'r'))
    results_dir = project['resultsDirectory']
    csv_file = project_file.rstrip('project') + 'report.csv'

    if os.path.exists(results_dir):
        classifierType = None # all types

        cr = ClassificationResults()
        print('Loading all results...')
        cr.readResults(results_dir)

        n_results = len(cr.results)
        results = cr.best(n_results, classifierType)
        # results is a list of tuples sorted by accuracy with these fields:
        # (accuracy, std, normAccuracy, normStd, results, results_file)

        # use position as index and sort by normAccuracy
        r = [(v[2], i) for i, v in enumerate(results)]
        r = sorted(r)[::-1]
        _, idx = zip(*r)

        fieldnames = ['accuracy_rank',
                      'accuracy',
                      'std',
                      'normalized_accuracy_rank',
                      'normalized_accuracy',
                      'normalized_std',
                      'results_file',
                      'param_file',
                      'parameters']

        results = [{fieldnames[0]: k + 1,
                    fieldnames[1]: v[0],
                    fieldnames[2]: v[1],
                    fieldnames[3]: idx.index(k) + 1,
                    fieldnames[4]: v[2],
                    fieldnames[5]: v[3],
                    fieldnames[6]: v[4],
                    fieldnames[7]: v[4].rstrip('results') + 'params',
                    fieldnames[8]: v[5]}
                   for k, v in enumerate(results)]

        with open(csv_file, 'w') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

            writer.writeheader()

            for i in results:
                writer.writerow(i)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        'Generates a csv containing relevant information about all the '
        'parameter combinations available in "resultsDirectory".')
    parser.add_argument('project_file', help='project file')

    args = parser.parse()
    generateParamsReport(args.project_file)
