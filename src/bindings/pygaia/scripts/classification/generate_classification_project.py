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


# We have separate template files because different versions of the
# music extractor require different layouts.
# We use a dict to aggregate the music extractor versions relying on the
# same layout so we don't need to store one template for each version.
VERSION_MAP = {
    '2.1-beta5': '2.1-beta5',
    '2.1-beta6': '2.1-beta5',
}

DEFAULT_VERSION = '2.1-beta2'


def get_essentia_versions(filelist):
    versions = set()

    for v in filelist.values():
        try:
            version = yaml.load(open(v)).get('metadata', {}).get('version', {}).get('essentia', {})
            if version:
                parsed_version = version.split('-')
                essentia_version = parsed_version[0]
                if parsed_version[1].startswith('beta'):
                    essentia_version += '-{}'.format(parsed_version[1])

                versions.add(essentia_version)
            else:
                versions.add('no_essentia_version_field')

        except IOError:
            print('Error retrieving the Essentia version of {}'.format(v))
    return versions

def generate_project(groundtruth_file, filelist_file, project_file, datasets_dir,
                    results_dir, seed=None, cluster_mode=False, template=None,
                    force_consistency=False):

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

    # check that there are no duplicated ids
    if len(gt_trackids) != len(set(gt_trackids)):
        print(groundtruth_file, "contains duplicated track ids")
        sys.exit(3)

    if len(fl_trackids) != len(set(fl_trackids)):
        print(filelist_file, "contains duplicated track ids")
        sys.exit(3)

    # check if filelist is consistent with groundtruth (no files missing)
    if set(gt_trackids) != set(fl_trackids):
        print("track ids found in", groundtruth_file, "are inconsistent with", filelist_file)
        sys.exit(4)

    if force_consistency:
        print('Checking Essentia version in the descriptor files to ensure consistency...')
        versions = get_essentia_versions(fl)

        if len(versions) > 1:
            raise Exception("Couldn't find a unique Essentia version in the dataset. "
                            "This exception is thrown because you are using the flag `force-consistency`")
        print('ok!')

    if not template:
        print('No classification project template specified.')
        essentia_version = DEFAULT_VERSION

        if not force_consistency:
            print('Analyzing the dataset to figure out which project template file to use...')
            versions = get_essentia_versions(fl)

        if len(versions) == 1:
            essentia_version = list(versions)[0]
        else:
            print("Couldn't find a unique essentia version in the dataset.")

        template_version = VERSION_MAP.get(essentia_version, DEFAULT_VERSION)

        print('Using classification project template "{}"'.format(template_version))
        template = 'classification_project_template_{}.yaml'.format(template_version)

    project_template = open(join(filedir(), template)).read()

    # if not seed specified, get the current clock value
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
    parser = ArgumentParser(description='Generates a project configuration file given a filelist, a groundtruth file, '
                                        'and the directories to store the datasets and the results files. '
                                        'The script has a parameter to specify the project template to use. '
                                        'If it is not specified, it will try to guess the appropriated one from the '
                                        'essentia version found on the descriptor files.')

    parser.add_argument('groundtruth_file', help='yaml file containing a relation between keys and labels.')
    parser.add_argument('filelist_file', help='yaml file containing a relation between keys and features file paths. '
                                              'Feature files should be in yaml (sig) format.')
    parser.add_argument('project_file', help='path where the project configuration file (.project) will be stored.')
    parser.add_argument('datasets_dir', help='path where the dataset files will be stored.')
    parser.add_argument('results_dir', help='path where the result files will be stored.')
    parser.add_argument('-s', '--seed', default=None, help='seed used to generate the random folds. '
                                                           'Use 0 to use current time (will vary on each trial).')
    parser.add_argument('-c', '--cluster_mode', action='store_true', help='Open a new python process for each subtask.')
    parser.add_argument('-t', '--template', default=None, help='classification project template file to use. '
                                                               'If not specified, the script will try to detect it from the descriptors metadata.')
    parser.add_argument('-f', '--force-consistency', action='store_true', help='Checks if all the descriptor files were computed with the same Essentia version. '
                                                                                'Throws an exception if not.')

    args = parser.parse_args()

    generate_project(args.groundtruth_file, args.filelist_file, args.project_file, args.datasets_dir,
                    args.results_dir, seed=args.seed, cluster_mode=args.cluster_mode, template=args.template,
                    force_consistency=args.force_consistency)
