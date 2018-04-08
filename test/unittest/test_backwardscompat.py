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



from builtins import range
from gaia2 import cvar
from gaia2 import DataSet
from gaia2 import TransfoChain
from gaia2 import MetricFactory
from gaia2 import View
from gaia2 import transform
import unittest2
import testdata

class TestBackwardsCompatibility(unittest2.TestCase):

    def setUp(self):
        cvar.verbose = False


    def compareResults(self, r1, r2):
        self.assertEqual(len(r1), len(r2))
        for i in range(len(r1)):
            self.assertEqual(r1[i][0], r2[i][0])
            self.assertAlmostEqual(r1[i][1], r2[i][1], 5)


    def testDataSet(self):
        # load 2.0 dataset, check some values are correct
        ds = DataSet()
        ds.load(testdata.GAIA_20_BACKWARDS_COMPAT_DATASET)

        self.assertAlmostEqual(ds.point('01 Message - Grandmaster Flash.mp3').value('tempotap_bpm'),
            101.05792999)

        self.assertEqual(ds.point('04 Blue Skies.mp3').label('key_key'),
            'G#')

    def testHistory(self):
        # load 2.0 history, check some transfos are correct
        h = TransfoChain()

        self.assertRaises(Exception, h.load, testdata.GAIA_20_BACKWARDS_COMPAT_HISTORY)
        return

        h.load(testdata.GAIA_20_BACKWARDS_COMPAT_HISTORY)

        self.assertEqual(h.size(), 3)

        self.assertEqual(h[1].toPython()['Analyzer parameters'],
                         {'descriptorNames': ['*.mean', '*.var']})

        self.assertAlmostEqual(h[1].toPython()['Applier parameters']['coeffs']['.energy.mean']['a'][0],
                               17.619043350219727)

        self.assertAlmostEqual(h[1].toPython()['Applier parameters']['coeffs']['.energy.mean']['b'][0],
                               -0.1723092645406723)


    def testDoubleLoadMixedVersions(self):
        ds = DataSet()
        ds.load(testdata.TEST_DATABASE)
        ds.load(testdata.GAIA_20_BACKWARDS_COMPAT_DATASET)


    def testComplete(self):
        # load 2.0 dataset, history, apply history to dataset
        # check nn-search results are the same as the ones we get when doing it from gaia 2.0
        ds = DataSet()
        ds.load(testdata.GAIA_20_BACKWARDS_COMPAT_DATASET)

        h = TransfoChain()

        self.assertRaises(Exception, h.load, testdata.GAIA_20_BACKWARDS_COMPAT_HISTORY)
        return

        h.load(testdata.GAIA_20_BACKWARDS_COMPAT_HISTORY)

        ds = h.mapDataSet(ds)
        v = View(ds)
        dist = MetricFactory.create('euclidean', ds.layout())

        results = v.nnSearch('01 Respect.mp3', dist).get(5)
        self.compareResults(results, testdata.GAIA_20_BACKWARDS_COMPAT_RESULTS)

        ds21 = DataSet()
        ds21.load(testdata.TEST_DATABASE)
        results = v.nnSearch(h.mapPoint(ds21.point('01 Respect.mp3')), dist).get(5)
        self.compareResults(results, testdata.GAIA_20_BACKWARDS_COMPAT_RESULTS)


    def testComplete2(self):
        # have a transformed 2.0 dataset, load it, and have gaia 2.1 transform
        # a point using the history.
        ds = DataSet()

        self.assertRaises(Exception, ds.load, testdata.GAIA_20_BACKWARDS_COMPAT_PCA_DATASET)
        return

        ds.load(testdata.GAIA_20_BACKWARDS_COMPAT_PCA_DATASET)

        ds21 = DataSet()
        ds21.load(testdata.TEST_DATABASE)
        p = ds21.point("17 Blue Monday ['88 12' Version].mp3")

        ds21 = ds.history().mapDataSet(ds21)

        self.assertEqual(ds.history().mapPoint(p),
                         ds21.history().mapPoint(p))

        ds = transform(ds, 'fixlength')
        ds21 = transform(ds21, 'fixlength')

        def search(ds, p):
            p = ds.history().mapPoint(p)
            dist = MetricFactory.create('euclidean', ds.layout())
            return View(ds).nnSearch(p, dist).get(5)

        self.compareResults(search(ds, p),
                            search(ds21, p))

suite = unittest2.TestLoader().loadTestsFromTestCase(TestBackwardsCompatibility)

if __name__ == '__main__':
    unittest2.TextTestRunner(verbosity=2).run(suite)
