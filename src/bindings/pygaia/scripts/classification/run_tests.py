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
from gaia2 import cvar
from gaia2.classification import ClassificationTaskManager
import os, os.path
import sys
import logging
from optparse import OptionParser

debugLevel = logging.INFO

def runTests(project_file):
    # open the yaml file describing the analysis to perform
    try:
        project_file = os.path.abspath(project_file)
    except:
        print ('ERROR: You need to specify a yaml project file...')
        print ('Exiting...')
        sys.exit(1)

    logging.getLogger('gaia2.classification').setLevel(debugLevel)
    if debugLevel == logging.DEBUG:
        cvar.verbose = True
    else:
        cvar.verbose = False

    # move to the project file directory so all paths can be relative
    olddir = os.getcwd()
    try:
        os.chdir(os.path.split(project_file)[0])
    except OSError:
        pass

    test = ClassificationTaskManager(project_file)
    test.run()

    try:
        os.chdir(olddir)
    except OSError:
        pass

if __name__ == '__main__':
    parser = OptionParser(usage = '%prog [options] project_file')

    options, args = parser.parse_args()

    try:
        project_file = args[0]
    except:
        parser.print_help()
        sys.exit(1)

    runTests(project_file)

