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

#emotionSVM.py

#trains SVMs based on binary training sets
#and applies them to songs to predict emotions

from builtins import str
from builtins import object
from gaia2 import *
from songlib import *
from classifier_SVM import *

class emotionSVM(object):
    def __init__(self):
        self.happyTraining = DataSet()
        self.happyTraining.load('Happy.db')
        self.sadTraining = DataSet()
        self.sadTraining.load('Sad.db')
        self.angryTraining = DataSet()
        self.angryTraining.load('Angry.db')
        self.peacefulTraining = DataSet()
        self.peacefulTraining.load('Peaceful.db')
        self.happySVM = TransfoChain()
        self.sadSVM = TransfoChain()
        self.angrySVM = TransfoChain()
        self.peacefulSVM = TransfoChain()
        try:
            self.happySVM.load('HappySVM')
        except:
            self.happySVM = train_SVM(self.happyTraining)
            self.happySVM.save('HappySVM')
        try:
            self.sadSVM.load('SadSVM')
        except:
            self.sadSVM = train_SVM(self.sadTraining)
            self.sadSVM.save('SadSVM')
        try:
            self.angrySVM.load('AngrySVM')
        except:
            self.angrySVM = train_SVM(self.angryTraining)
            self.angrySVM.save('AngrySVM')
        try:
            self.peacefulSVM.load('PeacefulSVM')
        except:
            self.peacefulSVM = train_SVM(self.peacefulTraining)
            self.peacefulSVM.save('PeacefulSVM')

    def predictEmotions(self, testPoint):
        emotions = []
        happyPredict = self.happySVM.mapPoint(testPoint)
        sadPredict = self.sadSVM.mapPoint(testPoint)
        angryPredict = self.angrySVM.mapPoint(testPoint)
        peacefulPredict = self.peacefulSVM.mapPoint(testPoint)

        if(happyPredict['emotion_tags']=='Happy'):
            emotions.append('Happy')

        if(sadPredict['emotion_tags']=='Sad'):
            emotions.append('Sad')

        if(angryPredict['emotion_tags']=='Angry'):
            emotions.append('Angry')

        if(peacefulPredict['emotion_tags']=='Peaceful'):
            emotions.append('Peaceful')

        return emotions

    def retrainSVM(self, newPoint, emotion):
        p = newPoint
        emotion = str(emotion)
        if("Happy" in emotion):
            if(self.happyTraining.contains(p.name())):
                self.happyTraining.removePoint(p.name())
            self.happyTraining.addPoint(p)
            self.happyTraining.point(p.name())['emotion_tags']=emotion
            self.happyTraining.save('Happy.db')
            happySVM = train_SVM(self.happyTraining)
            happySVM.save('HappySVM')
        elif("Sad" in emotion):
            if(self.sadTraining.contains(p.name())):
                self.sadTraining.removePoint(p.name())
            self.sadTraining.addPoint(p)
            self.sadTraining.point(p.name())['emotion_tags']=emotion
            self.sadTraining.save('Sad.db')
            sadSVM = train_SVM(self.sadTraining)
            sadSVM.save('SadSVM')
        elif("Angry" in emotion):
            if(self.angryTraining.contains(p.name())):
               self.angryTraining.removePoint(p.name())
            self.angryTraining.addPoint(p)
            self.angryTraining.point(p.name())['emotion_tags']=emotion
            self.angryTraining.save('Angry.db')
            angrySVM = train_SVM(self.angryTraining)
            angrySVM.save('AngrySVM')
        elif("Peaceful" in emotion):
            if(self.peacefulTraining.contains(p.name())):
               self.peacefulTraining.removePoint(p.name())
            self.peacefulTraining.addPoint(p)
            self.peacefulTraining.point(p.name())['emotion_tags']=emotion
            self.peacefulTraining.save('Peaceful.db')
            peacefulSVM = train_SVM(self.peacefulTraining)
            peacefulSVM.save('PeacefulSVM')
        

        
