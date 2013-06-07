#songlib.py

from gaia2 import *
import os
import subprocess


class SongLibrary:
    def __init__(self, name='songlibrary'):
        self.library = DataSet()
        self.library.setName(name)
        self.saveFile = name+'.db'
        self.loadLib()

    def analyzeLib(self, directory):
        files=os.listdir(directory)
        for song in files:
            input = directory+'\\'+song
            print input
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
        print emotions
        print self.library.point(point.name()).label('emotion_tags')
        self.saveLib()

    def getEmotions(self, point):
        return self.library.point(point.name()).label('emotion_tags')

    def createGroundTruth(self, directory, emotionTag):
        files=os.listdir(directory)
        for song in files:
            input = directory+'\\'+song
            print input
            if(".mp3" in input):
                if(".sig" not in input):
                    outputstr = directory+'\\'+song+".sig"
                    if(os.path.exists(outputstr)==False):
                        subprocess.call([ r'C:\Program Files\Essentia\extractors\streaming_extractor.exe', input, outputstr])
                        p = self.addSong(outputstr)
                        self.setEmotions(p, emotionTag)
        self.saveLib()
        

        



        
