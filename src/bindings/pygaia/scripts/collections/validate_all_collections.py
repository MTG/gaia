#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, subprocess, yaml
from os.path import *
from gaia2.mtgdb.environment import MTGDB_AUDIO
import gaia2


if __name__ == '__main__':
    collections_file = join(gaia2.rootdir(), 'mtgdb', 'mtgdb_collections.yaml')

    for db in yaml.load(open(collections_file).read()):
        cmd = [ 'python', join(split(__file__)[0], 'validate_collection.py'), join(MTGDB_AUDIO, db['location']) ]

        subprocess.call(cmd)

        print '\n'
