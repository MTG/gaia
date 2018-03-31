#!/usr/bin/env python
# encoding: utf-8

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

from __future__ import with_statement
from gaia2 import RealDescriptor, transform
import gaia2.fastyaml as yaml
from os.path import join, split, exists, basename, splitext
import glob

class GroundTruth(dict):
    def __init__(self, className, *args, **kwargs):
        super(GroundTruth, self).__init__(*args, **kwargs)
        self.className = className

    def copy(self):
        return GroundTruth(self.className, self)

    @staticmethod
    def fromFile(filename):
        result = GroundTruth('')
        result.load(filename)
        return result

    def load(self, yamlfile):
        with open(yamlfile) as f:
            data = yaml.load(f.read())

        try:
            ver = data['version']
            tp = data['type']
        except:
            raise Exception("Groundtruth file should contain at least the 'version' and the 'type' fields")

        if ver != 1.0 and tp != 'singleClass':
            raise Exception("Groundtruth can only load 'singleClass v1.0' files")

        self.className = data['className']
        self.clear()
        self.update(data['groundTruth'])

    def save(self, yamlfile):
        with open(yamlfile, 'w') as f:
            yaml.dump({ 'version': 1.0,
                        'type': 'singleClass',
                        'className': self.className,
                        'groundTruth': dict(self) }, f)

def getGroundTruthFromDirectory(className, basedir,
                                validFile = lambda x: x.endswith('.sig'),
                                filename2gid = lambda x: splitext(basename(x))[0]):
    """Given a hierarchy where the .sig files are contained in directories
    which name is the class they belong to, return a map from filename -> class.

    validFile is a function that returns whether a given file should be considered
    filename2gid is the function that returns a Gaia ID given a sig filename. By default,
    this returns the base filename without the extension.
    """

    if not exists(basedir):
        raise IOError("Directory '%s' doesn't exist" % basedir)

    classes = [ basename(d) for d in glob.glob(join(basedir, '*')) ]

    # get class map: point id -> class
    classMap = GroundTruth(className)

    for cls in classes:
        files = [ f for f in glob.glob(join(basedir, cls, '*')) if validFile(f) ]

        for f in files:
            classMap[filename2gid(f)] = cls

    return classMap


def getGroundTruthFromLabel(ds, labelName):
    """Return a ground truth map (eg: point ID -> class) for all the points in
    the dataset using the specified descriptor as class."""

    groundTruth = GroundTruth(labelName)

    for p in ds.points():
        groundTruth[p.name().decode('utf-8')] = unicode(p.label(labelName))

    return groundTruth

