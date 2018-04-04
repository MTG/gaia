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

# This script creates a balanced ground truth given an unbalanced on by applying 
# random sampling. The size of the resulting classes equals to the minimum size 
# among original classes.

from __future__ import print_function
import sys
import yaml
from random import shuffle

try:
    input_gt = sys.argv[1]
    balanced_gt = sys.argv[2]
except:
    print ('usage:', sys.argv[0], '<input-grounttruth> <output-balanced-groundtruth>')
    sys.exit()

input_gt = yaml.load(open(input_gt, 'r'))

gt = {}
for t, l in list(input_gt['groundTruth'].items()):
    gt.setdefault(l, [])
    gt[l] += [t]

for label in gt:
    print (label, len(gt[label]))

min_class_len = min(len(gt[label]) for label in gt)
print ('Minimum class length:', min_class_len)

input_gt['groundTruth'] = {}
for label in gt:
    shuffle(gt[label])
    for track in gt[label][:min_class_len]:
        input_gt['groundTruth'][track] = label

with open(balanced_gt, 'w') as f:
    yaml.dump(input_gt, f)
