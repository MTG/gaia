#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, errno
from os.path import join, basename, splitext, abspath

def toMatrix(vec):
    # TODO: better return a numpy array, maybe?
    result = []
    dim1 = vec[0]
    dim2 = vec[1]
    for i in xrange(int(dim1)):
        row = []
        for j in xrange(int(dim2)):
            row.append(vec[i*int(dim2)+j+2])
        result.append(row)
    return result


def tuplify(obj):
    if isinstance(obj, dict):
        return tuplify(obj.items())
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
    for values in combinations(d.values()):
        yield dict(zip(keys, values))


def makedir(path):
    """Create a directory and all the required parents. Do not raise an exception if the directory already existed.

    This is the equivalent of 'mkdir -p'."""
    try:
        os.makedirs(path)
    except OSError, e:
        if e.errno == errno.EEXIST:
            pass
        else: raise


class TextProgress:

    def __init__(self, total, format = '[%(current)d/%(total)d] (%(percent)d%% done)...'):
        self.total = total
        self.format = '\r' + format

    def update(self, current):
        percentage = current*100 // self.total
        print self.format % { 'current': current, 'total': self.total, 'percent': percentage },
        if current == self.total: print
        sys.stdout.flush()



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
        print '\nERROR: there are some which end up being duplicates when taking their ID:'
        # need to find which are duplicates and print them
        from collections import defaultdict
        d = defaultdict(list)
        for filename in filenames:
            d[filename2gid(filename)] += [ filename ]

        for pid, filenames in d.items():
            if len(filenames) > 1:
                print '\n%s:' % pid
                for f in filenames:
                    print ' -', f

        assert False

    return result


def printHistory(history):
    for t in history.toPython():
        print 'Transformation:', t['Analyzer name']
        print 'Parameters:', t['Analyzer parameters']
        print
