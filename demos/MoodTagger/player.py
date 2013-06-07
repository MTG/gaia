import win32com.client
from nowplayingfeatures import *
from winampnew import *

class Player:

    def __init__(self, ptype):
        if(ptype=='iTunes'):
            self.app = win32com.client.gencache.EnsureDispatch("iTunes.Application")
            self.app.Play()
        elif(ptype=='winamp'):
            self.app = Winamp()
        self.type = ptype

    def sendCurrentSongToEssentia(self):
        if(self.type=='iTunes'):
            return sendCurrentiTunesSongToEssentia(self.app)
        elif(self.type=='winamp'):
            return sendCurrentWinampSongToEssentia(self.app, self.app.getListPosition())

    def changeType(self, newtype):
        if(newtype != self.type):
            if(newtype == 'iTunes'):
                self.app = win32com.client.gencache.EnsureDispatch("iTunes.Application")
            elif(newtype=='winamp'):
                self.app = Winamp()
        self.type = newtype
        print self.type

    def getCurrentTitle(self):
        if(self.type=='iTunes'):
            return self.app.CurrentTrack.Name
        elif(self.type=='winamp'):
            filename = self.app.getCurrentPlayingTitle()
            index = filename.find("-")+2
            title = filename[index:]
            return title
        else:
            return ""

    def getCurrentArtist(self):
        if(self.type=='iTunes'):
            return self.app.CurrentTrack.Artist
        elif(self.type=='winamp'):
            filename=self.app.getCurrentPlayingTitle().split("-")
            return filename[0]
        else:
            return ""

    def getCurrentLength(self):
        if(self.type=='iTunes'):
            time = self.app.CurrentTrack.Time
            timearray = time.split(':')
            seconds = int(timearray[0])*60+(int(timearray[1]))
            return seconds
        elif(self.type=='winamp'):
            return self.app.getPlayingTrackLength()

    def getCurrentPosition(self):
        if(self.type=='iTunes'):
            return self.app.PlayerPosition
        elif(self.type=='winamp'):
            position = self.app.getPlayingTrackPosition()/1000
            return position
        else:
            return 0



    



    
