#!/usr/bin/env python
# -*- coding: utf-8 -*-

import gaia2
from gaia2 import *
from gaia2.mtgdb import Collection
from gaia2.scripts.classification.generate_svm_history_from_config import trainSVM
import gaia2.fastyaml as yaml
import glob, random, os.path


COLLECTIONS = [ 'tzanetakis', 'timbre', 'mirex-ballroom', 'culture',
                'gender', 'live_studio', 'perceptual_speed',
                'speech_music', 'voice_instrumental',  'dortmund',
                'electronica', 'rosamerica', 'MirexMoods' ]


#COLLECTIONS = [ 'tzanetakis', 'timbre' ]
#COLLECTIONS = [ 'mirex-ballroom', 'culture' ]
#COLLECTIONS = [ 'gender', 'live_studio' ]
#COLLECTIONS = [ 'perceptual_speed', 'timbre' ]
#COLLECTIONS = [ 'speech_music', 'voice_instrumental' ]
#COLLECTIONS = [ 'dortmund', 'electronica' ]
#COLLECTIONS = [ 'rosamerica' ]

MOODS_COLLECTIONS = [ 'sad', 'happy', 'aggressive', 'relaxed', 'acoustic', 'electronic', 'party' ]

# mood & genre

NFOLDS = 5
DATASETS_DIR = '/var/data/dima_eval/datasets'
WORK_DIR = '/var/data/dima_eval/results'


def CN(collection):
    exceptions = { 'mirex-ballroom': 'ballroom',
                   'dortmund': 'genre_dortmund',
                   'electronica': 'genre_electronica',
                   'rosamerica': 'genre_rosamerica',
                   'perceptual_speed': 'perceptual_speed'
                   }
    try:
        return exceptions[collection.name]
    except:
        return collection.groundTruth.className


def loadCollections():
    collections = {}
    for cname in COLLECTIONS:
        print 'Loading collection %s...' % cname
        collections[cname] = Collection(cname)

    for mname in MOODS_COLLECTIONS:
        print 'Loading mood collection %s...' % mname
        collections['mood_%s' % mname] = Collection('CyrilMoods', groundTruth = 'mood_%s' % mname)

    return collections

def generateFolds(collections, nfolds):
    """creates a list of n folds which have uniform distribution of points amongst classes."""
    collectionFolds = {}

    for cname in collections.keys():
        groundTruth = collections[cname].groundTruth
        classes = set(groundTruth.values())

        # get map from class to point names
        iclasses = {}
        for c in classes:
            iclasses[c] = [ p for p in groundTruth.keys() if groundTruth[p] == c ]
            random.shuffle(iclasses[c])

        # get folds
        folds = []
        for i in range(nfolds):
            folds.append([])
            for c in iclasses.values():
                foldsize = len(c)//nfolds + 1 # +1 so we take all instances into account, last fold might have fewer instances
                folds[i] += c[ foldsize * i : foldsize * (i+1) ]

        collectionFolds[cname] = folds

    return collectionFolds



def trainSVMfolds(collections, folds):
    gaia2.verbose = False

    for cname in collections.keys():
        collec = collections[cname]

        print 'Training models for all folds for collection', cname
        # load best parameters
        filename = glob.glob('test/evaldata/essentia_svm_models/%s*.param' % CN(collec))[0]

        params = yaml.loadfile(filename)['model']
        if params.pop('classifier') != 'svm':
            raise Exception('Can only use this script on SVM config parameters.')

        preproc = params.pop('preprocessing')

        # load original preprocessed dataset
        ds = DataSet()
        ds.load('%s/%s-harm-%s.db' % (WORK_DIR, CN(collec), preproc))

        # add 'highlevel.' in front of the descriptor, this is what will appear in the final Essentia sigfile
        gt = collec.groundTruth.copy()
        gt.className = 'highlevel.' + CN(collec)

        # create and train datasets for all folds
        print 'Training for model:', gt.className
        for i, fold in enumerate(folds[cname]):
            print ' - fold', i
            model_filename = '%s/%s_%d.model' % (WORK_DIR, CN(collec), i)
            if os.path.exists(model_filename):
                print 'already computed'
                continue

            # remove points from the fold to have a training dataset
            dsf = ds.copy()
            toRemove = set(fold) & set(ds.pointNames())
            dsf.removePoints(list(toRemove))

            # train SVM
            h = trainSVM(dsf, gt, **params)

            h.save(model_filename)



