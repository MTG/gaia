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
from builtins import zip
from builtins import range
from builtins import object
import functools
import os, sys, errno
from os.path import join, basename, splitext, abspath

print = functools.partial(print, flush=True)

def toMatrix(vec):
    # TODO: better return a numpy array, maybe?
    result = []
    dim1 = vec[0]
    dim2 = vec[1]
    for i in range(int(dim1)):
        row = []
        for j in range(int(dim2)):
            row.append(vec[i*int(dim2)+j+2])
        result.append(row)
    return result


def tuplify(obj):
    if isinstance(obj, dict):
        return tuplify(list(obj.items()))
    elif isinstance(obj, (list, tuple)):
        return tuple(tuplify(elem) for elem in obj)
    else:
        return obj


def combinations(vrange):
    """From a list of possible values in each position, generate tuples for all possible combinations."""
    if not vrange:
        yield ()
        return

    for first in vrange[0]:
        for subcomb in combinations(vrange[1:]):
            yield (first,) + subcomb

def dictcombinations(d):
    """From a dictionary of key to possible values, generate dictionaries with all possible combinations for the values."""
    keys = tuple(d.keys())
    for values in combinations(list(d.values())):
        yield dict(list(zip(keys, values)))


def makedir(path):
    """Create a directory and all the required parents. Do not raise an exception if the directory already existed.

    This is the equivalent of 'mkdir -p'."""
    try:
        os.makedirs(path)
    except OSError as e:
        if e.errno == errno.EEXIST:
            pass
        else: raise


class TextProgress(object):

    def __init__(self, total, format = '[%(current)d/%(total)d] (%(percent)d%% done)...'):
        self.total = total
        self.format = '\r' + format

    def update(self, current):
        percentage = current*100 // self.total
        print (self.format % { 'current': current, 'total': self.total, 'percent': percentage }, flush=False)
        if current == self.total: 
            print ('')

def generateMergeFilelist(basedir,
                          relative = True,
                          validFile = lambda x: x.endswith('.sig'),
                          filename2gid = lambda x: splitext(basename(x))[0]):
    """Generate a dictionary of IDs to absolute filenames ready to be used for merging a DataSet.
    This will look for all the files inside basedir for which the validFile function is true.
    The IDs will be built by taking the basename of the file and removing its extension."""

    # get the list of all valid files inside this directory
    filenames = []
    for root, dirs, files in os.walk(basedir, followlinks = True):
        filenames += [ join(root, filename) for filename in files if validFile(filename) ]

    if relative:
        l = len(basedir) + 1 # +1 for trailing '/'
        filenames = [ f[l:] for f in filenames ]
    else:
        filenames = [ abspath(f) for f in filenames ]

    # create a dictionary with their extracted IDs as keys
    result = dict((filename2gid(filename), filename) for filename in filenames)

    # make sure we don't have 2 files that didn't accidentally end up with the same ID
    #assert len(result) == len(filenames)
    if len(result) != len(filenames):
        print ('\nERROR: there are some which end up being duplicates when taking their ID:')
        # need to find which are duplicates and print them
        from collections import defaultdict
        d = defaultdict(list)
        for filename in filenames:
            d[filename2gid(filename)] += [ filename ]

        for pid, filenames in list(d.items()):
            if len(filenames) > 1:
                print ('\n%s:' % pid)
                for f in filenames:
                    print (' -', f)

        assert False

    return result


def printHistory(history):
    for t in history.toPython():
        print ('Transformation:', t['Analyzer name'])
        print ('Parameters:', t['Analyzer parameters'])
        print ('')
