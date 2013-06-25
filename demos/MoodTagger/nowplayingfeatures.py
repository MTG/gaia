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
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see http://www.gnu.org/licenses/

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




