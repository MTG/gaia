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

import argparse
import collections
import csv
import os

from gaia2.fastyaml import yaml

from get_classification_results import ClassificationResults


def generate_params_report(project_file, result_file):
    project = yaml.load(open(project_file, 'r'))
    project_file_dir = os.path.dirname(project_file)
    results_dir = project['resultsDirectory']

    tsv_file = os.path.join(project_file_dir, result_file)

    if os.path.exists(results_dir):
        classifier_type = None # all types

        cr = ClassificationResults()
        print('Loading all results...')
        cr.readResults(results_dir)

        n_results = len(cr.results)
        results = cr.best(n_results, classifier_type)
        # results is a list of tuples sorted by accuracy with these fields:
        # (accuracy, std, norm_Accuracy, norm_std, results_file results)

        # use position as index and sort by norm_accuracy
        r = [(v[2], i) for i, v in enumerate(results, 1)]
        r = sorted(r, reverse=True)
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

        results = [{'accuracy_rank': k,
                    'accuracy': accuracy,
                    'accuracy_std': acc_std,
                    'normalized_accuracy_rank': idx.index(k) + 1,
                    'normalized_accuracy': norm_accuracy,
                    'normalized_accuracy_std': norm_std,
                    'results_file': results_file,
                    'param_file': results_file.rstrip('results') + 'params',
                    'kernel': results_param['model']['kernel'],
                    'C': results_param['model']['C'],
                    'preprocessing': results_param['model']['preprocessing'],
                    'balance_classes': results_param['model']['balanceClasses'],
                    'type': results_param['model']['type'],
                    'classifier': results_param['model']['classifier'],
                    'gamma': results_param['model']['gamma'],
                    'evaluation': results_param['evaluation']}
                   for k, (accuracy, acc_std, norm_accuracy, norm_std, results_file, results_param) in enumerate(results, 1)]

        with open(tsv_file, 'w') as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames, delimiter='\t')
            writer.writeheader()

            writer.writerows(results)

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
    parser.add_argument('result_file', default='report.tsv', help='name of file to write results to')

    args = parser.parse_args()
    generate_params_report(args.project_file, args.result_file)
