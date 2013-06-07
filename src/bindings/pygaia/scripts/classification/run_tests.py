#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import cvar
from gaia2.classification import ClassificationTaskManager
import os, os.path
import sys
import logging

debugLevel = logging.INFO

if __name__ == '__main__':
    # open the yaml file describing the analysis to perform
    try:
        yamlfile = os.path.abspath(sys.argv[1])
    except:
        print 'ERROR: You need to specify a yaml project file...'
        print 'Exiting...'
        sys.exit(1)

    logging.getLogger('gaia2.classification').setLevel(debugLevel)
    if debugLevel == logging.DEBUG:
        cvar.verbose = True
    else:
        cvar.verbose = False

    # move to the project file directory so all paths can be relative
    try:
        os.chdir(os.path.split(yamlfile)[0])
    except OSError:
        pass

    test = ClassificationTaskManager(yamlfile)
    test.run()

