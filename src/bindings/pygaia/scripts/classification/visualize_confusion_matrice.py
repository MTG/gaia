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
from gaia2.classification import ConfusionMatrix


def main():
    parser = argparse.ArgumentParser(
        description='Visualize a confusion matrix as HTML.')
    parser.add_argument('results_file', help='the results file')
    parser.add_argument('confusion_matrix_html_file', help='the output HTML file')
    args = parser.parse_args()

    cm = ConfusionMatrix()
    cm.load(args.results_file)
    with open(args.confusion_matrix_html_file, 'w') as f:
        f.write(cm.toHtml())

if __name__ == '__main__':
    main()