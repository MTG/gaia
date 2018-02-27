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
import gaia2
import gaia2.fastyaml as yaml
import environment
from gaia2.classification import GroundTruth
from os.path import join, split


class Collection(object):
    """An mtgdb.Collection instance represents a collection on the disk which file
    structure is the one defined by the MTGDB collections. It allows easy access
    to its properties, such as list of files, groundtruth, etc..."""

    def __init__(self, dirpath, groundTruth = None):
        self.name = split(dirpath)[-1]
        self._basepath = dirpath
        self._config = yaml.loadfile(self.configFilePath())

        self._files = {}
        for type, props in self._config['audioFormats'].items():
            self._files[type] = self.absolutePathFiles(type, props['filelist'])

        self.loadGroundTruth(groundTruth)

    def __repr__(self):
        return '<Gaia2 Collection located at "%s">' % self._basepath


    def configFilePath(self):
        return join(self._basepath, 'metadata', 'config.yaml')

    def groundTruthFilePath(self, gttype):
        return join(self._basepath, 'metadata', self._config['groundTruth'][gttype])

    def audioDirectory(self, audioFormat = None):
        if audioFormat is None:
            audioFormats = self._config['audioFormats'].keys()
            audioFormat = audioFormats[0]
            if len(audioFormats) > 1:
                print ('WARNING: multiple audio formats, choosing "%s" out of %s' % (audioFormat, audioFormats))

        return join(self._basepath, 'audio', audioFormat)


    def relativePathFiles(self, audioFormat = None):
        if audioFormat is None:
            audioFormats = self._config['audioFormats'].keys()
            audioFormat = audioFormats[0]
            if len(audioFormats) > 1:
                print ('WARNING: only taking audio format: %s out of: %s' % (audioFormat, audioFormats))

        filelist = self._config['audioFormats'][audioFormat]['filelist']

        return yaml.loadfile(join(self._basepath, 'metadata', filelist))


    def absolutePathFiles(self, audioFormat, filelist):
        """Returns a map with the given list of audio files with their path expanded."""
        flist = yaml.loadfile(join(self._basepath, 'metadata', filelist))
        expanded = dict((id, join(self.audioDirectory(audioFormat), fpath)) for id, fpath in flist.items())
        return expanded


    def loadGroundTruth(self, name = None):
        gttypes = self._config['groundTruth'].keys()

        if name is None:
            name = gttypes[0]
            if len(gttypes) > 1:
                print ('WARNING: more than 1 GroundTruth file, selecting default "%s" (out of %s)' % (name, gttypes))
        else:
            if name not in gttypes:
                print ('WARNING: invalid ground truth: "%s", selecting default one instead: "%s" (out of %s)' % (name, gttypes[0], gttypes))
                name = gttypes[0]

        self._groundTruthFile = self.groundTruthFilePath(name)
        self.groundTruth = GroundTruth.fromFile(self._groundTruthFile)


    def files(self, type = None):
        if type is None:
            if len(self._files) == 1:
                return self._files.values()[0]
            else:
                raise ValueError('You need to specify which type of filelist you want. Available are: %s', self._files.keys())

        return self._files[type]

    def size():
        return len(self._files.values()[0])
