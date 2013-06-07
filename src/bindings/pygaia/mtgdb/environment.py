#!/usr/bin/env python
# -*- coding: utf-8 -*-


# try to guess on which computer we're running (with its IP address) to set some env variables
import subprocess
for l in subprocess.Popen('/sbin/ifconfig', stdout = subprocess.PIPE).communicate()[0].split('\n'):
    if 'inet addr' in l:
        ipaddr = l.split('inet addr:')[1].split(' ')[0]
        break

username = subprocess.Popen('whoami', stdout = subprocess.PIPE).communicate()[0].strip()


G_ENV = { # Nico
          ('10.80.4.166', 'nwack'): { 'mtgdb_audio': '/mnt/mtgdb-audio',
                                      'base_dir': '/home/nwack/dev/gaia2' },
          # Nico griller
          ('10.55.0.230', 'nwack'): { 'mtgdb_audio': '/raid/audio',
                                      'base_dir': '/home/nwack/dev/gaia' },
          # Nico casa
          #('192.168.1.3', 'download'): { 'mtgdb_audio': '/tmp',
          #                               'base_dir': '/home/download/dev/gaia' },

          # Marco griller
          ('10.55.0.230', 'mmarchini'): { 'mtgdb_audio': '/raid/audio',
                                          'base_dir': '/home/mmarchini/gaia' },
          }

G_MOUNT_POINTS = { '10.80.4.166': '/mnt/mtgdb-audio', # Nico
                   '10.55.0.230': '/raid/audio',      # Griller
                   '10.80.6.78':  '/raid/audio'       # Buildbot
                   }

try:
    userid = (ipaddr, username)
    MTGDB_AUDIO = G_ENV[userid]['mtgdb_audio']
    BASE_DIR = G_ENV[userid]['base_dir']

except:
    BASE_DIR = None
    MTGDB_AUDIO = '/raid/audio' # this seems like a reasonable default

    try:
        MTGDB_AUDIO = G_MOUNT_POINTS[ipaddr]
    except:
        print 'Warning: could not detect on which computer this program is running.'
        print 'IP address:', ipaddr, '- username:', username
        print 'Please edit the mtgdb/environment.py file to add your data to it to be able to run it.'
