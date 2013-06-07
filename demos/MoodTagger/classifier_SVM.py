#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import *

def train_SVM(dataset):
    # recreate a copy of the given dataset without history
    ds = dataset.copy()
    ds.forgetHistory()
    ds = transform(ds, 'remove', {'descriptorNames':['*metadata*','*mfcc*', '*hpcp*', '*tristimulus*', 'beats_position', 'bpm_estimates', 'bpm_intervals', 'onset_times', 'rubato_start', 'rubato_stop', 'chords_key']})
    ds = transform(ds, 'FixLength')
    ds = transform(ds, 'cleaner')
    ds = transform(ds, 'normalize', { 'independent': True })
    ds = transform(ds, 'enumerate', {'descriptorNames': ds.layout().descriptorNames(StringType), 'except':'emotion_tags'})
    ds = transform(ds, 'svmtrain', { 'className': 'emotion_tags' })
    h = ds.history()
    return h


