# Classification scripts

This folder contains scripts for training SVM models. Given a dataset, several parameters are tried through a grid search process using n-fold cross-validation.
A final model is trained with all the data (without a validation set) featuring the best parameter combination in terms of accuracy. 



## Main routines
The main model training script is `gaia2.scripts.classification.train_model`. It can be imported as a module or run as an executable:

```
$ python -m gaia2.scripts.classification.train_model --help
usage: train_model.py [-h] [--seed SEED] [--cluster_mode]
                      groundtruth_file filelist_file project_file project_dir
                      results_model_file

Generates a model trained using descriptor files specified in the groundtruth
and filelist.

positional arguments:
  groundtruth_file      yaml file containing a relation between keys and
                        labels.
  filelist_file         yaml file containing a relation between keys and
                        features file paths. Feature files should be in yaml
                        (sig) format
  project_file          path where the project configuration file will be
                        stored. If this file doesn't exist, then a new project
                        file will be made from a template
  project_dir           path to a directory where the best performing model
                        will be stored.
  results_model_file

optional arguments:
  -h, --help            show this help message and exit
  --seed SEED, -s SEED  seed used to generate the random folds. Use 0 to use
                        current time (will vary on each trial).
  --cluster_mode, -cm   Open a new python process for each subtask.
```


Alternatively, the module `train_model_from_sigs` only requires a path to a folder containing subfolders 
representing the classes with the features.

For example, a directory structure:

    project
    |- angry
    |  |- 1.json
    |  |- 2.json
    |  |- 3.json
    |  `- 4.json
    |- happy
    |  |- 1.json
    |  |- 2.json
    |  `- 3.json
    `- sad
       |- 1.json
       `- 2.json
       
Will generate a dataset with 3 classes (angry, happy, sad), with 4, 3, and 2 files in each class, respectively.

This tool generates the `groundtruth_file` and the `filelist_file` and calls `train_model`.

```
$ python -m gaia2.scripts.classification.train_model_from_sigs --help
usage: train_model_from_sigs.py [-h] [--force] [--seed SEED] [--cluster_mode]
                                input_directory output_directory project_name

Generates a model trained using descriptor files (*.sig, *.json) in a
directory. Each subdirectory is considered a class with the descriptor files
therein being its training examples. JSON files are converted to YAML if
needed.

positional arguments:
  input_directory       directory with the json/sig files.
  output_directory      directory to store the results and model output.
  project_name          the project name.

optional arguments:
  -h, --help            show this help message and exit
  --force, -f           If set, retrain an existing model.
  --seed SEED, -s SEED  seed used to generate the random folds. Use 0 to use
                        current time (will vary on each trial).
  --cluster_mode, -cm   Open a new python process for each subtask.
```


To create feature files, use the [Essentia Music Extractor](https://essentia.upf.edu/documentation/FAQ.html#training-and-running-classifier-models-in-gaia)

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