from __future__ import print_function, absolute_import
from . import train_model
from . import json_to_sig

import gaia2.fastyaml as yaml
import sys
import os
import glob
import shutil
from argparse import ArgumentParser

template = {"type": "singleClass",
            "version": 1.0,
            "className": "",
            "groundTruth": {}
            }


def get_files_in_dir(dirname, extension):
    return glob.glob(os.path.join(dirname, "*.%s" % extension))


def main(input_directory, output_directory, project_name, force=False,
         seed=None, cluster_mode=False, force_consistency=False):
    print("looking for data in dir", input_directory)
    print("storing results in dir", output_directory)

    project_dir = os.path.abspath(input_directory)

    projname = project_name

    output_dir = os.path.abspath(output_directory)

    # if config/results exist, need force to rm them
    project_file = os.path.join(output_dir, "%s.project" % projname)
    results_model_file = os.path.join(output_dir, "%s.history" % projname)
    resultsdir = os.path.join(output_dir, "results")

    if force:
        shutil.rmtree(output_directory, ignore_errors=True)

    if not os.path.exists(resultsdir):
        os.makedirs(resultsdir)

    classes = [d for d in os.listdir(project_dir)
               if os.path.isdir(os.path.join(project_dir, d))]
    print(classes)

    groundtruth_name = os.path.join(resultsdir, "groundtruth.yaml")
    json_name = os.path.join(resultsdir, "filelist-to-convert.yaml")
    yaml_name = os.path.join(resultsdir, "filelist-yaml.yaml")

    filelist = {}
    groundtruth = template

    for c in classes:
        jsonfiles = get_files_in_dir(os.path.join(project_dir, c), "json")
        yamlfiles = get_files_in_dir(os.path.join(project_dir, c), "sig")

        yamlfilesNoExt = [f.rstrip('.sig') for f in yamlfiles]

        if (len(jsonfiles) > 0):
            filesToConvert = {
                os.path.splitext(os.path.basename(f))[0]:
                os.path.join(project_dir, c, f)
                for f in jsonfiles if f.rstrip('.json') not in yamlfilesNoExt
            }

            print("{} json files have to be converted into yamls. "
                  "{} already exist.".format(len(filesToConvert),
                                             len(yamlfiles)))

            yaml.dump(filesToConvert, open(json_name, "w"))
            json_to_sig.convertJsonToSig(json_name, yaml_name)

        yamlfiles = get_files_in_dir(os.path.join(project_dir, c), "sig")

        print("got", len(yamlfiles), "files in", c)
        for f in yamlfiles:
            id = os.path.splitext(os.path.basename(f))[0]
            groundtruth["groundTruth"][id] = c
            filelist[id] = os.path.join(project_dir, c, f)

    # check directories for sig and convert
    groundtruth["className"] = projname
    yaml.dump(filelist, open(yaml_name, "w"))
    yaml.dump(groundtruth, open(groundtruth_name, "w"))

    if os.path.exists(json_name):
        os.remove(json_name)

    train_model.train_model(groundtruth_name, yaml_name,
                           project_file, resultsdir, results_model_file,
                           seed=seed, cluster_mode=cluster_mode,
                           force_consistency=force_consistency)


if __name__ == "__main__":
    parser = ArgumentParser(description="Generates a model trained using descriptor files (*.sig, *.json) "
                                        "in a directory. Each subdirectory is considered a class with the "
                                        "descriptor files therein being its training examples. JSON files are "
                                        "converted to YAML if needed.")
    parser.add_argument('input_directory',
                        help='directory with the json/sig files.')
    parser.add_argument('output_directory',
                        help='directory to store the results and model output.')
    parser.add_argument('project_name',
                        help='the project name.')
    parser.add_argument('-f', '--force', action='store_true',
                        help='If set, retrain an existing model.')
    parser.add_argument('-s', '--seed', type=float, default=1,
                        help='seed used to generate the random folds. '
                             'Use 0 to use current time (will vary on each trial).')
    parser.add_argument('-cm', '--cluster_mode', action='store_true',
                        help='Open a new python process for each subtask.')
    parser.add_argument('-fc', '--force-consistency', action='store_true',
                        help='Checks if all the descriptor files were computed with the same Essentia version. '
                             'Throws an exception if not.')

    args = parser.parse_args()

    seed = args.seed
    if args.seed == 0:
        seed = None

    main(args.input_directory, args.output_directory, args.project_name, args.force, seed=seed,
         cluster_mode=args.cluster_mode, force_consistency=args.force_consistency)
