#!/usr/bin/env python
import sys, glob, os
from os.path import *

def move_sub(basedir, subdir, dry_run = True):
    allf = glob.glob('%s/*' % basedir)
    #print allf
    audiodir = join(basedir, subdir)
    cmds = [ 'mkdir %s' % audiodir ]
    for f in allf:
        cmds += [ 'mv %s %s' % (f, audiodir) ]

    if len(cmds) == 1:
        # not a directory, most likely
        return

    if dry_run:
        for cmd in cmds:
            print cmd
    else:
        for cmd in cmds:
            os.system(cmd)


def usage():
    print 'add_audio_format.py basedir audioFormat [--yes]'
    print 'specifying --yes will actually make the changes, otherwise the commands are just printed on screen'
    sys.exit(1)



if __name__ == '__main__':
    try:
        basedir = sys.argv[1]
        audioformat = sys.argv[2]
    except:
        usage()

    dry_run = True
    if len(sys.argv) == 4 and sys.argv[3] == '--yes':
        dry_run = False
    else:
        if len(sys.argv) > 3:
            usage()

    # need to move:
    #  - the audio folder
    #  - the descriptors folders

    # 1- get the list of audio folder
    move_sub(join(basedir, 'audio'), audioformat, dry_run)

    # 2- move the descriptors folders
    for descf in glob.glob(join(basedir, 'descriptors', '*')):
        move_sub(descf, audioformat, dry_run)
