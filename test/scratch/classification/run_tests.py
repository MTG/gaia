#!/usr/bin/env python
# -*- coding: utf-8 -*-

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
