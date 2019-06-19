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
    csv_file = project_file.rstrip('project') + 'report.tsv'

    if os.path.exists(results_dir):
        classifier_type = None # all types

        cr = ClassificationResults()
        print('Loading all results...')
        cr.readResults(results_dir)

        n_results = len(cr.results)
        results = cr.best(n_results, classifier_type)
        # results is a list of tuples sorted by accuracy with these fields:
        # (accuracy, std, norm_Accuracy, norm_std, results, results_file)

        # use position as index and sort by norm_accuracy
        r = [(v[2], i) for i, v in enumerate(results)]
        r = sorted(r)[::-1]
        _, idx = zip(*r)

        fieldnames = ['accuracy_rank',
                      'accuracy',
                      'accuracy_std',
                      'normalized_accuracy_rank',
                      'normalized_accuracy',
                      'normalized_accuracy_std',
                      'results_file',
                      'param_file',
                      'kernel',
                      'C',
                      'preprocessing',
                      'balance_classes',
                      'type',
                      'classifier',
                      'gamma',
                      'evaluation']

        results = [{fieldnames[0]: k + 1,  # accuracy_rank
                    fieldnames[1]: v[0],  # accuracy
                    fieldnames[2]: v[1],  # std
                    fieldnames[3]: idx.index(k) + 1,   # normalized_accuracy_rank
                    fieldnames[4]: v[2],  # normalized_accuracy
                    fieldnames[5]: v[3],  # normalized_std
                    fieldnames[6]: v[4],  # results_file
                    fieldnames[7]: v[4].rstrip('results') + 'params',  # param_file
                    fieldnames[8]: v[5]['model']['kernel'],  # kernel
                    fieldnames[9]: v[5]['model']['C'],  # C
                    fieldnames[10]: v[5]['model']['preprocessing'],  # preprocessing
                    fieldnames[11]: v[5]['model']['balanceClasses'],  # balance_classes
                    fieldnames[12]: v[5]['model']['type'],  # type
                    fieldnames[13]: v[5]['model']['classifier'],  # classifier
                    fieldnames[14]: v[5]['model']['gamma'],  # gamma
                    fieldnames[15]: v[5]['evaluation']}  # evaluation
                   for k, v in enumerate(results)]

        with open(csv_file, 'w') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames, delimiter='\t')
            writer.writeheader()

            for i in results:
                writer.writerow(i)

    else:
        print('No results found for {} in its "resultsDirectory" ({}).\n'
              'If the project was moved you may want to manually update '
              'this field on the project file.'.format(project_file, results_dir))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Generates a csv containing accuracy information for all the '
                    'parameter combinations found in "resultsDirectory" for a '
                    'given project (.project file).')
    
    parser.add_argument('project_file', help='project file')

    args = parser.parse_args()
    generateParamsReport(args.project_file)
