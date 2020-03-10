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

from __future__ import print_function, absolute_import
import sys, os, shutil
from argparse import ArgumentParser
from os.path import basename, splitext, join

from . import generate_classification_project
from . import run_tests
from . import select_best_model
from . import generate_params_report


def train_model(groundtruth_file, filelist_file, project_file, project_dir, results_model_file, seed=None, cluster_mode=False):
    if not os.path.isfile(project_file):
        print("Creating classification project", project_file)

        # /datasets and /results location
        datasets_dir = os.path.join(project_dir, 'datasets')
        results_dir = os.path.join(project_dir, 'results')

        if not os.path.exists(project_dir):
            os.makedirs(project_dir)
        else:
            # remove /datasets and /results in the case old results are there
            if os.path.exists(datasets_dir):
                shutil.rmtree(datasets_dir)
            if os.path.exists(results_dir):
                shutil.rmtree(results_dir)

        # generate classification project
        generate_classification_project.generate_project(
                groundtruth_file, filelist_file, project_file, datasets_dir, results_dir, seed=seed, cluster_mode=cluster_mode)

    else:
        print("Project file", project_file, "has been found. Skipping project generation step.")

    # run tests
    run_tests.runTests(project_file)

    # analyze results and select best model
    select_best_model.selectBestModel(project_file, results_model_file)

    # generate a csv containing statistics for each parameter
    generate_params_report.generateParamsReport(project_file)


if __name__ == '__main__':
    parser = ArgumentParser(
        description="Generates a model trained using descriptor files specified in the groundtruth and filelist.")

    parser.add_argument('groundtruth_file',
                        help='yaml file containing a relation between keys and labels.')
    parser.add_argument('filelist_file',
                        help='yaml file containing a relation between keys and features file paths. '
                             'Feature files should be in yaml (sig) format')
    parser.add_argument('project_file',
                        help="path where the project configuration file will be stored. "
                             "If this file doesn't exist, then a new project file will be made from a template")
    parser.add_argument('project_dir',
                        help='path to a directory where the best performing model will be stored.')
    parser.add_argument('results_model_file')
    parser.add_argument('--seed', '-s', type=float, default=1,
                        help='seed used to generate the random folds. '
                             'Use 0 to use current time (will vary on each trial).')
    parser.add_argument('--cluster_mode', '-cm', action='store_true',
                        help='Open a new python process for each subtask.')

    args = parser.parse_args()

    seed = args.seed
    if args.seed == 0:
        seed = None

    train_model(args.groundtruth_file, args.filelist_file, args.project_file, args.project_dir, args.results_model_file,
                seed=seed, cluster_mode=args.cluster_mode)
