#!/usr/bin/env python

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



from builtins import str
from builtins import zip
from builtins import range
from gaia2 import *
import math
import unittest
import testdata


class TestFrozenSearch(unittest.TestCase):

    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def loadPreparedTestDB(self):
        ds = testdata.loadTestDB()
        ds = transform(ds, 'removevl')
        ds = transform(ds, 'fixlength')
        ds = transform(ds, 'remove', { 'descriptorNames': ds.layout().descriptorNames(StringType) })

        return ds

    def loadFrozenTestDB(self):
        ds = self.loadPreparedTestDB()

        fds = FrozenDataSet()
        fds.fromDataSet(ds)

        return fds


    def assertEqualPoint(self, p, fp):
        j = 0
        for descname in p.layout().descriptorNames():
            desc = p[descname]
            if not isinstance(desc, tuple):
                desc = (desc,)

            for dim in desc:
                self.assertAlmostEqual(fp[j], dim)
                j += 1


    def testLoadDataSet(self):
        ds = self.loadPreparedTestDB()
        fds = self.loadFrozenTestDB()

        self.assertEqual(fds.size(), ds.size())
        for i in range(ds.size()):
            self.assertEqualPoint(ds[i], fds[i])


    def testSinglePointConversion(self):
        ds = self.loadPreparedTestDB()

        p = ds[0]
        fp = FrozenPoint.fromPoint(p)

        self.assertEqual(str(ds.layout()), str(fp.layout()))

        self.assertEqualPoint(p, fp)


    def testAPICompatibility(self):
        fds = self.loadFrozenTestDB()

        p = fds[1]

        self.assertAlmostEqual(p['spectral_kurtosis.mean'][0], 10.212173461914062)

        p2 = FrozenPoint.fromBase64(p.toBase64())

        self.assertEqual(p, p2)
        self.assertEqual(p.layout(), p2.layout())



    def testSimpleSearch(self):
        ds = self.loadPreparedTestDB()
        ds = transform(ds, 'normalize')
        #ds = transform(ds, 'pca', { 'resultName': 'pca', 'dimension': 25 })

        fds = FrozenDataSet()
        fds.fromDataSet(ds)

        pidx = 23
        nresults = 10

        # test euclidean distance
        dist = MetricFactory.create('Euclidean', ds.layout())
        v = View(ds)

        expected = v.nnSearch(ds[pidx], dist).get(nresults)

        fv = FrozenView(fds)
        fdist = FrozenMetricFactory.create('Euclidean', fds.layout()) #, { 'descriptorName': 'pca' })
        found = fv.nnSearch(fds[pidx], fdist).get(nresults)

        (names1, names2), (dists1, dists2) = list(zip(list(zip(*expected)), list(zip(*found))))
        self.assertEqual(names1, names2)
        for d1, d2 in zip(dists1, dists2):
            self.assertAlmostEqual(d1, d2, 3)

        # test cosine angle distance
        pidx = 54

        dist = MetricFactory.create('CosineAngle', ds.layout())
        expected = v.nnSearch(ds[pidx], dist).get(nresults)

        fdist = FrozenMetricFactory.create('CosineAngle', fds.layout()) #, { 'descriptorName': 'pca' })
        found = fv.nnSearch(fds[pidx], fdist).get( nresults)

        (names1, names2), (dists1, dists2) = list(zip(list(zip(*expected)), list(zip(*found))))
        self.assertEqual(names1, names2)
        for d1, d2 in zip(dists1, dists2):
            self.assertAlmostEqual(d1, d2, 3)



suite = unittest.TestLoader().loadTestsFromTestCase(TestFrozenSearch)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
