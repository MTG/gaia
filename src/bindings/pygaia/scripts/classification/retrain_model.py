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
from gaia2.fastyaml import yaml
from generate_svm_history_from_config import trainSVMHistory


def retrainModel(project_file, params_file, output_file):
    project = yaml.load(open(project_file, 'r'))
    class_name = project['className']

    trainSVMHistory(project_file, params_file, output_file, class_name)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Retrains a model with all the available data given the '
                    'project configuration file and a set of parameters. '
                    'The model is saved in the output file.')

    parser.add_argument('project_file', help='configuration file (.project)')
    parser.add_argument('params_file', help='parameters (.param)')
    parser.add_argument('output_file', help='history file (the model)')

    args = parser.parse_args()
    retrainModel(args.project_file, args.params_file, args.output_file)
