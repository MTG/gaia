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
