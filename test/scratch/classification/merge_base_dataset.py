#!/usr/bin/env python
# -*- coding: utf-8 -*-

from config import *
import glob
import os.path


def getSignatureData(basedir, ext):
    '''NOTE: assumes that the class is the directory in which the file is located'''
    mergelist = {}
    groundTruth = {}

    for classdir in glob.glob(basedir + '/*'):
        cname = classdir.split('/')[-1]
        for filename in glob.glob(classdir + '/*.' + ext):
            # small hack...
            if ext == 'sig' and filename.endswith('.neq.sig'):
                continue

            pointId = filename.split('/')[-1]

            mergelist[pointId] = filename
            groundTruth[pointId] = cname

    return mergelist, groundTruth


def getSignatureData_MirexArtist(basedir, ext):
    '''NOTE: assumes that the class is the directory in which the file is located'''
    mergelist = {}
    groundTruth = {}

    for classdir in glob.glob(basedir + '/*'):
        cname = classdir.split('/')[-1]
        for filename in glob.glob(classdir + '/*/*.' + ext):
            # small hack...
            if ext == 'sig' and filename.endswith('.neq.sig'):
                continue

            pointId = filename.split('/')[-1]

            mergelist[pointId] = filename
            groundTruth[pointId] = cname

    return mergelist, groundTruth

def getSignatureData_CyrilMoods(basedir, ext):
    '''NOTE: assumes that the class is the directory in which the file is located'''
    mergelist = {}
    groundTruth = {}

    for classdir in glob.glob(basedir + '/*'):
        cname = classdir.split('/')[-1]

        # perform class selection
        if cname in mood_classes:
            for filename in glob.glob(classdir + '/*.' + ext):
                # small hack...
                if ext == 'sig' and filename.endswith('.neq.sig'):
                    continue

                pointId = filename.split('/')[-1]

                mergelist[pointId] = filename
                groundTruth[pointId] = cname

    return mergelist, groundTruth

def getSignatureData_MirexMoods(basedir, ext):
    '''NOTE: assumes that the class is the directory in which the file is located'''
    mergelist = {}
    groundTruth = {}

    for classdir in glob.glob(basedir + '/*'):
        cname = classdir.split('/')[-1]

        # perform class selection
        if cname in mood_classes:
            for filename in glob.glob(classdir + '/*.' + ext):
                # small hack...
                if ext == 'sig' and filename.endswith('.neq.sig'):
                    continue

                pointId = filename.split('/')[-1]

                mergelist[pointId] = filename
                groundTruth[pointId] = cname

    return mergelist, groundTruth


def getSignatureData_iTMS(basedir, ext, metafile):
    mergelist = {}
    groundTruth = {}

    metadata = open(metafile).readlines()

    for m in metadata:
        cname = m.split('\t')[7]
        if cname in genre_classes:
            filename = m.split('\t')[9].strip()
            filename = filename.replace('http://audio.test.com/sample/audio/mp3', basedir) + '.' + ext
            pointId = filename.replace(basedir + '/', '').replace('/', '_')
            mergelist[pointId] = filename
            groundTruth[pointId] = cname

    return mergelist, groundTruth

def mergeDataSet(eqloud):

    if eqloud == 'eqloud': ext = 'sig'
    else: ext = 'neq.sig'

    datasetName = datasetdir + className + '_%s.db' % eqloud

    if os.path.exists(datasetName):
        print 'Dataset already exists:', datasetName
        return

    if className == 'genre_itms':
        mergelist, groundTruth = getSignatureData_iTMS(str(basedir), ext, metafile)
    elif className == 'mood':
        mergelist, groundTruth = getSignatureData_CyrilMoods(str(basedir), ext)
    elif className == 'mood_mirex':
        mergelist, groundTruth = getSignatureData_MirexMoods(str(basedir), ext)
    elif className == 'artist':
        mergelist, groundTruth = getSignatureData_MirexArtist(str(basedir), ext)
    else: # genre
        mergelist, groundTruth = getSignatureData(str(basedir), ext)

    # merge dataset
    import gaia2
    ds = gaia2.DataSet.mergeFiles(mergelist)

    # preprocessing common to all tests:
    ds = gaia2.transform(ds, 'removevl')
    ds = gaia2.transform(ds, 'fixlength')
    ds = gaia2.transform(ds, 'cleaner')

    # transform the dataset to add the class information
    ds = gaia2.transform(ds, 'addfield', { 'string': className })

    for p in ds.points():
        p[className] = groundTruth[p.name()]

    ds.save(datasetName)

    # write groundTruth as pickled file
    import cPickle
    cPickle.dump(groundTruth, open(datasetName + '.groundtruth', 'w'))



if __name__ == '__main__':
    mergeDataSet('eqloud')
    mergeDataSet('neqloud')
