#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import with_statement
import os, sys
from os.path import join, abspath, splitext, exists
from optparse import OptionParser
import gaia2.fastyaml as yaml
import gaia2.mtgdb
from gaia2.classification import GroundTruth
from gaia2 import filedir


PROJECT_TEMPLATE = open(join(filedir(), 'classification_project_template.yaml')).read()

def validEqloud(f):
    return f.endswith('.sig') and not f.endswith('.neq.sig')

def validNeqloud(f):
    return f.endswith('.neq.sig')


def sigfileListFromCollection(collection, sigfiles_dir):
    # generate the filelist for the .sig files, as what we have in the metadata/ folder is
    # the filelist for the audio files
    # NB: we only take the first audio format available here (still true?)
    audioFiles = collection.relativePathFiles()

    # NB: we only take the files which are present in the groundtruth, as the others are
    #     useless to us (and probably amount to noise, even if we were able to deal with them)

    sigfileList = dict((pid, abspath(join(sigfiles_dir, f) + '.sig'))
                       for pid, f in audioFiles.items()
                       if pid in collection.groundTruth)

    return sigfileList




def generateProjectFromCollection():
    parser = OptionParser(usage = '%prog [options] collection_name sigfiles_dir project_file\n\n' +
                          'this will also generate a groundtruth and a filelist file to be used by the project file.')

    parser.add_option('-g', '--groundtruth', dest = 'desiredGroundTruth',
                      help = 'Which type of ground truth to use, in case the collection has more than one')

    options, args = parser.parse_args()

    try:
        collection_name = args[0]
        sigfiles_dir = args[1]
        project_file = args[2]
    except:
        parser.print_help()
        sys.exit(1)

    if os.path.isdir(collection_name):
        collec = gaia2.mtgdb.Collection(collection_name, groundTruth = options.desiredGroundTruth)
    else:
        collec = gaia2.mtgdb.MtgdbCollection(collection_name, groundTruth = options.desiredGroundTruth)

    # write yaml file of sigfiles to merge for this project
    filelistFilename = abspath(splitext(project_file)[0] + '.filelist.yaml')
    sigfileList = sigfileListFromCollection(collec, sigfiles_dir)

    with open(filelistFilename, 'w') as filelist:
        yaml.dump(sigfileList, filelist)

    # write the project file
    with open(project_file, 'w') as pfile:
        pfile.write(PROJECT_TEMPLATE % { 'className': collec.groundTruth.className,
                                         'filelist': filelistFilename,
                                         'groundtruth': abspath(collec._groundTruthFile) })

    print 'Successfully written', project_file





if __name__ == '__main__':
    generateProjectFromCollection()

