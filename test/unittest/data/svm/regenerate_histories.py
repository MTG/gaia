#!/usr/bin/env python

from gaia2 import *
import os, yaml


ds = DataSet()
ds.load('../dataset_small.db')


# add genre information
ds = transform(ds, 'addfield', { 'string': 'genre' })
gt = {}
toremove = []

for root, dirs, files in os.walk('../dataset_small/'):
    for f in files:
        if not f.endswith('.sig'): continue

        genre = root.split('/')[-1]
        if genre == 'Sample':
            toremove += [ f[:-4] ]
        else:
            ds.point(f[:-4])['genre'] = genre

ds.removePoints(toremove)


ds = transform(ds, 'removevl')
ds = transform(ds, 'fixlength')
genre_ds = ds

# singledesc
ds = genre_ds
ds = transform(ds, 'normalize', { 'except': '*cov',
                                  'independent': False })
ds = transform(ds, 'svmtrain', { 'className': 'genre',
                                 'descriptorNames': 'spectral_centroid.mean' })

ds.history().save('test_svm_singledesc.history')

gtorig = yaml.load(open('../svm_old/test_svm_singledesc.gt.yaml'))
for p in ds.points():
    if str(p['genre']) != gtorig[p.name()]:
        print 'for point', p.name(), 'expected:', gtorig[p.name()], 'got:', p['genre']


# multidimdesc
ds = genre_ds
ds = transform(ds, 'normalize', { 'except': '*cov',
                                  'independent': False })
ds = transform(ds, 'svmtrain', {'className': 'genre',
                                'descriptorNames': 'mfcc.mean'})

ds.history().save('test_svm_multidimdesc.history')


# all
ds = genre_ds
ds = transform(ds, 'normalize', { 'except': '*cov',
                                  'independent': True,
                                  'outliers': 0.1 })
ds = transform(ds, 'svmtrain', { 'className': 'genre',
                                 'descriptorNames': '*.mean' })

ds.history().save('test_svm_all.history')


# probability
ds = genre_ds
ds = transform(ds, 'normalize', { 'except': '*cov',
                                  'outliers': 0.05 })
ds = transform(ds, 'svmtrain', { 'className': 'genre',
                                 'descriptorNames': '*.mean',
                                 'probability': True })

ds.history().save('test_svm_probability.history')

