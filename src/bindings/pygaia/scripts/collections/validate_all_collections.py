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
import os, sys, subprocess, yaml
from os.path import *
from gaia2.mtgdb.environment import MTGDB_AUDIO
import gaia2


if __name__ == '__main__':
    collections_file = join(gaia2.rootdir(), 'mtgdb', 'mtgdb_collections.yaml')

    for db in yaml.load(open(collections_file).read()):
        cmd = [ 'python', join(split(__file__)[0], 'validate_collection.py'), join(MTGDB_AUDIO, db['location']) ]

        subprocess.call(cmd)

        print ('\n')
