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

# <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
# 
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>


##### General options ##############################################################
from __future__ import print_function
from past.builtins import execfile
PYTHONPATH = '/tmp/gaiatemp/usr/local/lib/python2.5/site-packages/'
import sys
sys.path = [ PYTHONPATH ] + sys.path

from mtgdbpath import MtgdbPath

# try to guess on which computer we're running (with its IP address) to set some env variables
import subprocess
for l in subprocess.Popen('/sbin/ifconfig', stdout = subprocess.PIPE).communicate()[0].split('\n'):
    if 'inet addr' in l:
        ipaddr = l.split('inet addr:')[1].split(' ')[0]
        break

username = subprocess.Popen('whoami', stdout = subprocess.PIPE).communicate()[0].strip()


G_ENV = { # Nico
          ('10.80.4.166', 'nwack'): { 'mtgdb_audio': '/iua-data/mtgdb-audio',
                                    'base_dir': '/home/nwack/dev/gaia/test/classification' },
          # griller Nico
          ('10.55.0.230', 'nwack'): { 'mtgdb_audio': '/raid/audio',
                                      'base_dir': '/home/nwack/dev/gaia/test/classification' },
          # griller Marco
          ('10.55.0.230', 'mmarchini'): { 'mtgdb_audio': '/raid/audio',
                                          'base_dir': '/home/mmarchini/gaia/test/classification' },
          # Nico casa
          ('192.168.1.3', 'download'): { 'mtgdb_audio': '/tmp',
                                         'base_dir': '/home/download/dev/gaia/test/classification' }
          }

try:
    userid = (ipaddr, username)
    MTGDB_AUDIO = G_ENV[userid]['mtgdb_audio']
    BASE_DIR = G_ENV[userid]['base_dir']
except:
    print('Error: could not detect on which computer this program is running. Exiting...')
    print('Please edit the config.py file to add your data to it to be able to run it.')
    sys.exit(1)


# make sure we always call the scripts with the first argument being the project config file
try:
    projectFile = sys.argv[1]
    execfile(projectFile)
except:
    print('Error while trying to load the project file:\n')
    raise

commonConfig = dict(list(d.items())[0] for d in orderedCommonConfig)

evalConfig = {}
for key, value in list(orderedEvalConfig.items()):
    evalConfig[key] = dict(list(d.items())[0] for d in value)
