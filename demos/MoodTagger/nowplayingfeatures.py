import win32com.client
import os
import subprocess
import winampnew

#iTunes = win32com.client.gencache.EnsureDispatch("iTunes.Application")
		
def sendCurrentiTunesSongToEssentia(iTunes):

	current = win32com.client.CastTo(iTunes.CurrentTrack, "IITFileOrCDTrack")	
	songPath = current.Location
	outputstr = current.Name + '.sig'
	subprocess.call([ r'C:\Program Files\Essentia\extractors\streaming_extractor.exe', songPath, outputstr])
	return outputstr


def sendCurrentWinampSongToEssentia(winamp, index):
	songPath = winamp.getPlaylistFile(index)
	outputstr = winamp.getPlaylistTitle(index) + ".sig"
	subprocess.call([ r'C:\Program Files\Essentia\extractors\streaming_extractor.exe', songPath, outputstr])
	return outputstr




