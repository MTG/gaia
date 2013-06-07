#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, subprocess
import time

class ProcessTimeout:
    '''Class that checks that the given process doesn't run for longer than a given number of seconds, kills it otherwise.'''
    def __init__(self, seconds = 5, msg = None):
        self.seconds = seconds
        self.msg = msg
        self._watcher = subprocess.Popen([ 'python', 'processwatcher.py', str(os.getpid()), str(seconds) ])

    def __del__(self):
        print 'killing watcher process'
        subprocess.call([ 'kill', '-9', str(self._watcher.pid) ])


if __name__ == '__main__':
    ppid = int(sys.argv[1])
    secs = float(sys.argv[2])

    time.sleep(secs)

    subprocess.call(['kill', '-9', str(ppid) ])
