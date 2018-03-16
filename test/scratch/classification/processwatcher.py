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
from builtins import str
from builtins import object
import os, sys, subprocess
import time

class ProcessTimeout(object):
    '''Class that checks that the given process doesn't run for longer than a given number of seconds, kills it otherwise.'''
    def __init__(self, seconds = 5, msg = None):
        self.seconds = seconds
        self.msg = msg
        self._watcher = subprocess.Popen([ 'python', 'processwatcher.py', str(os.getpid()), str(seconds) ])

    def __del__(self):
        print('killing watcher process')
        subprocess.call([ 'kill', '-9', str(self._watcher.pid) ])


if __name__ == '__main__':
    ppid = int(sys.argv[1])
    secs = float(sys.argv[2])

    time.sleep(secs)

    subprocess.call(['kill', '-9', str(ppid) ])
