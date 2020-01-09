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
from __future__ import with_statement
import sys
from os.path import join, abspath
from optparse import OptionParser
from gaia2 import filedir
import gaia2.fastyaml as yaml

PROJECT_TEMPLATE = open(join(filedir(), 'classification_project_template.yaml')).read()

def generateProject(groundtruth_file, filelist_file, project_file, datasets_dir, results_dir):
    gt = yaml.load(open(groundtruth_file, 'r'))
    try:
        className = gt['className']
        groundTruth = gt['groundTruth']
    except:
        print(groundtruth_file, "groundtruth file has incorrect format")
        sys.exit(2)

    fl = yaml.load(open(filelist_file, 'r'))

    gt_trackids = list(groundTruth.keys())
    fl_trackids = list(fl.keys())

    # check that there are no dublicate ids
    if len(gt_trackids) != len(set(gt_trackids)):
        print(groundtruth_file, "contains dublicate track ids")
        sys.exit(3)

    if len(fl_trackids) != len(set(fl_trackids)):
        print(filelist_file, "contains dublicate track ids")
        sys.exit(3)

    # check if filelist is consistent with groundtruth (no files missing)
    if set(gt_trackids) != set(fl_trackids):
        print("track ids found in", groundtruth_file, "are inconsistent with", filelist_file)
        sys.exit(4)

    # write the project file
    with open(project_file, 'w') as pfile:
        pfile.write(PROJECT_TEMPLATE % { 'className': className,
                                         'datasetsDirectory': abspath(datasets_dir),
                                         'resultsDirectory': abspath(results_dir),
                                         'filelist': abspath(filelist_file),
                                         'groundtruth': abspath(groundtruth_file) })

    print('Successfully written', project_file)


if __name__ == '__main__':
    parser = OptionParser(usage = '%prog [options] groundtruth_file filelist_file project_file datasets_dir results_dir')

    options, args = parser.parse_args()

    try:
        groundtruth_file = args[0]
        filelist_file = args[1]
        project_file = args[2]
        datasets_dir = args[3]
        results_dir = args[4]
    except:
        parser.print_help()
        sys.exit(1)

    generateProject(groundtruth_file, filelist_file, project_file, datasets_dir, results_dir)

