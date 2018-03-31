#!/usr/bin/env python
# -*- coding: utf-8 -*-

# <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
# 
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY 
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>

# This script filters descriptors filelist removing mapping for 
# all tracks that are not present in the ground truth file

from __future__ import print_function
import sys, yaml

try:
    gt_file = sys.argv[1]
    filelist_file = sys.argv[2]
    results_filelist_file = sys.argv[3]
except:
    print ('usage:', sys.argv[0], '<ground-truth-file> <filelist-file> <result-filelist-file>')
    sys.exit()

gt = yaml.load(open(gt_file, 'r'))
filelist = yaml.load(open(filelist_file, 'r'))

result = {}
for track, path in filelist.iteritems():
    if track in gt['groundTruth']:
        result[track] = path

with open(results_filelist_file, 'w') as f:
    yaml.dump(result, f)

