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

from __future__ import print_function

# try to guess on which computer we're running (with its IP address) to set some env variables
import subprocess
for l in subprocess.Popen('/sbin/ifconfig', stdout = subprocess.PIPE).communicate()[0].split('\n'):
    if 'inet addr' in l:
        ipaddr = l.split('inet addr:')[1].split(' ')[0]
        break

username = subprocess.Popen('whoami', stdout = subprocess.PIPE).communicate()[0].strip()


G_ENV = { 
          # Dmitry 
          ('10.80.25.171', 'dbogdanov'): { 'mtgdb_audio': '/mnt/mtgdb',
                                           'base_dir': '/home/dbogdanov/gaia' },
          }

G_MOUNT_POINTS = { 
                   '10.80.25.171': '/mnt/mtgdb' # Dmitry
                   }


import os
if os.environ.has_key('MTGDB_AUDIO'):
    # read value from environment variable
    MTGDB_AUDIO = os.environ['MTGDB_AUDIO']
else:

    try:
        userid = (ipaddr, username)
        MTGDB_AUDIO = G_ENV[userid]['mtgdb_audio']
        BASE_DIR = G_ENV[userid]['base_dir']

    except:
        BASE_DIR = None
        MTGDB_AUDIO = '/raid/audio' # this seems like a reasonable default

        try:
            MTGDB_AUDIO = G_MOUNT_POINTS[ipaddr]
        except:
            print ('Warning: could not detect on which computer this program is running.')
            print ('IP address:', ipaddr, '- username:', username)
            print ('Please edit the mtgdb/environment.py file to add your data to it to be able to run it.')
