#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
#
# This file is part of Gaia
#
# Gaia is free software: you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation (FSF), either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the Affero GNU General Public License     
# version 3 along with this program. If not, see http://www.gnu.org/licenses/



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


