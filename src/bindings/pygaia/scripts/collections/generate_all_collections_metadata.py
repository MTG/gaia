#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, subprocess, yaml
from os.path import *


def usage():
    print '%s mtgdb_audio_mount_point' % sys.argv[0]
    sys.exit(1)

if __name__ == '__main__':
    try:
        mtgdb = sys.argv[1]
    except:
        usage()

    for db in yaml.load(open('mtgdb_collections.yaml').read()):
        cmd = [ 'python', 'generate_collection_metadata.py',
                '-g', db['groundtruthType'], join(mtgdb, db['location']), db['type'] ]

        subprocess.call(cmd)
