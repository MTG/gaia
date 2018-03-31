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

#songlib.py

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
from builtins import object
from gaia2 import *
import os
import subprocess


class SongLibrary(object):
    def __init__(self, name='songlibrary'):
        self.library = DataSet()
        self.library.setName(name)
        self.saveFile = name+'.db'
        self.loadLib()

    def analyzeLib(self, directory):
        files=os.listdir(directory)
        for song in files:
            input = directory+'\\'+song
            print(input)
            if(".mp3" in input):
                if(".sig" not in input):
                    outputstr = directory+'\\'+song+".sig"
                    if(os.path.exists(outputstr)==False):
                        subprocess.call([ r'C:\Program Files\Essentia\extractors\streaming_extractor.exe', input, outputstr])
                        self.addSong(outputstr)
        self.saveLib()

    def addSong(self, filename):
        #adds song data as a data point, returns the added point
        file = open(filename)
        p = Point()
        p.loadFromString(file.read())
        title = p.label('title')
        p.setName(title.toString())
        if(not p in self.library):
            p.layout().add('emotion_tags', StringType, VariableLength)
            self.library.addPoint(p)
        self.saveLib()
        return p

    def saveLib(self):
        self.library.save(self.saveFile)

    def loadLib(self):
        try:
            self.library.load(self.saveFile)
        except:
            pass

    def setEmotions(self, point, emotions):
        self.library.point(point.name()).setLabel('emotion_tags', emotions)
        print(emotions)
        print(self.library.point(point.name()).label('emotion_tags'))
        self.saveLib()

    def getEmotions(self, point):
        return self.library.point(point.name()).label('emotion_tags')

    def createGroundTruth(self, directory, emotionTag):
        files=os.listdir(directory)
        for song in files:
            input = directory+'\\'+song
            print(input)
            if(".mp3" in input):
                if(".sig" not in input):
                    outputstr = directory+'\\'+song+".sig"
                    if(os.path.exists(outputstr)==False):
                        subprocess.call([ r'C:\Program Files\Essentia\extractors\streaming_extractor.exe', input, outputstr])
                        p = self.addSong(outputstr)
                        self.setEmotions(p, emotionTag)
        self.saveLib()
