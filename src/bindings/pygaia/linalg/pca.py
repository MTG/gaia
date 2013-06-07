#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import DataSet, Point, PointLayout, RealType, transform


def PCA(x):
    points = []
    layout = PointLayout()
    layout.add('x', RealType)

    for i, l in enumerate(x):
        p = Point()
        p.setName('p%d' % i)
        p.setLayout(layout)
        p['x'] = l
        points.append(p)

    ds = DataSet()
    ds.addPoints(points)

    ds = transform(ds, 'fixlength')
    ds = transform(ds, 'pca', { 'dimension': len(x[0]), 'resultName': 'pca' })

    result = []
    for p in ds.points():
        result.append(p['pca'])

    return result
