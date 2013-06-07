#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import transform, RealType, pmap, mergeDataSets


def addRCA(ds, groundTruth, dim, selectConfig = {}):
    #ds_rca = transform(ds, 'fixlength') # should be unnecessary
    ds_rca = ds
    if selectConfig:
        ds_rca = transform(ds_rca, 'select', selectConfig)

    ds_rca = transform(ds_rca, 'gaussianize')

    # if dimension is too high, we need to preprocess before with a PCA, otherwise RCA doesn't work
    l = ds_rca.layout()
    descdim = l.descriptorLocation(l.descriptorNames()).dimension(RealType)
    if descdim > 80:
        ds_rca = transform(ds_rca, 'pca', { 'resultName': 'pca%d' % 80,
                                            'dimension': 80 })

    ds_rca = transform(ds_rca, 'rca', { 'resultName': 'rca%d' % dim,
                                        'dimension': dim,
                                        'classMap': pmap(groundTruth) })

    return mergeDataSets(ds, ds_rca)
