# This script creates a balanced ground truth given an unbalanced on by applying 
# random sampling. The size of the resulting classes equals to the minimum size 
# among original classes.

from __future__ import print_function
import argparse
import yaml
from random import shuffle


def main():
    parser = argparse.ArgumentParser(description='Create a balanced ground truth by random sampling.')
    parser.add_argument('input_groundtruth', help='input ground truth file')
    parser.add_argument('output_balanced_groundtruth', help='output balanced ground truth file')
    args = parser.parse_args()

    input_gt_file = args.input_groundtruth
    balanced_gt = args.output_balanced_groundtruth

    input_gt = yaml.load(open(input_gt_file, 'r'))

    gt = {}
    for t, l in input_gt['groundTruth'].items():
        gt.setdefault(l, [])
        gt[l] += [t]

    for label in gt:
        print(label, len(gt[label]))

    min_class_len = min(len(gt[label]) for label in gt)
    print('Minimum class length:', min_class_len)

    input_gt['groundTruth'] = {}
    for label in gt:
        shuffle(gt[label])
        for track in gt[label][:min_class_len]:
            input_gt['groundTruth'][track] = label

    with open(balanced_gt, 'w') as f:
        yaml.dump(input_gt, f)

if __name__ == '__main__':
    main()
