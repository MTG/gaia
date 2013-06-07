#!/usr/bin/env python
# -*- coding: utf-8 -*-


##### General options ##############################################################
PYTHONPATH = '/tmp/gaiatemp/usr/local/lib/python2.5/site-packages/'
import sys
sys.path = [ PYTHONPATH ] + sys.path

from mtgdbpath import MtgdbPath

# try to guess on which computer we're running (with its IP address) to set some env variables
import subprocess
for l in subprocess.Popen('/sbin/ifconfig', stdout = subprocess.PIPE).communicate()[0].split('\n'):
    if 'inet addr' in l:
        ipaddr = l.split('inet addr:')[1].split(' ')[0]
        break

username = subprocess.Popen('whoami', stdout = subprocess.PIPE).communicate()[0].strip()


G_ENV = { # Nico
          ('10.80.4.166', 'nwack'): { 'mtgdb_audio': '/iua-data/mtgdb-audio',
                                    'base_dir': '/home/nwack/dev/gaia/test/classification' },
          # griller Nico
          ('10.55.0.230', 'nwack'): { 'mtgdb_audio': '/raid/audio',
                                      'base_dir': '/home/nwack/dev/gaia/test/classification' },
          # griller Marco
          ('10.55.0.230', 'mmarchini'): { 'mtgdb_audio': '/raid/audio',
                                          'base_dir': '/home/mmarchini/gaia/test/classification' },
          # Nico casa
          ('192.168.1.3', 'download'): { 'mtgdb_audio': '/tmp',
                                         'base_dir': '/home/download/dev/gaia/test/classification' }
          }

try:
    userid = (ipaddr, username)
    MTGDB_AUDIO = G_ENV[userid]['mtgdb_audio']
    BASE_DIR = G_ENV[userid]['base_dir']
except:
    print 'Error: could not detect on which computer this program is running. Exiting...'
    print 'Please edit the config.py file to add your data to it to be able to run it.'
    sys.exit(1)


# make sure we always call the scripts with the first argument being the project config file
try:
    projectFile = sys.argv[1]
    execfile(projectFile)
except:
    print 'Error while trying to load the project file:\n'
    raise

commonConfig = dict(d.items()[0] for d in orderedCommonConfig)

evalConfig = {}
for key, value in orderedEvalConfig.items():
    evalConfig[key] = dict(d.items()[0] for d in value)
