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
from gaia2.fusion import transformDataSet, mergeChunk, mergeAll, mergeDirectory
from optparse import OptionParser, OptionGroup
from os.path import isdir, isfile
import sys, gaia2


def createOptionParser():
    parser = OptionParser(usage = 'usage: %prog [options] input_sigfiles output_db\n\n' +
    'where input_sigfiles can be either a yaml file containing the mapping ids -> sigfiles,\n' +
    'or a directory containing them (in which case the ids will be guessed from the filenames)\n\n' +
    'input_sigfiles and output_db can be specified either as keyword options or as positional arguments.')

    parser.add_option("-y", "--yamllist", dest="yamllist",
                      help="the Yaml file containing the list of files to be merged")

    parser.add_option("-d", "--directory", dest="directory",
                      help="the directory containing all the sigfiles to be merged")

    parser.add_option("-o", "--output", dest="outputFile",
                      help="the filename of the output dataset")

    parser.add_option("-c", "--chunksize", dest="chunkSize", default = 30000,
                      help="the size of the chunks to be used for merging")

    parser.add_option("-t", "--transfofile", dest="transfoFile",
                      help="the file containing the original transformations to be applied, in yaml format")

    parser.add_option("-s", "--select", dest="select",
                      help="the descriptors to be included when initally loading the points")

    parser.add_option("-e", "--exclude", dest="exclude",
                      help="the descriptors to be excluded when initally loading the points")

    advancedGroup = OptionGroup(parser, 'Advanced Options',
                                'Note: use these options only if you know what you\'re doing. You\'ve been warned...')

    advancedGroup.add_option("-r", "--transform",
                             action="store_true", dest="transform", default=False,
                             help="apply basic transformations to dataset instead of merging it")

    advancedGroup.add_option("-i", "--input", dest="inputFile",
                             help="the filename of the input dataset. Only valid when transforming datasets")

    parser.add_option_group(advancedGroup)

    return parser


def usage():
    '''gaiafusion --yamllist=mylist.yaml --output=data/itunes.db
    gaiafusion -d essentia_1.0.6/sigfiles -o amazon.db
    gaiafusion --chunksize=10000 ...
    '''
    createOptionParser().print_help()
    sys.exit(1)


def fusion():
    options, args = createOptionParser().parse_args()

    # dispatch the positional args into their named equivalents
    try:
        if options.transform:
            if not options.inputFile:
                options.inputFile, args = args[0], args[1:]

            if not options.outputFile:
                options.outputFile, args = args[0], args[1:]

        else: # no transform, just merge
            if not options.yamllist and not options.directory:
                path, args = args[0], args[1:]
                if isfile(path):
                    options.yamllist = path
                elif isdir(path):
                    options.directory = path
                else:
                    raise IOError('File/dir not found: %s' % path)

            if not options.outputFile:
                options.outputFile, args = args[0], args[1:]

        # if there are any remaining args, then we have a problem...
        if args:
            usage()

    except IndexError:
        usage()

    # TODO: validate parameters

    # dispatch request
    if options.transform:
        transformDataSet(options.inputFile, options.outputFile, options.transfoFile)
    else:
        select, exclude = None, None
        if options.select:
            select = options.select.split(',')
        if options.exclude:
            exclude = options.exclude.split(',')

        if options.yamllist:
            mergeAll(options.yamllist, options.outputFile, int(options.chunkSize), options.transfoFile, select, exclude)
        elif options.directory:
            if not isdir(options.directory):
                print('ERROR: %s is not a valid directory' % options.directory)
                sys.exit(1)

            mergeDirectory(options.directory, options.outputFile, int(options.chunkSize), options.transfoFile, select, exclude)
        else:
            usage()


if __name__ == '__main__':
    fusion()
