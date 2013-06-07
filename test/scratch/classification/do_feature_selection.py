#!/usr/bin/env python
# -*- coding: utf-8 -*-

from config import *
from gaia2 import *
import os


def baseFeatureSelect(ds_orig):
    #return transform(ds_orig, 'select', { 'descriptorNames': [ '.'+className, '*.mean', '*.var', '*cov', '*.value' ],
    #                                      'except': [ 'silence_rate_*', 'metadata*', 'beats_position*', 'bpm_estimates*',
    #                                      'bpm_intervals*', '*_peak_*', 'onset_times*', 'perceptual_speed*', 'rubato_*',
    #                                      'chords_histogram*', 'chords_progression*', '*hpcp*' ] })
    return transform(ds_orig, 'remove', { 'descriptorNames': [ 'silence_rate_*', 'metadata*', 
                                          '*hpcp*',
                                          '*.min', '*.max', '*.dmean', '*.dvar', '*.dmean2', '*.dvar2' ] })

bfs = baseFeatureSelect

def transform1(ds_orig):
    return bfs(ds_orig)

def transform2(ds_orig):
    return transform(bfs(ds_orig), 'select', { 'descriptorNames': [ 'lowlevel*', className ] })

def transform3(ds_orig):
    return transform(bfs(ds_orig), 'select', { 'descriptorNames': '*',
                                               'except': 'sfx*' })

def transform4(ds_orig):
    return transform(bfs(ds_orig), 'select', { 'descriptorNames': [ 'lowlevel*', className ],
                                               'except': [ 'barkbands.*', 'spectral_energyband*',
                                                            ] })

def transform5(ds_orig):
    ds = transform(bfs(ds_orig), 'fixlength')
    return transform(ds, 'gaussianize', { 'descriptorNames': 'lowlevel.*',
                                          'except': [ 'mfcc*', 'barkbands*', '*spectral_contrast*' ] })

def transform6(ds_orig):
    ds = transform(bfs(ds_orig), 'fixlength')
    ds = transform(ds, 'remove', { 'descriptorNames': [ 'barkbands*', 'sfx*' ] })
    return transform(ds, 'gaussianize', { 'descriptorNames': '*',
                                          'except': [ 'mfcc*', '*spectral_contrast*' ] })

def transform7(ds_orig):
    return transform(ds_orig, 'remove', { 'descriptorNames': [ '*hpcp*', '*silence_rate*' ] })

def transform8(ds_orig):
    return transform(ds_orig, 'select', { 'descriptorNames': [ '.'+className, '*.mean', '*.var', '*.dmean', '*.dvar', '*cov' ],
                                          'except': [ '*hpcp*', '*silence_rate*' ] })



if __name__ == '__main__':

    for eqloud in commonConfig['eqloud']:
        datasetName = datasetdir + className + '_%s.db' % eqloud

        if not(os.path.exists(datasetName)):
            print 'Dataset %s does not exist. Exiting.' % datasetName
            sys.exit(1)

        ds_orig = DataSet()
        ds_orig.load(datasetName)

        def dsname(i):
            return datasetName[:-3] + '.feat%d.db' % i

        # transform and save all datasets
        for i in commonConfig['featset']:
            if os.path.exists(dsname(i)):
                print 'Dataset already exists: %s' % dsname(i)
            else:
                exec('ds = transform%d(ds_orig)' % i)
                ds.save(dsname(i))

