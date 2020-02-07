# This script filters descriptors filelist removing mapping for
# all tracks that are not present in the ground truth file

import sys, yaml

try:
    gt_file = sys.argv[1]
    filelist_file = sys.argv[2]
    results_filelist_file = sys.argv[3]
except:
    print('usage:', sys.argv[0], '<ground-truth-file> <filelist-file> <result-filelist-file>')
    sys.exit()

gt = yaml.load(open(gt_file, 'r'))
filelist = yaml.load(open(filelist_file, 'r'))

result = {}
for track, path in filelist.items():
    if track in gt['groundTruth']:
        result[track] = path

with open(results_filelist_file, 'w') as f:
    yaml.dump(result, f)

