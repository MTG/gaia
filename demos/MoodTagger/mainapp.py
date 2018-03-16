from __future__ import print_function
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

# File: mainapp.py
#Runs the main application
from future import standard_library
standard_library.install_aliases()
from builtins import filter
from builtins import str
from builtins import range
from builtins import object
from tkinter import *
from tkentrycomplete import *
import tkinter.filedialog
import win32com.client
from nowplayingfeatures import *
from winampnew import *
from player import *
from songlib import *
from gaia2 import *
from emotionSVM import *



class App(object):

    def __init__(self, master):
        self.songLibrary = SongLibrary() #user's song library (should be 'songlibrary.db' by default
        self.emo = emotionSVM() #collection of SVM classifiers to predict the presence of the emotions Happy, Sad, Angry, and Peaceful
        #self.userTags = DataSet() #collection of past user tags (not needed yet)
        #self.userTagList = self.userTags.pointNames()

        self.label = Label(master,text="Hello! Please select tags for the following song.\n If you would like to change the music player you use, please us the Music Player menu.\n").grid(row=0)

        self.player = Player('iTunes') #interface to the current player (iTunes or winamp)
        
        self.title = Label(master, text="Title: "+self.player.getCurrentTitle())
        self.title.grid(row=1)
        self.artist = Label(master, text="Artist: "+self.player.getCurrentArtist())
        self.artist.grid(row=2)

        self.tags=[] #tags the user has already selected
        self.predictTags=[] #tags the system predicts

        if(self.songLibrary.library.contains(self.player.getCurrentTitle().encode('ascii', 'ignore'))): #if the current song is in the library already
            currtags = str(self.songLibrary.getEmotions(self.songLibrary.library.point((self.player.getCurrentTitle().encode('ascii', 'ignore')))))
            self.tags=currtags.split(',') #retrieve the listed emotions(if any)
            #then predict which emotions are there
            self.predictTags = self.emo.predictEmotions(self.songLibrary.library.point((self.player.getCurrentTitle().encode('ascii', 'ignore'))))
        print("Place: __init__:")
        print("Current Tags: ", self.tags)
        print("Predicted Tags: ", self.predictTags)
        self.happyVar = StringVar() #variable for the "Happy" check box
        if("Happy" in self.tags): #if the user has already tagged the song as Peaceful
            self.happyVar.set("Happy")
            self.happycolor = 'grey'
        else:
            if("Happy" in self.predictTags): #if the system predicts the song to be peaceful
                self.happyVar.set("Happy")
                self.happycolor = 'red'
            else:
                self.happyVar.set("not")
                self.happycolor = 'grey'
        self.happyCheck = Checkbutton(master, text="Happy", variable=self.happyVar, onvalue="Happy", offvalue="not", bg=self.happycolor)
        self.happyCheck.grid(row=3, sticky=W)


        self.sadVar = StringVar() #variable for the "Sad" checkbox
        if("Sad" in self.tags): #if the user has already tagged the song as happy
            self.sadVar.set("Sad")
            self.sadcolor = 'grey'
        else:
            if("Sad" in self.predictTags): #if the system predicts the song to be happy (and the user hasn't tagged it that way already)
                self.sadVar.set("Sad")
                self.sadcolor = 'red'
            else:
                self.sadVar.set("not")
                self.sadcolor = 'grey'
        self.sadCheck = Checkbutton(master, text="Sad", variable=self.sadVar, onvalue="Sad", offvalue="not", bg=self.sadcolor)
        self.sadCheck.grid(row=3, sticky=W, padx=100)


        self.angryVar = StringVar() #variable for the "Angry" check box
        if("Angry" in self.tags): #if the user has already tagged the song as angry
            self.angryVar.set("Angry")
            self.angrycolor = 'grey'
        else:
            if("Angry" in self.predictTags): #if the system predicts the song to be angry
                self.angryVar.set("Angry")
                self.angrycolor = 'red'
            else:
                self.angryVar.set("not")
                self.angrycolor = 'grey'
        self.angryCheck = Checkbutton(master, text="Angry", variable=self.angryVar, onvalue="Angry", offvalue="not", bg=self.angrycolor)
        self.angryCheck.grid(row=4, sticky=W)
 

        self.peacefulVar = StringVar() #variable for the "Peaceful" check box
        if("Peaceful" in self.tags): #if the user has already tagged the song as Peaceful
            self.peacefulVar.set("Peaceful")
            self.peacefulcolor = 'grey'
        else:
            if("Peaceful" in self.predictTags): #if the system predicts the song to be peaceful
                self.peacefulVar.set("Peaceful")
                self.peacefulcolor = 'red'
            else:
                self.peacefulVar.set("not")
                self.peacefulcolor = 'grey'
        self.peacefulCheck = Checkbutton(master, text="Peaceful", variable=self.peacefulVar, onvalue="Peaceful", offvalue="not", bg=self.peacefulcolor)
        self.peacefulCheck.grid(row=4, sticky=W, padx=100)

        self.openInstruction = Label(master, text="You may also enter your own tag").grid(row=5, sticky=W)

        self.openTagVar = StringVar()
        self.openTagEntry = Entry(master, textvariable=self.openTagVar, text="").grid(row=5, sticky=W, padx=200)

        self.tags=[self.happyVar, self.sadVar, self.angryVar, self.peacefulVar]
        

        self.button = Button(master, text="Submit", command=self.submit).grid(row=6, sticky=W)
        self.quit = Button(master, text="Quit", command=root.destroy).grid(row=6, sticky=E)
        directory = tkinter.filedialog.askdirectory()
        self.songLibrary.analyzeLib(directory)

        self.refresher=Label(master)
        self.refresher.after(1000, self.refreshWindow)


    def isTag(self, tag):
        if(tag.get()=="not"):
            return False
        else:
            return True


    def submit(self):
        redTags = list(filter(self.isTag, self.tags)) #the tags taken from the GUI (filtered by whether there is a "not" in the tag)
        tagsToSend = redTags[0].get() #gets the first tag (this is done so that the correct number of commas are submitted in the next loop
        for i in range(1, len(redTags)):
            tagsToSend=tagsToSend+","+redTags[i].get() #go through the rest of the tags and add them on the the string
        #Routine to go through the text box and add written tags to the sent tags
      #  if(self.openTagVar.get()!=""):
       #     tagsToSend=tagsToSend+","+self.openTagVar.get()
        #    userTags = self.openTagVar.get().split(",")
         #   for tag in userTags:
          #      p = Point()
           #     p.loadFromString(tag)
            #    p.setName(tag)
             #   if not self.userTags.contains(p):
              #      self.userTags.addPoint(p)
               #     self.userTags.save("usertags.db")
        print("Tags to send: ", tagsToSend)       
        point = self.songLibrary.library.point(self.player.getCurrentTitle().encode('ascii', 'ignore'))
        self.songLibrary.setEmotions(point, tagsToSend)
        print("Place: Submit")
        print(self.songLibrary.getEmotions(point))
        for tag in redTags:
            rtag = str(tag.get())
            #tag the user checks that the system misses
            if(not rtag in self.predictTags):
                print("Retraining: ", rtag)
                self.emo.retrainSVM(point, tag.get())
        for pTag in self.predictTags:
            #tag the system selects that the user removes
            if(not pTag in tagsToSend):
                newTag = "not "+pTag
                print("Retraining: ", newTag)
                self.emo.retrainSVM(point, newTag)
                

    def refreshMenu(self, type):
        self.player.changeType(type)
        self.title.config(text="Title: "+self.player.getCurrentTitle())
        self.artist.config(text="Artist: "+self.player.getCurrentArtist())
        title = self.player.getCurrentTitle().encode('ascii', 'ignore')
        if(self.songLibrary.library.contains(title)):
            currtags = str(self.songLibrary.getEmotions(self.songLibrary.library.point(title)))
            print(currtags)
            tags=currtags.split(',')
            self.predictTags = self.emo.predictEmotions(self.songLibrary.library.point(title))
            print("Place: refreshMenu")
            print("Current Tags: ", tags)
            print("Predicted Tags: ", self.predictTags)
            if("Happy" in tags):
                self.happyVar.set("Happy")
                self.happycolor='grey'
            else:
                if("Happy" in self.predictTags):
                    self.happyVar.set("Happy")
                    self.happycolor='red'
                else:
                    self.happyVar.set("not")
                    self.happycolor='grey'
            if("Sad" in tags):
                self.sadVar.set("Sad")
                self.sadcolor='grey'
            else:
                if("Sad" in self.predictTags):
                    self.sadVar.set("Sad")
                    self.sadcolor='red'
                else:
                    self.sadVar.set("not")
                    self.sadcolor='grey'
            if("Angry" in tags):
                self.angryVar.set("Angry")
                self.angrycolor='grey'
            else:
                if("Angry" in self.predictTags):
                    self.angryVar.set("Angry")
                    self.angrycolor='red'
                else:
                    self.angryVar.set("not")
                    self.angrycolor='red'
            if("Peaceful" in tags):
                self.peacefulVar.set("Peaceful")
                self.peacefulcolor='grey'
            else:
                if("Peaceful" in self.predictTags):
                    self.peacefulVar.set("Peaceful")
                    self.peacefulcolor='red'
                else:
                    self.peacefulVar.set("not")
                    self.peacefulcolor='grey'
        else:
            file = self.player.sendCurrentSongToEssentia()
            try:
                self.songLibrary.addSong(file)
                self.predictTags = self.emo.predictEmotions(self.songLibrary.library.point(title))
                print("Place: refreshMenu (song not in library)")
                print("PredictedTags: ", self.predictTags)
                if("Happy" in self.predictTags):
                    self.happyVar.set("Happy")
                    self.happycolor = 'red'
                else:
                    self.happyVar.set("not")
                    self.happycolor='grey'
                if("Sad" in self.predictTags):
                    self.sadVar.set("Sad")
                    self.sadcolor = 'red'
                else:
                    self.sadVar.set("not")
                    self.sadcolor = 'grey'
                if("Angry" in self.predictTags):
                    self.angryVar.set("Angry")
                    self.angrycolor = 'red'
                else:
                    self.angryVar.set("not")
                    self.angrycolor = 'grey'
                if("Peaceful" in self.predictTags):
                    self.peacefulVar.set("Peaceful")
                    self.peacefulcolor='red'
                else:
                    self.peacefulVar.set("not")
                    self.peacefulcolor='grey'
            except:
                    self.happyVar.set("not")
                    self.happycolor='grey'
                    self.sadVar.set("not")
                    self.sadcolor = 'grey'
                    self.angryVar.set("not")
                    self.angrycolor = 'grey'
                    self.peacefulVar.set("not")
                    self.peacefulcolor='grey'
        self.happyCheck.config(bg=self.happycolor)
        self.sadCheck.config(bg=self.sadcolor)
        self.angryCheck.config(bg=self.angrycolor)
        self.peacefulCheck.config(bg=self.peacefulcolor)

    def refreshWindow(self):
        if(self.player.getCurrentPosition()==0):
            self.title.config(text="Title: "+self.player.getCurrentTitle())
            self.artist.config(text="Artist: "+self.player.getCurrentArtist())
            if(self.songLibrary.library.contains(self.player.getCurrentTitle().encode('ascii', 'ignore'))): #if the current song is in the library
                #get the emotions
                currtags = str(self.songLibrary.getEmotions(self.songLibrary.library.point((self.player.getCurrentTitle().encode('ascii', 'ignore')))))
                tags=currtags.split(',')
                #predict emotions
                self.predictTags = self.emo.predictEmotions(self.songLibrary.library.point((self.player.getCurrentTitle().encode('ascii', 'ignore'))))
                print("Place: refreshWindow")
                print("Current Tags: ", tags)
                print("Predicted Tags: ", self.predictTags)
                #change display to reflect the current and predicted tags
                if("Happy" in tags):
                    self.happyVar.set("Happy")
                    self.happycolor='grey'
                else:
                    if("Happy" in self.predictTags):
                        self.happyVar.set("Happy")
                        self.happycolor='red'
                    else:
                        self.happyVar.set("not")
                        self.happycolor='grey'
                if("Sad" in tags):
                    self.sadVar.set("Sad")
                    self.sadcolor='grey'
                else:
                    if("Sad" in self.predictTags):
                        self.sadVar.set("Sad")
                        self.sadcolor='red'
                    else:
                        self.sadVar.set("not")
                        self.sadcolor='grey'
                if("Angry" in tags):
                    self.angryVar.set("Angry")
                    self.angrycolor='grey'
                else:
                    if("Angry" in self.predictTags):
                        self.angryVar.set("Angry")
                        self.angrycolor='red'
                    else:
                        self.angryVar.set("not")
                        self.angrycolor='grey'
                if("Peaceful" in tags):
                    self.peacefulVar.set("Peaceful")
                    self.peacefulcolor='grey'
                else:
                    if("Peaceful" in self.predictTags):
                        self.peacefulVar.set("Peaceful")
                        self.peacefulcolor='red'
                    else:
                        self.peacefulVar.set("not")
                        self.peacefulcolor='grey'
            else:
                file = self.player.sendCurrentSongToEssentia()
                try:
                    self.songLibrary.addSong(file)
                    self.predictTags = self.emo.predictEmotions(self.songLibrary.library.point((self.player.getCurrentTitle().encode('ascii', 'ignore'))))
                    if("Happy" in self.predictTags):
                        self.happyVar.set("Happy")
                        self.happycolor = 'red'
                    else:
                        self.happyVar.set("not")
                        self.happycolor='grey'
                    if("Sad" in self.predictTags):
                        self.sadVar.set("Sad")
                        self.sadcolor = 'red'
                    else:
                        self.sadVar.set("not")
                        self.sadcolor = 'grey'
                    if("Angry" in self.predictTags):
                        self.angryVar.set("Angry")
                        self.angrycolor = 'red'
                    else:
                        self.angryVar.set("not")
                        self.angrycolor = 'grey'
                    if("Peaceful" in self.predictTags):
                        self.peacefulVar.set("Peaceful")
                        self.peacefulcolor='red'
                    else:
                        self.peacefulVar.set("not")
                        self.peacefulcolor='grey'
                except:
                        self.happyVar.set("not")
                        self.happycolor='grey'
                        self.sadVar.set("not")
                        self.sadcolor = 'grey'
                        self.angryVar.set("not")
                        self.angrycolor = 'grey'
                        self.peacefulVar.set("not")
                        self.peacefulcolor='grey'
        self.happyCheck.config(bg=self.happycolor)
        self.sadCheck.config(bg=self.sadcolor)
        self.angryCheck.config(bg=self.angrycolor)
        self.peacefulCheck.config(bg=self.peacefulcolor)
        self.refresher.after(1000, self.refreshWindow)

  
        

root = Tk()
app = App(root)
menu = Menu(root)
root.config(menu=menu)
settingsmenu=Menu(menu)
playermenu = Menu(menu)
settingsmenu.add_cascade(label="Music Player", menu=playermenu)
menu.add_cascade(label="Settings", menu=settingsmenu)
playermenu.add_radiobutton(label = 'Winamp', value = 'Winamp', command= lambda type='winamp': app.refreshMenu(type))
playermenu.add_radiobutton(label = 'iTunes', value = 'iTunes', command= lambda type='iTunes': app.refreshMenu(type))





root.mainloop()
