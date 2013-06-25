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
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see http://www.gnu.org/licenses/



from gaia2 import *
import unittest
import testdata
import os, tempfile


class TestDataSet(unittest.TestCase):

    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def testDoubleLoad(self):
        ds1 = DataSet()
        ds1.load(testdata.TEST_DATABASE)
        ds1.load(testdata.TEST_DATABASE)

        ds2 = DataSet()
        ds2.load(testdata.TEST_DATABASE)

        self.assertEqual(ds1.size(), ds2.size())
        for p1, p2 in zip(ds1.points(), ds2.points()):
            self.assertEqual(p1, p2)

    def testLinkedDataSets(self):
        d = testdata.createSimpleDataSet()
        d2 = transform(d, 'select', { 'descriptorNames': [ 'a*', 'b*' ] })

        d2.setReferenceDataSet(d)

        p = Point()
        p.setName('ptest')
        p.setLayout(d.originalLayout())
        p['a.1'] = 23

        d.addPoint(p)

        self.assertEqual(d2.point('ptest').value('a.1'), 23)
        self.assertRaises(Exception, d2.point('ptest').value, 'c')

        d2.removePoint('ptest')
        self.assertRaises(Exception, d.point, 'ptest')

    def testDeleteLinkedDataSets(self):
        ds = testdata.createSimpleDataSet()
        ds2 = transform(ds, 'select', { 'descriptorNames': [ 'a*', 'b*' ] })

        ds2.setReferenceDataSet(ds)

        del ds2
        del ds

        ds = testdata.createSimpleDataSet()
        ds2 = transform(ds, 'select', { 'descriptorNames': [ 'a*', 'b*' ] })

        ds2.setReferenceDataSet(ds)

        del ds
        del ds2



    def testLinkedViews(self):
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

        query = Point(ds.point('p'))
        query['a.1'] = 23.0

        self.assertEqual((('p', 0.), ('p4', 19.), ('p3', 20.), ('p2', 21.)),
                         v.nnSearch(query, dist).get(4))

        ds.removePoint('p')
        self.assertEqual((('p4', 19.), ('p3', 20.), ('p2', 21.), ('p1', 22.)),
                         v.nnSearch(query, dist).get(4))

        ds.removePoint('p3')
        self.assertEqual((('p4', 19.), ('p2', 21.), ('p1', 22.), ('p0', 23.)),
                         v.nnSearch(query, dist).get(4))

        # get original query point to add in dataset
        testdata.resetSettings()
        dsq = testdata.createSimpleDataSet()
        orig_query = Point(dsq.point('p'))
        orig_query['a.1'] = 23.0

        ds.addPoint(orig_query)
        self.assertEqual((('p', 0.), ('p4', 19.), ('p2', 21.), ('p1', 22.)),
                         v.nnSearch(query, dist).get(4))


    def testHistory(self):
        # add a point in a transformed dataset
        d = testdata.createSimpleDataSet()
        ds = transform(d, 'select', { 'descriptorNames': [ 'a*', 'b*' ] })

        p = Point()
        p.setName('ptest')
        p.setLayout(testdata.createSimpleLayout())

        ds.addPoint(p)

    def testCollectionMultipleAdd(self):
        d = testdata.loadTestDB()
        d1 = DataSet()
        d2 = DataSet()
        d3 = DataSet()

        # add them one by one
        for p in d.points():
            d1.addPoint(p)

        # add them all at once
        d2.addPoints(d)

        # add them all at once using a list of points
        d3.addPoints([ p for p in d.points() ])

        self.assertEquals(d1.size(), d2.size())
        for p1, p2 in zip(d1.points(), d2.points()):
            self.assertEqual(p1, p2)

        self.assertEquals(d1.size(), d3.size())
        for p1, p3 in zip(d1.points(), d3.points()):
            self.assertEqual(p1, p3)

    def createSequentialDataSet(self):
        # used in testRemovePoints
        layout = testdata.createSimpleLayout()
        ds = DataSet()
        p = Point()
        p.setLayout(layout)
        for name in [ '0', '1', '2', '3', '4', '5' ]:
            p.setName(name)
            ds.addPoint(p)

        return ds

    def testCollectionRemovePoints(self):
        ds = self.createSequentialDataSet()
        ds.removePoints([])
        self.assertEquals(ds[0].name(), '0')
        self.assertEquals(ds[1].name(), '1')
        self.assertEquals(ds[2].name(), '2')
        self.assertEquals(ds[3].name(), '3')
        self.assertEquals(ds[4].name(), '4')
        self.assertEquals(ds[5].name(), '5')

        ds = self.createSequentialDataSet()
        ds.removePoints([ '3', '4', '5' ])
        self.assertEquals(ds[0].name(), '0')
        self.assertEquals(ds[1].name(), '1')
        self.assertEquals(ds[2].name(), '2')

        ds = self.createSequentialDataSet()
        ds.removePoints([ '1', '2' ])
        self.assertEquals(ds[0].name(), '0')
        self.assertEquals(ds[1].name(), '3')
        self.assertEquals(ds[2].name(), '4')
        self.assertEquals(ds[3].name(), '5')

        ds = self.createSequentialDataSet()
        ds.removePoints([ '1', '3' ])
        self.assertEquals(ds[0].name(), '0')
        self.assertEquals(ds[1].name(), '2')
        self.assertEquals(ds[2].name(), '4')
        self.assertEquals(ds[3].name(), '5')

        ds = self.createSequentialDataSet()
        ds.removePoints([ '2', '5' ])
        self.assertEquals(ds[0].name(), '0')
        self.assertEquals(ds[1].name(), '1')
        self.assertEquals(ds[2].name(), '3')
        self.assertEquals(ds[3].name(), '4')


    def testAppendDataSet(self):
        ds = testdata.loadTestDB()
        ds2 = self.createSequentialDataSet()

        # check homogeneous collections
        self.assertRaises(Exception, ds.appendDataSet, ds2)

        # check no duplicates
        self.assertRaises(Exception, ds2.appendDataSet, ds2)

        # check normal operation of appendCollection
        ppoints = [ Point(p) for p in ds2.points() ]
        for p in ppoints:
            p.setName('p' + p.name())

        ds3 = DataSet()
        ds3.addPoints(ppoints)
        ds3.appendDataSet(ds2)

        for p in ds3.points():
            if p.name()[0] == 'p':
                self.assert_(p == ds3.point(p.name()[1:]))
            else:
                self.assert_(p == ds3.point('p' + p.name()))

    def testAppendDataSetWithEnums(self):
        ds = DataSet()
        ds.setReferenceDataSet(None)
        ds.load('data/pv.yml.124.ds.essentia1.0.6_ref.db')

        enumValues = {}
        enumDescs = ds.layout().descriptorNames(EnumType)

        for p in ds.points():
            for desc in enumDescs:
                enumValues[(p.name(), desc)] = p[desc]

        ds2 = DataSet()
        ds2.load('data/pv.yml.125.ds.essentia1.0.6_ref.db')

        for p in ds2.points():
            for desc in enumDescs:
                enumValues[(p.name(), desc)] = p[desc]

        ds.appendDataSet(ds2)

        # assert all values in enum values are ok
        for (pname, desc), value in enumValues.items():
            self.assertEqual(value, ds.point(pname)[desc])

        ds3 = DataSet()
        ds3.load('data/pv.yml.126.ds.essentia1.0.6_ref.db')

        for p in ds3.points():
            for desc in enumDescs:
                enumValues[(p.name(), desc)] = p[desc]

        ds.appendDataSet(ds3)

        # check all values are all still the same
        for (pname, desc), value in enumValues.items():
            self.assertEqual(value, ds.point(pname)[desc])

        (tmpFile, tmpName) = tempfile.mkstemp()
        os.close(tmpFile)
        ds.save(tmpName)
        ds.load(tmpName)

        for (pname, desc), value in enumValues.items():
            self.assertEqual(value, ds.point(pname)[desc])

        os.remove(tmpName)


    def testFixLength(self):
        testdata.useFixedLength = True
        self.testLinkedDataSets()
        self.testLinkedViews()
        self.testHistory()
        self.testCollectionMultipleAdd()

    def testEnumerate(self):
        testdata.useEnumerate = True
        self.testLinkedDataSets()
        self.testLinkedViews()
        self.testHistory()
        self.testCollectionMultipleAdd()

    def testEnumerateFixLength(self):
        testdata.useEnumerate = True
        self.testFixLength()


suite = unittest.TestLoader().loadTestsFromTestCase(TestDataSet)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
