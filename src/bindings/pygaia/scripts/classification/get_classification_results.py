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


from __future__ import print_function
import argparse
import gaia2.fastyaml as yaml

from gaia2.scripts.classification import classification_results

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Show the best result parameters of a training session')
    parser.add_argument('results_dir', help='The location that the results are stored in')
    parser.add_argument('classifier_type', nargs='?', help='If set, only show results from this classifier type')

    args = parser.parse_args()

    cr = classification_results.ClassificationResults()
    print('Loading all results...')
    cr.read_results(args.results_dir)

    print('Best parameters:')
    for accuracy, acc_std, norm_accuracy, norm_std, filename, params in cr.best(10, args.classifier_type):
        print('*' * 70)
        print('Correct classification: %2f%%' % accuracy)
        print('Filename:', filename)

        model = params['model']
        print('Classifier:', model['classifier'])
        print('Parameters:')
        del model['classifier']
        print('    ' + yaml.dump(model).replace('\n', '\n    ')[:-4])
