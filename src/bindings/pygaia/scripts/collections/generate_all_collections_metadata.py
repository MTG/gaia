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



import os, sys, subprocess, yaml
from os.path import *


def usage():
    print '%s mtgdb_audio_mount_point' % sys.argv[0]
    sys.exit(1)

if __name__ == '__main__':
    try:
        mtgdb = sys.argv[1]
    except:
        usage()

    for db in yaml.load(open('mtgdb_collections.yaml').read()):
        cmd = [ 'python', 'generate_collection_metadata.py',
                '-g', db['groundtruthType'], join(mtgdb, db['location']), db['type'] ]

        subprocess.call(cmd)
