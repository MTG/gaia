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
from argparse import ArgumentParser
from gaia2 import filedir
import gaia2.fastyaml as yaml


# Use a dict to map essentia versions to the appropriate template so that we
# don't need to store one template for each version.
VERSION_MAP = {
    '2.1-beta5': '2.1-beta5',
    '2.1-beta6': '2.1-beta5',
}


def generateProject(groundtruth_file, filelist_file, project_file, datasets_dir,
                    results_dir, seed=None, cluster_mode=False, template=None):

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

    print('Analizing the dataset to figure out which project template file to use...')

    essentia_version = ''
    if not template:
        try:
            versions = set([yaml.load(open(v))['metadata']['version']['essentia'] for v in fl.values()])

            if (len(versions) == 1):
                parsed_version = list(versions)[0].split('-')

                essentia_version = parsed_version[0]

                if parsed_version[1].startswith('beta'):
                    essentia_version += '-{}'.format(parsed_version[1])
            else:
                print('More than one essentia versions found in the datataset.\n'
                      'Using the default project template.')
        except IOError:
            print('Error retrieving essentia version')

    template_version = VERSION_MAP.get(essentia_version, 'default')

    print('Using classification project template "{}"'.format(template_version))
    template = 'classification_project_template_{}.yaml'.format(template_version)

    project_template = open(join(filedir(), template)).read()

    # If not seed specified, get the current clock value
    if seed is None:
        import time
        seed = time.time()

    # write the project file
    with open(project_file, 'w') as pfile:
        pfile.write(project_template % { 'className': className,
                                         'datasetsDirectory': abspath(datasets_dir),
                                         'resultsDirectory': abspath(results_dir),
                                         'filelist': abspath(filelist_file),
                                         'groundtruth': abspath(groundtruth_file),
                                         'seed': seed,
                                         'clusterMode': cluster_mode})

    print('Successfully written', project_file)


if __name__ == '__main__':
    parser = ArgumentParser()

    parser.add_argument('groundtruth_file')
    parser.add_argument('filelist_file')
    parser.add_argument('project_file')
    parser.add_argument('datasets_dir')
    parser.add_argument('results_dir')
    parser.add_argument('-s', '--seed', default=None)
    parser.add_argument('-c', '--cluster_mode', action='store_true')
    parser.add_argument('-t', '--template', default=None)

    args = parser.parse_args()

    groundtruth_file = args.groundtruth_file
    filelist_file = args.filelist_file
    project_file = args.project_file
    datasets_dir = args.datasets_dir
    results_dir = args.results_dir
    seed = args.seed
    cluster_mode = args.cluster_mode
    template = args.template

    generateProject(groundtruth_file, filelist_file, project_file, datasets_dir,
                    results_dir, seed=seed, cluster_mode=cluster_mode, template=template)
