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



import gaia2
from gaia2 import DataSet, transform, applyTransfoChain, fastyaml
import os, sys, tempfile, subprocess
from os.path import join, dirname

def transformDataSet(inputFilename, outputFilename, transfoFile = None):
    """Apply the list of transformations given as a yaml sequence to the specified dataset."""
    print 'Preprocessing dataset chunk for %s...' % outputFilename
    gaia2.cvar.verbose = False

    transfoList = '''
    - transfo: removevl
    - transfo: fixlength
    - transfo: cleaner
    '''

    if transfoFile is not None:
        transfoList = open(transfoFile).read()

    ds = DataSet()
    ds.load(inputFilename)

    ds = applyTransfoChain(ds, transfoList)

    ds.save(outputFilename)


def horizontalLine():
    print '\n' + '-'*80 + '\n'


def mergeChunk(points, outputFilename, transfoFile, start = 0, end = 1000000, select = None, exclude = None):
    """This function merges the given points (file containing the points, + optional start/end index).
    The history to be applied is given as a list of tuples (transfoName,  transfoParams).
    It also checks the the history for this DataSet corresponds to the original one (if given)."""

    # merge those points using an external process launching gaiamerge
    # NB: this uses child processes to make sure that even if we have a memleak, it won't have such
    #     a big effect when merging millions of files
    cmd = [ 'gaiamerge', points, outputFilename + '.raw', str(start), str(end) ]
    if select:
        if isinstance(select, basestring):
            select = [ select ]
        cmd += [ '--select=' + ','.join(select) ]
    if exclude:
        if isinstance(exclude, basestring):
            exclude = [ exclude ]
        cmd += [ '--exclude=' + ','.join(exclude) ]

    subprocess.call(cmd)

    # transform this dataset, also with an external process
    cmd = [ 'gaiafusion', '--transform', '-i', outputFilename + '.raw', '-o', outputFilename ]
    if transfoFile:
        cmd += [ '-t', transfoFile ]

    subprocess.call(cmd)


def harmonizeChunks(partfiles):
    # TODO: check all histories are the same, if not, try to do sth about it
    # find the GCLD (greatest common layout divisor :-) )
    ds = DataSet()
    ds.load(partfiles[0])
    origLayout = ds.layout().copy()
    gcld = ds.layout().copy()

    for pfile in partfiles[1:]:
        ds.load(pfile)
        gcld = gcld & ds.layout()

    # keep some stats about which descriptors got removed and the reason why before throwing
    # away the original history and simplifying it
    vldescs = set()
    nandescs = set()

    # now that we have our GCLD, transform all the chunks so they have the same layout (our GCLD)
    # and simplify their histories so that they also have the same history (the minimum history
    # required to arrive at this target layout).
    for pfile in partfiles:
        ds.load(pfile)

        for t in ds.history().toPython():
            tname = t['Analyzer name']
            descs = t['Applier parameters']['descriptorNames']
            if   tname == 'cleaner':  nandescs.update(descs)
            elif tname == 'removevl': vldescs.update(descs)

        toremove = ds.layout().differenceWith(gcld)
        if toremove:
            ds = transform(ds, 'remove', { 'descriptorNames': toremove })

        ds.simplifyHistory()
        ds.save(pfile)

    # also get the other descriptors that got removed (because of a select or remove transfo)
    rdescs = set(origLayout.differenceWith(gcld)) - (vldescs | nandescs)

    return vldescs, nandescs, rdescs


def mergeAll(pointList, outputFilename, chunkSize, transfoFile, select = None, exclude = None):
    # TODO: validation of the yaml file format? (ie: pre-2.3 yaml files should be rejected)
    totalPoints = len(fastyaml.load(open(pointList).read()))

    begin, end = 0, chunkSize
    partfiles = []
    partfileTemplate = outputFilename + '_%d_%d.partdb'

    # keep this information for future reference as it won't be accessible anymore
    # once the dataset is merged
    excluded = []
    if exclude:
        try:
            p = gaia2.Point()
            p.load(gaia2.fastyaml.loadfile(pointList).items()[0][1])
            excluded = p.layout().descriptorNames(exclude)
        except Exception as e:
            print(e)

    # merge each chunk separately
    # this includes removevl and fixlength, which should yield smaller files than just after
    # merging, so it should then be possible to load all of them together to merge them
    while begin < totalPoints:
        end = min(end, totalPoints)
        partfile = partfileTemplate % (begin, end)
        partfiles += [ partfile ]

        mergeChunk(pointList, partfile, transfoFile, begin, end, select, exclude)
        begin, end = end, end + chunkSize

        horizontalLine()

    # make sure all histories are the same, if not do whatever it takes to reach that point
    # also "simplify" the histories so that they are the minimum history representation required
    # to get to the layout of the final dataset
    print 'Harmonizing chunks so that they all have the same layout & history...'
    vldescs, nandescs, rdescs = harmonizeChunks(partfiles)
    rdescs = rdescs | set(excluded)
    horizontalLine()

    # merge all those partfiles together
    print 'Assembling full dataset together...'
    dstotal = DataSet()

    for pfile in partfiles:
        print 'Merging partfile', pfile
        ds = DataSet()
        ds.load(pfile)
        dstotal.appendDataSet(ds)

    dstotal.save(outputFilename)

    # print a nice informative summary of what has been done to the dataset
    horizontalLine()

    msg = '''
Final dataset information
-------------------------

Number of points: %s

Descriptors removed:
  - because they were of variable length: %s
  - because they were either constant, contained NaN or contained Inf: %s
  - because they were removed explicitly: %s

Your dataset has been saved at %s'''

    # remove leading dot
    vldescs = sorted( d[1:] for d in vldescs )
    nandescs = sorted( d[1:] for d in nandescs )
    rdescs = sorted( d[1:] for d in rdescs )

    print msg % (str(dstotal.size()), ', '.join(vldescs), ', '.join(nandescs), ', '.join(rdescs), outputFilename)

    # clean up temporary files
    for pfile in partfiles:
        os.remove(pfile)
        os.remove(pfile + '.raw')



def mergeDirectory(dirname, outputFilename, chunkSize, transfoFile, select = None, exclude = None):
    idIsFullPath = False
    ext = '.sig'
    # TODO: this should be more flexible
    validFile = lambda x: x.endswith('.sig') and not x.endswith('.neq.sig')

    # find the list of all the points that should go into the dataset
    plist = {}
    for root, dirs, files in os.walk(dirname):
        for filename in filter(validFile, files):
            fullpath = os.path.join(root, filename)
            pid = filename
            if idIsFullPath:
                pid = fullpath
            # remove extension from the point id
            pid = pid[:-len(ext)]
            plist[pid] = fullpath

    # write a temporary yaml filelist (should delete itself upon closing)
    import tempfile
    yamllist = tempfile.NamedTemporaryFile(mode = 'w+')
    fastyaml.dump(plist, yamllist)
    yamllist.flush()

    # call 'classic' merge function
    mergeAll(yamllist.name, outputFilename, chunkSize, transfoFile, select, exclude)


