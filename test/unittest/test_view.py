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



from builtins import zip
from builtins import range
from gaia2 import *
import math
import unittest
import testdata


class TestView(unittest.TestCase):

    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def assertSearchResultEqual(self, r1, r2):
        self.assertEqual(r1[0], r2[0])
        self.assertAlmostEqual(r1[1], r2[1])


    def testSimple(self):
        ds = testdata.createSimpleDataSet()
        p2 = Point(ds.point('p')); p2.setName('p2')
        p3 = Point(ds.point('p')); p3.setName('p3')
        p3['a.1'] = 1
        p4 = Point(ds.point('p')); p4.setName('p4')
        p4['a.1'] = 1
        p4['a.2'] = 1
        ds.addPoint(p2)
        ds.addPoint(p3)
        ds.addPoint(p4)

        ds = transform(ds, 'fixlength')
        dist = MetricFactory.create('euclidean', ds.layout())
        v = View(ds)

        results = v.nnSearch('p', dist).get(10)
        self.assertEqual(results[0][1], 0.0)
        self.assertEqual(results[1][1], 0.0)
        self.assertSearchResultEqual(results[2], ('p3', 1.0))
        self.assertSearchResultEqual(results[3], ('p4', math.sqrt(2)))


    def testDeleteUnderlyingDataSet(self):
        ds = testdata.loadTestDB()

        params = { 'descriptorNames': [ '*.mean', '*.var' ] }

        ds = transform(ds, 'fixlength', params)
        ds = transform(ds, 'cleaner', params)
        ds = transform(ds, 'normalize', params)
        dist = MetricFactory.create('euclidean', ds.layout(), params)

        v = View(ds)
        del ds

        #self.assertRaises(Exception, v.nnSearch, '01 Respect.mp3')
        # this doesn't throw anymore, as the View keeps a ref to the dataset
        v.nnSearch('01 Respect.mp3', dist)


    def assertEqualSearchSpace(self, s1, s2):
        size = s1.size()
        self.assertEqual(s2.size(), size)

        res1 = s1.get(size)
        res2 = s2.get(size)

        try:
            from collections import defaultdict
        except ImportError:
            class defaultdict(dict):
                def __init__(self, ctor):
                    self._ctor = ctor
                def __getitem__(self, key):
                    if key not in list(self.keys()):
                        dict.__setitem__(self, key, self._ctor())
                    return dict.__getitem__(self, key)

        d1 = defaultdict(lambda: set())
        d2 = defaultdict(lambda: set())

        for ((pid1, dist1), (pid2, dist2)) in zip(res1, res2):
            d1[dist1].add(pid1)
            d2[dist2].add(pid2)

        self.assertEqual(d1, d2)



    def testSubspaceSearch(self):
        ds = testdata.loadTestDB()
        ds = transform(ds, 'fixlength')
        dist = MetricFactory.create('euclidean', ds.layout(), { 'descriptorNames': '*.mean' })
        v = View(ds)
        pid = 'Higher State of Consciousness.mp3'

        key_a_minor = v.nnSearch(pid, dist, 'WHERE label.key_key = "A" AND label.key_mode = "minor"')
        key_a =       v.nnSearch(pid, dist, 'WHERE label.key_key = "A"')
        key_minor =   v.nnSearch(pid, dist, 'WHERE label.key_mode = "minor"')

        key_a_minor_sspace1 = v.nnSearch(pid, key_minor, dist, 'WHERE label.key_key = "A"')
        key_a_minor_sspace2 = v.nnSearch(pid, key_a, dist, 'WHERE label.key_mode = "minor"')

        self.assertEqualSearchSpace(key_a_minor, key_a_minor_sspace1)
        self.assertEqualSearchSpace(key_a_minor, key_a_minor_sspace2)


    def testRegressionIndexing(self):
        ds = testdata.loadTestDB()
        ds = transform(ds, 'removevl')
        ds = transform(ds, 'fixlength')

        p = ds.samplePoint().name()
        p2 = 'Higher State of Consciousness.mp3'

        queries = [ (p, ''),
                    (p2, ''),
                    (p, 'WHERE (True AND True) and (true and TRUE)'),
                    (p, 'WHERE (false AND True) OR (true and false)'),
                    (p2, 'WHERE value.tempotap_bpm.value > 140'),
                    (p, 'WHERE true AND value.tempotap_bpm.value > 140'),
                    (p, 'WHERE value.tempotap_bpm > 110'),
                    (p, 'WHERE value.tempotap_bpm > -10'),
                    (p, 'WHERE value.tempotap_bpm < -10'),
                    (p, 'WHERE value.tempotap_bpm > 23000'),
                    (p, 'WHERE value.tempotap_bpm < 23000'),
                    (p, 'WHERE value.tempotap_bpm > 120 AND value.tempotap_bpm < 130'),
                    (p, 'WHERE value.tempotap_bpm BETWEEN 120 AND 130'),
                    (p, 'WHERE value.tempotap_bpm BETWEEN 130 AND 120'),
                    (p, 'WHERE value.tempotap_bpm BETWEEN 120 AND 120'),
                    (p, 'WHERE value.tempotap_bpm BETWEEN -2.3 AND 4096'),
                    (p, "WHERE value.tempotap_bpm BETWEEN -2.3 AND -1.4"),
                    (p, "WHERE value.tempotap_bpm BETWEEN 2048 AND 4096"),
                    (p, 'WHERE label.key_key = "C"'),
                    (p, 'WHERE label.key_key != "C"'),
                    (p, 'WHERE label.key_key = "X"'),
                    (p, 'WHERE label.key_key != "X"'),
                    (p, 'WHERE label.key_key != "C" AND label.key_mode != "major"'),
                    (p2, '''WHERE ((label.key_key = "A" AND label.key_mode = "major") OR
                                   (label.key_key = "E" AND label.key_mode = "minor"))
                                  AND value.tempotap_bpm < 90''' )
                    ]

        # test with standard views
        dist = MetricFactory.create('euclidean', ds.layout(), { 'descriptorNames': '*.mean' })
        v = View(ds)
        vidx = View(ds)
        vidx.indexOn('tempotap_bpm')
        vidx.indexOn('key_key')
        vidx.indexOn('key_mode')

        for (pt, filt) in queries:
            self.assertEqualSearchSpace(v.nnSearch(pt, dist, filt), vidx.nnSearch(pt, dist, filt))

        # test with frozen views
        dsr = transform(ds, 'select', { 'descriptorNames': ds.layout().descriptorNames(RealType) })
        dsnorm = transform(dsr, 'normalize')
        dspca = transform(dsnorm, 'pca', { 'resultName': 'pca', 'dimension': 25 })
        fds = FrozenDataSet()
        fds.fromDataSet(dspca)
        fds.setReferenceDataSet(ds)
        dist = FrozenDistanceFactory.create('Euclidean', fds.layout(), { 'descriptorName': 'pca' })

        v = FrozenView(fds)
        vidx = FrozenView(fds)
        vidx.indexOn('tempotap_bpm')
        vidx.indexOn('key_key')
        vidx.indexOn('key_mode')

        for (pt, filt) in queries:
            self.assertEqualSearchSpace(v.nnSearch(pt, dist, filt), vidx.nnSearch(pt, dist, filt))

    def testParsedVsConstructedFilters(self):
        ds = testdata.loadTestDB()
        ds = transform(ds, 'fixlength')

        p = ds.samplePoint()
        p2 = ds.point('Higher State of Consciousness.mp3')

        queries = [ (p, '', ''),
                    (p2, '', ''),
                    (p2, 'WHERE value.tempotap_bpm.value > 140', Filter('tempotap_bpm.value', '>', 140)),
                    (p, 'WHERE value.tempotap_bpm > 110', Filter('tempotap_bpm', '>', 110)),
                    (p, 'WHERE value.tempotap_bpm > -10', Filter('tempotap_bpm', '>', -10)),
                    (p, 'WHERE value.tempotap_bpm > 23000', Filter('tempotap_bpm', '>', 23000)),

                    (p, 'WHERE value.tempotap_bpm > 120 AND value.tempotap_bpm < 130',
                        AndFilter([ Filter('tempotap_bpm', '>', 120),
                                    Filter('tempotap_bpm', '<', 130) ])),

                    (p, 'WHERE value.tempotap_bpm BETWEEN 130 AND 120', Filter('tempotap_bpm', 'between', [ 130, 120 ])),
                    (p, 'WHERE label.key_key = "C"', Filter('key_key', '==', 'C')),

                    (p2, '''WHERE ((label.key_key = "A" AND label.key_mode = "major") OR
                                   (label.key_key = "E" AND label.key_mode = "minor"))
                                  AND value.tempotap_bpm < 90''',
                         AndFilter([
                             OrFilter([
                                 AndFilter([ Filter('key_key', '==', 'A'), Filter('key_mode', '==', 'major') ]),
                                 AndFilter([ Filter('key_key', '==', 'E'), Filter('key_mode', '==', 'minor') ])
                             ]),
                             Filter('tempotap_bpm', '<', 90)
                         ]) )
                    ]

        dist = MetricFactory.create('euclidean', ds.layout(), { 'descriptorNames': '*.mean' })
        v = View(ds)

        for (pt, filtstr, filt) in queries:
            self.assertEqualSearchSpace(v.nnSearch(pt, dist, filtstr), v.nnSearch(pt, dist, filt))


    def testCreatedInputSpace(self):
        ds = testdata.createSimpleDataSet()
        ds.point('p')['a.1'] = 23.0

        for i in range(5):
            p = Point()
            p.setName('p%d' % i)
            p.setLayout(ds.originalLayout())
            p['a.1'] = float(i)
            ds.addPoint(p)

        ds = transform(ds, 'fixlength')
        dist = MetricFactory.create('euclidean', ds.layout())
        v = View(ds)

        p = ds.point('p')

        RS_remove = v.nnSearch(p, dist)
        RS_remove.removePoints([ 'p2', 'p4' ])

        RS_create = InputSpace()
        RS_create.addPoints(ds, [ 'p', 'p0', 'p1', 'p3' ])

        rsc = v.nnSearch(p, RS_remove, dist)
        rsa = v.nnSearch(p, RS_create, dist)

        self.assertEqual((('p', 0.), ('p3', 20.), ('p1', 22.), ('p0', 23.)),
                         v.nnSearch(p, rsc, dist).get(10))

        self.assertEqual((('p', 0.), ('p3', 20.), ('p1', 22.), ('p0', 23.)),
                         v.nnSearch(p, rsa, dist).get(10))

        # test thresholdLimit method
        self.assertEqual((('p', 0.),),
                         v.nnSearch(p, rsa, dist).thresholdLimit(10).get(10))
        self.assertEqual((('p', 0.), ('p3', 20.)),
                         v.nnSearch(p, rsa, dist).thresholdLimit(20).get(10))
        self.assertEqual((('p', 0.), ('p3', 20.), ('p1', 22.)),
                         v.nnSearch(p, rsa, dist).thresholdLimit(22.01).get(10))


    def testEnumerate(self):
        testdata.useEnumerate = True
        self.testDeleteUnderlyingDataSet()
        self.testSubspaceSearch()
        self.testRegressionIndexing()
        self.testParsedVsConstructedFilters()
        self.testCreatedInputSpace()


suite = unittest.TestLoader().loadTestsFromTestCase(TestView)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
