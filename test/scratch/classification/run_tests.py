#!/usr/bin/env python
# -*- coding: utf-8 -*-

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



from config import *
from common import *
from readresults import *
import sys, os
import multiprocessing
import subprocess



def execute_single_test(cmd):
    print cmd
    proc = subprocess.Popen(cmd + ' > /dev/null', shell = True)
    proc.wait()



if __name__ == '__main__':

    # first merge the dataset from the essentia signatures
    print '---- MERGING DATASET ----'
    os.system('python merge_base_dataset.py ' + projectFile)


    # then prepare the different datasets on different feature sets
    print '---- PREPARING FEATURE SETS ----'
    os.system('python do_feature_selection.py ' + projectFile)


    # compute classifiers results on all feature sets
    print '---- COMPUTING CLASSIFIERS RESULTS ----'
    cmds = []
    # first %s corresponds to dataset name, need to prepend "python run_single_test.py"
    evalRunTemplate = { 'simca': 'simca ' + datasetdir + className + '_%(eqloud)s.db %(featset)d %(coveredVariance)d %(alpha).2f',
                        'svm':   'svm ' + datasetdir + className + '_%(eqloud)s.db %(featset)d %(type)s %(kernel)s %(C)f %(gamma)f',
                        'mixed': 'mixed ' + datasetdir + className + '_%(eqloud)s.db %(featset)d %(rcadim)d %(alpha).2f',
                        'mixedsegments': 'mixedsegments ' + datasetdir + className + '_%(eqloud)s.db %(featset)d %(rcadim)d %(alpha).2f %(func1)s %(func2)s',
                        'rcasvm':   'rcasvm ' + datasetdir + className + '_%(eqloud)s.db %(featset)d %(rcadim)d %(type)s %(kernel)s %(C)f %(gamma)f'
                        }

    for c in getMissingConfigs():
        if c.evalType not in [ 'simca' ]:
            cmds += [ 'python run_single_test.py ' + projectFile + ' ' + evalRunTemplate[c.evalType] % c._asdict() ]

    # Run all!
    pool = multiprocessing.Pool(processes = 4)
    pool.map(execute_single_test, cmds)
