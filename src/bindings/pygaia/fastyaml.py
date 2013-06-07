#!/usr/bin/env python
# -*- coding: utf-8 -*-

try:
    import yaml

    if 'CLoader' in dir(yaml):
        load = lambda x: yaml.load(x, yaml.CLoader)
        load_all = lambda x: yaml.load_all(x, yaml.CLoader)
    else:
        load = yaml.load
        load_all = yaml.load_all

    if 'CDumper' in dir(yaml):
        dump = lambda x, stream = None: yaml.dump(x, stream, Dumper=yaml.CDumper)
    else:
        dump = yaml.dump

except ImportError:
    print 'WARNING: could not import yaml module'


def loadfile(filename):
    return load(open(filename).read())
