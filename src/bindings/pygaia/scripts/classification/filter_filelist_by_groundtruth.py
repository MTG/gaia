# This script filters descriptors filelist removing mapping for
# all tracks that are not present in the ground truth file

import sys, yaml, argparse

parser = argparse.ArgumentParser(
    description='Filter filelist removing mappings for tracks not in ground truth.')
parser.add_argument('gt_file', help='ground truth file')
parser.add_argument('filelist_file', help='filelist file')
parser.add_argument('results_filelist_file', help='result filelist file')
args = parser.parse_args()

gt_file = args.gt_file
filelist_file = args.filelist_file
results_filelist_file = args.results_filelist_file

gt = yaml.load(open(gt_file, 'r'))
filelist = yaml.load(open(filelist_file, 'r'))

result = {}
for track, path in filelist.items():
    if track in gt['groundTruth']:
        result[track] = path

with open(results_filelist_file, 'w') as f:
    yaml.dump(result, f)

