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

import sys, os, shutil
from optparse import OptionParser
from os.path import basename, splitext, join

import json_to_sig
import generate_classification_project
import run_tests
import select_best_model

def trainModel(groundtruth_file, filelist_file, project_file, project_dir, results_model_file, seed=None):
    if not os.path.isfile(project_file):
        print "Creating classification project", project_file

        # /datasets and /results location
        datasets_dir = join(project_dir, 'datasets')
        results_dir = join(project_dir, 'results')

        if not os.path.exists(project_dir):
            os.makedirs(project_dir)
        else:
            # remove /datasets and /results in the case old results are there
            if os.path.exists(datasets_dir):
                shutil.rmtree(datasets_dir)
            if os.path.exists(results_dir):
                shutil.rmtree(results_dir)

        ## convert json to sig
        # temporary filelist location
        #filelist_file_sig = splitext(basename(filelist_file))[0] + '.sig.yaml'
        #filelist_file_sig = join(project_dir, filelist_file_sig)

        ## do not allow any missing sig files
        #if not json_to_sig.convertJsonToSig(filelist_file, filelist_file_sig):
        #    print "Error: some descriptor files are missing; training failed."
        #    sys.exit(2)

        # generate classification project
        generate_classification_project.generateProject(
                groundtruth_file, filelist_file, project_file, datasets_dir, results_dir, seed=seed)

    else:
        print "Project file", project_file, "has been found. Skipping project generation step."

    # run tests
    run_tests.runTests(project_file)

    # analyze results and select best model
    select_best_model.selectBestModel(project_file, results_model_file)


if __name__ == '__main__':
    parser = OptionParser(usage = '%prog [options] groundtruth_file filelist_file project_file project_dir results_model_file\n' +
"""
Project generation and related data preprocessing will be skipped if 'project_file'
already exists. Specify a non-existent 'project_file' or remove it if you want to
recreate the project. The filelist is expected to have "*.sig" files (yaml format)
"""
                         )

    options, args = parser.parse_args()

    try:
        groundtruth_file = args[0]
        filelist_file = args[1]
        project_file = args[2]
        project_dir = args[3]
        results_model_file = args[4]
    except:
        parser.print_help()
        sys.exit(1)

    trainModel(groundtruth_file, filelist_file, project_file, project_dir, results_model_file)
