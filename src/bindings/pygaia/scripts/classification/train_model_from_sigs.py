
import train_model
import json_to_sig

import gaia2.fastyaml as yaml
import sys
import os
import shutil
import json
import glob
from argparse import ArgumentParser

template = {"type": "singleClass",
            "version": 1.0,
            "className": "",
            "groundTruth": {}
            }


def get_files_in_dir(dirname, extension):
    return glob.glob(os.path.join(dirname, "*.%s" % extension))


def main(input_directory, output_directory, project_name, force=False):
    print("looking for data in dir", input_directory)
    print("storing results in dir", output_directory)

    project_dir = os.path.abspath(input_directory)

    projname = project_name

    output_dir = os.path.abspath(output_directory)

    # if config/results exist, need force to rm them
    project_file = os.path.join(output_dir, "%s.project" % projname)
    results_model_file = os.path.join(output_dir, "%s.history" % projname)
    resultsdir = os.path.join(output_dir, "results")

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

    train_model.trainModel(groundtruth_name, yaml_name,
                           project_file, resultsdir, results_model_file)


if __name__ == "__main__":
    parser = ArgumentParser(description="Generates a model of the sig files "
                                        "in `directory`. Converts to yaml if "
                                        "needed.")
    parser.add_argument('input_directory',
                        help='directory with the json/sig files.')
    parser.add_argument('output_directory',
                        help='directory with the output files.')
    parser.add_argument('project_name',
                        help='the project name.')
    parser.add_argument('--force', '-f', action='store_true',
                        help='directory with the output files.')

    args = parser.parse_args()

    main(args.input_directory, args.output_directory,
         args.project_name, args.force)
