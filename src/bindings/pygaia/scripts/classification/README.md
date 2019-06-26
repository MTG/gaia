# Classification script

This folder contains scripts for training SVM models. Given a dataset, several parameters are tried through a grid search process using n-fold cross-validation.
A final model is trained with all the data (without validation set) featuring the best parameters combination in terms of accuracy. 

## Main routines
`train_model.py` is the main script. It can be both, imported as a module or run as an executable. This is the syntax for the latter case:
`train_model.py  [--seed SEED] [--cluster_mode] groundtruth_file filelist_file project_file project_dir results_model_file `

Parameters:
`groundtruth_file` is yaml file containing a relation between keys and labels. Refer to `groundtruth_example.yaml` in this folder for an example.
`filelist_file` is a yaml file containing a relation between keys and feature file paths. Refer to `filelist_example.yaml` in this folder for an example.
`project_file` is the path where the project configuration file will be stored.
`project_dir` is the base path for the files generated during the training process.
`results_model_file` is the path to the file where the best model ranking the best performance will be stored.
`--seed` is an optional numeric parameter that controls the data splits for the n-fold cross-validation. If not specified, folds are stochastically chosen.
`--cluster_mode` is a flag to explicitly use the subprocess module to open a new python process for each subtask.

Alternatively, the script `train_model_from_sigs.py` only requires a path to a folder containing subfolders representing the classes with the features.
For instance, given the path `base/path/` the script will expect a set of files organized in subfolders as `base/path/class-a/`, `base/path/class-b/`...
The scripts generates the `groundtruth_file` and the `filelist_file` and calls `train_model.py`.
`train_model_from_sigs.py [--force] [--seed SEED] [--cluster_mode] input_folder output_folder project_name `

Parameters:
`input_folder` the base path to the subfolders with features.
`output_folder` the project folder where to store all the generated files.
`--seed` is an optional numeric parameter that controls the data splits for the n-fold cross-validation. If not set, folds are stochastically chosen.
`--cluster_mode` is a flag to explicitly use the subprocess module to open a new python process for each subtask.
`--force` Whether to retrain a model if output_folder already has one.


Parameters:
input_directory: directory with the json/sig(yaml) feature files.
output_directory: directory with the output files.
project_name: the project name.
--force: whether to retrain an existing model
--seed: is an optional numeric parameter that controls the data splits for the n-fold cross-validation. If not specified, folds are stochastically chosen.

Note that this folder does not contain scripts for the obtention of features from audio. This step is done relying on the [Essentia Music Extracor](https://essentia.upf.edu/documentation/FAQ.html#training-and-running-classifier-models-in-gaia)

## How it works
`train_model` performs the following actions:
1. Create a project configuration file containing the specified paths and parameter configurations to try in the SVM. (`generate_classification_project.py`). The values to try are contained in `classification_project_template.yaml`  and they can be modified in the generated one.
2. `runs_tests.py` tries all the parameter combinations. This creates a `classificationTaskManager.py` instance that runs each `classificationTask.py` in a concurrent way.
3. `generate_params_report.py` gets the results obtained by each set of parameters and creates a .tsv file that ranks them in terms of accuracy and normalized accuracy.
4. `select_best_model.py` reads the results obtained with each set of parameters and generates a final model trained with all the data featuring the best parameters with the higher accuracy.

## Additional tools
`retrain_model.py` is a tool that trains a model given a parameter configuration file (files in the results folder).
`visualize_confusion_matrice.py` is a script that transforms a classification result into an HTML confusion matrix.
`json_to_sig.py` is a script that transforms the feature files into sig (yaml) in case they are in json format.