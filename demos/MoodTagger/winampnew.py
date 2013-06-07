#    Copyright (C) 2009 Yaron Inger, http://ingeration.blogspot.com
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#    or download it from http://www.gnu.org/licenses/gpl.txt

#    Revision History:
#
#    11/05/2008 Version 0.1
#    	- Initial release
#
#    31/12/2009 Version 0.2
#    	- Added support for keyword queries (queryAsKeyword)

from ctypes import *
import win32api, win32con, win32gui, win32process, pywintypes
import os
import random
import time

class Winamp(object):
	# Winamp main window IPC
	WM_WA_IPC = win32con.WM_USER
	# Winamp's media library IPC
	WM_ML_IPC = WM_WA_IPC + 0x1000

	# add item to playlist
	IPC_ENQUEUEFILE = 100
	# playback status
	IPC_ISPLAYING = 104
	# delete winamp's internal playlist
	IPC_DELETE = 101
	# get output time
	IPC_GETOUTPUTTIME = 105

	#gets playlist position
	IPC_GETLISTPOS = 125
	# sets playlist position
	IPC_SETPLAYLISTPOS = 121
	# sets volume
	IPC_SETVOLUME = 122
	# gets list length
	IPC_GETLISTLENGTH = 124
	# gets given index playlist file
	IPC_GETPLAYLISTFILE = 211
	# gets given index playlist title
	IPC_GETPLAYLISTTITLE = 212

	#get currently playng filename
	IPC_GET_PLAYING_FILENAME = 3031

	# next playlist track
	IPC_GET_NEXT_PLITEM = 361
	# current playing title
	IPC_GET_PLAYING_TITLE = 3034

	# runs an ml query
	ML_IPC_DB_RUNQUERY = 0x0700
	# runs an ml query as a string
	ML_IPC_DB_RUNQUERY_SEARCH = 0x0701
	# frees ml query results from winamp's memory
	ML_IPC_DB_FREEQUERYRESULTS = 0x0705

	# playback possible options
	PLAYBACK_NOT_PLAYING = 0
	PLAYBACK_PLAYING = 1
	PLAYBACK_PAUSE = 3

	GET_TRACK_POSITION = 0
	GET_TRACK_LENGTH = 1

	# winamp buttons commands
	BUTTON_COMMAND_PREVIOUS = 40044
	BUTTON_COMMAND_PLAY = 40045
	BUTTON_COMMAND_PAUSE = 40046
	BUTTON_COMMAND_STOP = 40047
	BUTTON_COMMAND_NEXT = 40048

	# sort playlist by path and filename
	ID_PE_S_PATH = 40211

	class COPYDATATYPE(Structure):
		_fields_ = [("szData", c_char_p)]

	"""
	typedef struct tagCOPYDATASTRUCT {
		ULONG_PTR dwData;
		DWORD cbData;
		PVOID lpData;
	} COPYDATASTRUCT, *PCOPYDATASTRUCT;

	dwData
		Specifies data to be passed to the receiving application. 
	cbData
		Specifies the size, in bytes, of the data pointed to by the lpData member. 
	lpData
		Pointer to data to be passed to the receiving application. This member can be NULL.
	"""
	class COPYDATASTRUCT(Structure):
		_fields_ = [("dwData", c_ulong),
					("cbData", c_ulong),
					("lpData", c_void_p)]

	"""
	typedef struct 
	{
	  itemRecord *Items;
	  int Size;
	  int Alloc;
	} itemRecordList;
	"""
	class itemRecordList(Structure):
		_fields_ = [("Items", c_void_p),
					("Size", c_int),
					("Alloc", c_int)]

	"""
	typedef struct
	{
	  char *filename;
	  char *title;
	  char *album;
	  char *artist;
	  char *comment;
	  char *genre;
	  int year;
	  int track;
	  int length;
	  char **extended_info;
	  // currently defined extended columns (while they are stored internally as integers
	  // they are passed using extended_info as strings):
	  // use getRecordExtendedItem and setRecordExtendedItem to get/set.
	  // for your own internal use, you can set other things, but the following values
	  // are what we use at the moment. Note that setting other things will be ignored
	  // by ML_IPC_DB*.
	  // 
	  //"RATING" file rating. can be 1-5, or 0 or empty for undefined
	  //"PLAYCOUNT" number of file plays.
	  //"LASTPLAY" last time played, in standard time_t format
	  //"LASTUPD" last time updated in library, in standard time_t format
	  //"FILETIME" last known file time of file, in standard time_t format
	  //"FILESIZE" last known file size, in kilobytes.
	  //"BITRATE" file bitrate, in kbps
		//"TYPE" - "0" for audio, "1" for video

	} itemRecord;
	"""
	class itemRecord(Structure):
		_fields_ = [("filename", c_char_p),
					("title", c_char_p),
					("album", c_char_p),
					("artist", c_char_p),
					("comment", c_char_p),
					("genre", c_char_p),
					("year", c_int),
					("track", c_int),
					("length", c_int),
					("extended_info", c_char_p)]

	"""
	typedef struct 
	{
	  char *query;
	  int max_results;      // can be 0 for unlimited
	  itemRecordList results;
	} mlQueryStruct;
	"""
	class mlQueryStruct(Structure):
		pass

	def __init__(self):
		self.__initStructures()

		# get important Winamp's window handles
		try:
			self.__mainWindowHWND = self.__findWindow([("Winamp v1.x", None)])
			self.__playlistHWND = self.__findWindow([("BaseWindow_RootWnd", None), 
			("BaseWindow_RootWnd", "Playlist Editor"), 
			("Winamp PE", "Winamp Playlist Editor")])
			self.__mediaLibraryHWND = self.__findWindow([("BaseWindow_RootWnd", None), 
			("BaseWindow_RootWnd", "Winamp Library"), 
			("Winamp Gen", "Winamp Library"), (None, None)])
		except pywintypes.error, e:
			raise RuntimeError("Cannot find Winamp windows. Is winamp started?")

		self.__processID = win32process.GetWindowThreadProcessId(self.__mainWindowHWND)[1]

		# open Winamp's process
		self.__hProcess = windll.kernel32.OpenProcess(win32con.PROCESS_ALL_ACCESS, False, self.__processID)
	
	def detach(self):
		"""Detaches from Winamp's process."""
		windll.kernel32.CloseHandle(self.__hProcess)

	def __initStructures(self):
		# cannot be done statically, because mlQueryStruct doesn't know any self.itemRecordList
		try:
			self.mlQueryStruct._fields_ = [("query", c_char_p),
							("max_results", c_int),
							("itemRecordList", self.itemRecordList)]
		except AttributeError:
			# mlQueryStruct already initialized
			pass
	
	def __findWindow(self, windowList):
		"""Gets a handle to the lowest window in the given windows hierarchy.

			The given list should be in format of [<Window Class>, <Window Name>].
		"""
		currentWindow = None

		for i in xrange(len(windowList)):
			if currentWindow is None:
				currentWindow = win32gui.FindWindow(windowList[i][0], windowList[i][1])
			else:
				currentWindow = win32gui.FindWindowEx(currentWindow, 0, windowList[i][0], windowList[i][1])
		
		return currentWindow
	
	def __setattr__(self, attr, value):
		if attr == "playlist":
			self.clearPlaylist()

			[self.enqueueFile(item.filename) for item in value]
		else:
			object.__setattr__(self, attr, value)

	def __getattr__(self, attr):
		if attr == "playlist":
			return self.getPlaylistFilenames()
		else:
			try:
				return object.__getattr__(self, attr)
			except AttributeError:
				raise AttributeError, attr

	def __readStringFromMemory(self, address, isUnicode = False):
		"""Reads a string from Winamp's memory address space."""

		if isUnicode:
			bufferLength = win32con.MAX_PATH * 2
			buffer = create_unicode_buffer(bufferLength * 2)
		else:
			bufferLength = win32con.MAX_PATH
			buffer = create_string_buffer(bufferLength)

		bytesRead = c_ulong(0)

		"""Note: this is quite an ugly hack, because we assume the string will have a maximum
		size of MAX_PATH (=260) and that we're not in an end of a page.
		
		A proper way to solve it would be:
			1. calling VirutalQuery.
			2. Reading one byte at a time. (?)
			3. Use CreateRemoteThread to run strlen on Winamp's process.
		"""
		windll.kernel32.ReadProcessMemory(self.__hProcess, address, buffer, bufferLength, byref(bytesRead))

		return buffer.value
	
	def enqueueFile(self, filePath):
		"""Enqueues a file in Winamp's playlist.

		filePath is the given file path to enqueue.
		"""

		# prepare copydata structure for sending data
		cpyData = create_string_buffer(filePath)
		cds = self.COPYDATASTRUCT(c_ulong(self.IPC_ENQUEUEFILE),
							 c_ulong(len(cpyData.raw)),
							 cast(cpyData, c_void_p))

		# send copydata message
		win32api.SendMessage(self.__mainWindowHWND,
							 win32con.WM_COPYDATA,
							 0,
							 addressof(cds))

	def query(self, queryString, queryType = ML_IPC_DB_RUNQUERY):
		"""Queries Winamp's media library and returns a list of items matching the query.
		
		The query should include filters like '?artist has \'alice\''.
		For more information, consult your local Winamp forums or media library.
		"""
		queryStringAddr = self.__copyDataToWinamp(queryString)

		# create query structs and copy to winamp
		recordList = self.itemRecordList(0, 0, 0)
		queryStruct = self.mlQueryStruct(cast(queryStringAddr, c_char_p), 0, recordList)
		queryStructAddr = self.__copyDataToWinamp(queryStruct)

		# run query
		win32api.SendMessage(self.__mediaLibraryHWND, self.WM_ML_IPC, queryStructAddr, queryType)

		receivedQuery = self.__readDataFromWinamp(queryStructAddr, self.mlQueryStruct)

		items = []

		buf = create_string_buffer(sizeof(self.itemRecord) * receivedQuery.itemRecordList.Size)
		windll.kernel32.ReadProcessMemory(self.__hProcess, receivedQuery.itemRecordList.Items, buf, sizeof(buf), 0)

		for i in xrange(receivedQuery.itemRecordList.Size):
			item = self.__readDataFromWinamp(receivedQuery.itemRecordList.Items + (sizeof(self.itemRecord) * i), self.itemRecord)

			self.__fixRemoteStruct(item)

			items.append(item)

		# free results
		win32api.SendMessage(self.__mediaLibraryHWND, self.WM_ML_IPC, queryStructAddr, self.ML_IPC_DB_FREEQUERYRESULTS)

		return items
	
	def queryAsKeyword(self, queryString):
		"""Queries Winamp's media library and returns a list of items matching the query.
		
		The query should be a keyword, like 'alice' (then the query is then treated as a string query).
		This makes Winamp search the requested keyword in every data field in the media library database
		(such as Artist, Album, Track Name, ...).
		"""
		return self.query(queryString, self.ML_IPC_DB_RUNQUERY_SEARCH)
	
	def __copyDataToWinamp(self, data):
		if type(data) is str:
			dataToCopy = create_string_buffer(data)
		else:
			dataToCopy = data

		# allocate data in Winamp's address space
		lpAddress = windll.kernel32.VirtualAllocEx(self.__hProcess, None, sizeof(dataToCopy), win32con.MEM_COMMIT, win32con.PAGE_READWRITE)
		# write data to Winamp's memory
		windll.kernel32.WriteProcessMemory(self.__hProcess, lpAddress, addressof(dataToCopy), sizeof(dataToCopy), None)

		return lpAddress
	
	def __readDataFromWinamp(self, address, ctypesType):
		if ctypesType is c_char_p:
			buffer = create_string_buffer(win32con.MAX_PATH)
		else:
			buffer = create_string_buffer(sizeof(ctypesType))

		bytesRead = c_ulong(0)
		if windll.kernel32.ReadProcessMemory(self.__hProcess, address, buffer, sizeof(buffer), byref(bytesRead)) == 0:
			# we're in a new page
			if address / 0x1000 != address + sizeof(buffer):
				# possible got into an unpaged memory region, read until end of page
				windll.kernel32.ReadProcessMemory(self.__hProcess, address, buffer, ((address + 0x1000) & 0xfffff000) - address, byref(bytesRead))
			else:
				raise RuntimeError("ReadProcessMemory failed at 0x%08x." % address)

		if ctypesType is c_char_p:
			return cast(buffer, c_char_p)
		else:
			return cast(buffer, POINTER(ctypesType))[0]
	
	def __fixRemoteStruct(self, structure):
		offset = 0

		for i in xrange(len(structure._fields_)):
			(field_name, field_type) = structure._fields_[i]

			if field_type is c_char_p or field_type is c_void_p:
				# get pointer address
				address = cast(addressof(structure) + offset, POINTER(c_int))[0]
				
				# ignore null pointers
				if address != 0x0:
					structure.__setattr__(field_name, self.__readDataFromWinamp(address, field_type))

			offset += sizeof(field_type)
	
	def __sendUserMessage(self, wParam, lParam, hwnd = None):
		"""Sends a user message to the given hwnd with the given wParam and lParam."""
		if hwnd is None:
			targetHWND = self.__mainWindowHWND
		else:
			targetHWND = hwnd

		return win32api.SendMessage(targetHWND, self.WM_WA_IPC, wParam, lParam)

	def __sendCommandMessage(self, wParam, lParam, hwnd = None):
		"""Sends a command message to the given hwnd with the given wParam and lParam."""
		if hwnd is None:
			targetHWND = self.__mainWindowHWND
		else:
			targetHWND = hwnd

		return win32api.SendMessage(targetHWND, win32con.WM_COMMAND, wParam, lParam)
	
	def getPlaybackStatus(self):
		"""Gets Winamp's playback status. Use the constants PLAYBACK_NOT_PLAYING, 
		PLAYBACK_PLAYING and PLAYBACK_PAUSE = 3 to resolve status."""
		return self.__sendUserMessage(0, self.IPC_ISPLAYING)
	
	def getPlayingTrackLength(self):
		"""Gets the length in second of the current playing track."""
		return self.__sendUserMessage(self.GET_TRACK_LENGTH, self.IPC_GETOUTPUTTIME)

	def getPlayingTrackPosition(self):
		"""Gets the position in milliseconds of the current playing track."""
		return self.__sendUserMessage(self.GET_TRACK_POSITION, self.IPC_GETOUTPUTTIME)
	
	def clearPlaylist(self):
		"""Clears the playlist."""
		return self.__sendUserMessage(0, self.IPC_DELETE)
	
	def setPlaylistPosition(self, position):
		"""Sets the playlist position in the given position number (zero based)."""
		return self.__sendUserMessage(position, self.IPC_SETPLAYLISTPOS)
	
	def setVolume(self, volume):
		"""Sets the internal Winamp's volume meter. Will only accept values in the range 0-255."""
		assert volume >= 0 and volume <= 255

		self.__sendUserMessage(volume, self.IPC_SETVOLUME)
	
	def getCurrentPlayingTitle(self):
		"""Returns the title of the current playing track"""
		address = self.__sendUserMessage(0, self.IPC_GET_PLAYING_TITLE)

		return self.__readStringFromMemory(address, True)
	
	def getPlaylistFile(self, position):
		"""Returns the filename of the current selected file in the playlist."""
		address = self.__sendUserMessage(position, self.IPC_GETPLAYLISTFILE)

		return self.__readStringFromMemory(address)

	def getPlaylistTitle(self, position):
		"""Returns the title of the current selected file in the playlist."""
		address = self.__sendUserMessage(position, self.IPC_GETPLAYLISTTITLE)

		return self.__readStringFromMemory(address)
	
	def getListLength(self):
		"""Returns the length of the current playlist."""
		return self.__sendUserMessage(0, self.IPC_GETLISTLENGTH)

	def getListPosition(self):
		"""Returns the position of the playlist"""
		return self.__sendUserMessage(0, self.IPC_GETLISTPOS)
	
	def getPlaylistFilenames(self):
		"""Retrieves a list of the current playlist song filenames."""
		return [self.getPlaylistFile(position) for position in range(self.getListLength())]
	
	def getPlaylistTitles(self):
		"""Retrieves a list of the current playlist song titles."""
		return [self.getPlaylistTitle(position) for position in range(self.getListLength())]
	
	def next(self):
		"""Sets playlist marker to next song."""
		self.__sendCommandMessage(self.BUTTON_COMMAND_NEXT, 0)
	
	def previous(self):
		"""Sets playlist marker to previous song."""
		self.__sendCommandMessage(self.BUTTON_COMMAND_PREVIOUS, 0)
	
	def pause(self):
		"""Pauses Winamp's playback."""
		self.__sendCommandMessage(self.BUTTON_COMMAND_PAUSE, 0)

	def play(self):
		"""Starts / resumes playing Winamp's playback, or restarts current playing song."""
		self.__sendCommandMessage(self.BUTTON_COMMAND_PLAY, 0)
	
	def stop(self):
		"""Stops Winamp's playback."""
		self.__sendCommandMessage(self.BUTTON_COMMAND_STOP, 0)
	
	def sortPlaylist(self):
		"""Sorts the current playlist alphabetically."""
		self.__sendCommandMessage(self.ID_PE_S_PATH, 0, self.__playlistHWND)
	
	def playAlbum(self, album):
		"""Plays a given album name."""
		self.playlist = self.query("album = \"%s\"" % album)
		self.stop()
		self.sortPlaylist()
		self.setPlaylistPosition(0)
		self.play()

def printMediaLibraryItem(item):
	print "Filename: %s\nTrack: %s, Album: %s, Artist: %s\nComment: %s, Genre: %s" % (item.filename, item.track, item.album, item.artist, item.comment, item.genre)