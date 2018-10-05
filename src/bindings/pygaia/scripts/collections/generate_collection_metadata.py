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
import sys, yaml, glob, os
from optparse import OptionParser
from os.path import join, exists, basename
from gaia2.utils import makedir, generateMergeFilelist
from gaia2.classification import GroundTruth


def get_argument_parser():
    parser = OptionParser(usage = "usage: %prog [options] collection_dir class_name")

    parser.add_option('-g', '--groundtruth', dest = 'gttype',
                      help = "where to get the groundtruth from: dir or txt        " +
                      " - dir: will take the first directory level as class value  " +
                      "                                              " +
                      " - txt: will look in the metadata folder for .txt files with the same name " +
                      "as the audio files, and set the content of those as class value",
                      metavar = 'TYPE'
                      )

    return parser



def generate_std_metadata(basedir, gtname, options):
    audioFormats = {}

    # make sure metadata folder exists
    os.system('mkdir -p "%s"' % join(basedir, 'metadata'))

    # generate a filelist for each audio folder
    filelists = {}
    for format in glob.glob(join(basedir, 'audio', '*')):
        format = basename(format)

        flist = generateMergeFilelist(join(basedir, 'audio', format),
                                      validFile = lambda x: True,
                                      filename2gid = lambda x: x)

        filelists[format] = flist

        filelist = open(join(basedir, 'metadata', '%s_filelist.yaml' % format), 'w')
        yaml.dump(flist, filelist)

        audioFormats[format] = { 'filelist': '%s_filelist.yaml' % format }

    # generate groundtruth, if asked
    groundTruth = {}
    if options.gttype is not None:

        if options.gttype == 'dir':
            # use the last filelist to get the GT, which should be independent of audio format
            # as it relies on points IDs only
            gt = GroundTruth(gtname)
            for pid in flist:
                gt[pid] = pid.split('/')[0]

            gt.save(join(basedir, 'metadata', 'groundtruth.yaml'))
            groundTruth[gtname] = 'groundtruth.yaml'

        elif options.gttype == 'txt':
            gt = GroundTruth(gtname)
            for pid in flist:
                gtfile = join(basedir, 'metadata', format, pid) + '.txt'
                gt[pid] = open(gtfile).read().strip()

            gt.save(join(basedir, 'metadata', 'groundtruth.yaml'))
            groundTruth[gtname] = 'groundtruth.yaml'

        elif options.gttype == 'mdir':
            # look for all the directories which can be paired in a XXX / not_XXX fashion
            # and create a groundtruth file for each of those
            mdirs = set(pid.split('/')[0] for pid in flist)
            mdirs = [ c for c in mdirs if ('not_' + c in mdirs or 'not-' + c in mdirs) ]
            print('Found following possible classes', mdirs)

            for c in mdirs:
                gt = GroundTruth(gtname + '_' + c)
                for pid in flist:
                    cls = pid.split('/')[0]
                    # only keep those files which we are interested in for our specific subclass
                    if not (cls == c or cls == 'not_' + c or cls == 'not-' + c):
                        continue

                    gt[pid] = cls

                gt.save(join(basedir, 'metadata', 'groundtruth_%s.yaml' % c))
                groundTruth['%s_%s' % (gtname, c)] = 'groundtruth_%s.yaml' % c


        else:
            print('WARNING: unknown groundtruth type:', str(options.gttype))
            print('         not generating any groundtruth files...')



    # write the main config file
    config = { 'version': 1.0,
               'audioFormats': audioFormats,
               'groundTruth': groundTruth
               }

    yaml.dump(config, open(join(basedir, 'metadata', 'config.yaml'), 'w'))



if __name__ == '__main__':
    parser = get_argument_parser()
    options, args = parser.parse_args()

    try:
        basedir = args[0]
        gtname = args[1]

        if len(args) != 2:
            raise 'fallthrough'
    except:
        print(parser.print_help())
        sys.exit(1)

    if options.gttype not in [ 'dir', 'txt', 'mdir' ]:
        print('ERROR: You have to specify a groundtruth type which is either "dir", "mdir" or "txt"\n')
        print(parser.print_help())
        sys.exit(1)

    generate_std_metadata(basedir, gtname, options)