def addSVMModels(collections, ds_orig, i):
    ds_result = ds_orig.copy()

    for cname in collections.keys():
        className = CN(collections[cname])
        print 'Adding descriptor', className

        h = TransfoChain()
        h.load('%s/%s_%d.model' % (WORK_DIR, className, i))
        ds_desc = h.partialMapDataSet(ds_orig, len(ds_orig.history().toPython()))
        ds_result = mergeDataSets(ds_result, ds_desc)

    return ds_result



def generateEvaluationDatasets(collections, folds):
    for cname in collections.keys():
        print '-'*100
        print 'For collection', cname
        collec = collections[cname]

        ds_orig = DataSet()
        ds_orig.load('%s/%s-harm.db' % (WORK_DIR, CN(collec)))

        for i, fold in enumerate(folds[cname]):
            print '\n  ---- fold', i
            testDS = '%s/%s_%d_test.db' % (WORK_DIR, CN(collec), i)
            if os.path.exists(testDS):
                print 'test DS already computed:', testDS
                continue

            ds = addSVMModels(collections, ds_orig, i)

            ds_train = ds.copy()
            toRemove = set(ds.pointNames()) & set(fold)
            ds_train.removePoints(list(toRemove))
            ds_train.save('%s/%s_%d_train.db' % (WORK_DIR, CN(collec), i))

            toKeep = set(ds.pointNames()) - set(fold)
            ds.removePoints(list(toKeep))
            ds.save(testDS)




def harmonizeDatasets(collections):
    for cname in collections.keys():
        collec = collections[cname]

        ds_orig = DataSet()
        ds_orig.load('%s/%s-raw.db' % (DATASETS_DIR, CN(collec)))

        ds_harm_filename = '%s/%s-harm.db' % (WORK_DIR, CN(collec))

        if os.path.exists(ds_harm_filename):
            ds_harm = DataSet()
            ds_harm.load(ds_harm_filename)

        else:
            ds_harm = transform(ds_orig, 'remove', { 'descriptorNames': [ 'lowlevel.silence_rate*', 'metadata.*', 'lowlevel.spectral_complexity.min' ],
                                                     'failOnUnmatched': False })
            ds_harm.save(ds_harm_filename)

        # reprocess all pre-processed datasets
        procs = [ 'basic', 'gaussianized', 'lowlevel', 'nobands', 'normalized', 'nosfx' ]

        for proc in procs:
            ds_proc = DataSet()
            ds_proc_filename = '%s/%s-%s.db' % (DATASETS_DIR, CN(collec), proc)
            ds_harm_filename = '%s/%s-harm-%s.db' % (WORK_DIR, CN(collec), proc)
            if os.path.exists(ds_harm_filename):
                print 'dataset already harmonized:', ds_proc_filename
                continue

            ds_proc.load(ds_proc_filename)
            ds_harm_proc = ds_proc.history().retransformDataSet(ds_harm, 2)
            ds_harm_proc.save(ds_harm_filename)



if __name__ == '__main__':
    c = loadCollections()

    try:
        os.mkdir(WORK_DIR)
    except OSError:
        pass

    # need to do some prep work before to harmonize all datasets layouts. This won't be
    # necessary anymore in the future when all is nicely generated with a single coherent
    # script, but at the moment we have to work with the data we have...
    harmonizeDatasets(c)

    cachedFolds = False
    foldsFile = '%s/folds.yaml' % WORK_DIR
    if os.path.exists(foldsFile):
        folds = yaml.loadfile(foldsFile)
    else:
        print 'Generating folds for all collections...'
        folds = generateFolds(c, NFOLDS)
        yaml.dump(folds, open(foldsFile, 'w'))

    print 'Training SVM models for their corresponding folds...'
    trainSVMfolds(c, folds)

    print 'Generating the evaluation datasets from the models...'
    generateEvaluationDatasets(c, folds)
