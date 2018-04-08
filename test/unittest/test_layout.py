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



from gaia2 import cvar
from gaia2 import RealType
from gaia2 import StringType
from gaia2 import VariableLength
from gaia2 import PointLayout
from gaia2 import UndefinedType
from gaia2 import DataSet
from gaia2 import Point
from gaia2 import mergeLayouts 
from gaia2 import transform
import unittest2
import testdata

class TestLayout(unittest2.TestCase):

    def setUp(self):
        cvar.verbose = False
        self.l1 = testdata.createSimpleLayout()

    def testCorrectOrdering(self):
        self.assertEqual(self.l1.descriptorLocation('a.1').index(), 0)
        self.assertEqual(self.l1.descriptorLocation('a.2').index(), 1)
        self.assertEqual(self.l1.descriptorLocation('b').index(), 2)
        self.assertEqual(self.l1.descriptorLocation('c').index(), 3)

    def testIndexThrows(self):
        self.assertRaises(Exception, self.l1.descriptorLocation('a').index)

    def testCopyRemove(self):
        l2 = self.l1.copy()
        l3 = l2

        l2.remove('b')

        self.assertEqual(self.l1.regionSize(RealType, VariableLength), 4)
        self.assertEqual(l2.regionSize(RealType, VariableLength), 3)
        self.assertEqual(l3.regionSize(RealType, VariableLength), 3)

        self.assertEqual(self.l1.regionSize(StringType, VariableLength), 1)

        l2.remove('a')

        self.assertEqual(self.l1.regionSize(RealType, VariableLength), 4)
        self.assertEqual(l2.regionSize(RealType, VariableLength), 1)
        self.assertEqual(l3.regionSize(RealType, VariableLength), 1)

    def testCorrectNodes(self):
        l = PointLayout()
        self.assertRaises(Exception, l.add, 'undef')

        l.add('blah', RealType)
        l.add('', 'blah', StringType)

        self.assertEqual(len(l.descriptorNames()), 1)
        self.assertEqual(l.descriptorLocation('blah').type(), RealType)

        l.add('blouh', StringType)
        self.assertEqual(len(l.descriptorNames()), 2)
        self.assertRaises(Exception, l.add, 'blouh', 'bluh', UndefinedType)
        self.assertEqual(len(l.descriptorNames()), 2)

    def testMerge(self):
        l2 = testdata.createSimpleLayout()
        self.assertEqual(self.l1, l2)
        self.assertRaises(Exception, mergeLayouts, self.l1, l2)

        l3 = PointLayout()
        l3.add('a', RealType)
        l3.add('a', '3', RealType)

        l4 = mergeLayouts(l2, l3)

        self.assertEqual(len(l4.descriptorNames()), len(l2.descriptorNames())+1)
        self.assertEqual(l4.descriptorLocation('a').size(RealType, VariableLength), 3)


    def testBasicReferenceCounting(self):
        l1 = PointLayout()
        self.assertEqual(l1.ref(), 1)

        # add python ref
        l2 = l1
        self.assertEqual(l1.ref(), 1)
        self.assertEqual(l2.ref(), 1)

        # add C++ ref
        l3 = PointLayout(l2)
        self.assertEqual(l1.ref(), 2)
        self.assertEqual(l2.ref(), 2)
        self.assertEqual(l3.ref(), 2)

        # make different copy
        l4 = l3.copy()
        self.assertEqual(l4.ref(), 1)

        # remove C++ ref
        del l3
        self.assertEqual(l1.ref(), 1)
        self.assertEqual(l2.ref(), 1)

        # add C++ ref
        l3 = PointLayout(l2)
        # remove python ref
        del l1
        self.assertEqual(l2.ref(), 2)
        self.assertEqual(l3.ref(), 2)

        # remove last python ref, hence remove C++ ref too
        del l2
        self.assertEqual(l3.ref(), 1)

    def testComplexReferenceCounting(self):
        ds = DataSet()
        self.assertEqual(ds.layout().ref(), 2) # 1 + 1 from temp object

        p = Point()
        p.setName('p1')
        lext = PointLayout(p.layout()) # +1, {lext,p}.ref = 2
        self.assertEqual(lext.ref(), 2)

        lext = p.layout().copy() # copy, lext.ref = 1; p.ref -= 1, = 1
        self.assertEqual(lext.ref(), 1)

        ds.addPoint(p) # +3 (dataset + pointcopy), ref = 3

        self.assertEqual(lext.ref(), 1)
        self.assertEqual(ds.layout().ref(), 4) # 3 + 1 temp object

        p2 = Point(p) # +1, {p,p2}.ref = 5
        p2.setName('p2')
        self.assertEqual(ds.layout().ref(), 5)
        ds.addPoint(p2)
        self.assertEqual(ds.layout().ref(), 6) # +1 pointcopy, ref = 6


    def testAddToDataSetWithDifferentLayouts(self):
        p1 = Point()
        p1.setLayout(self.l1) # +1, ref = 2
        p2 = Point()

        ds = DataSet()
        ds.addPoint(p1) # +2 (dataset+pointcopy), ref = 4
        self.assertRaises(Exception, ds.addPoint, p2)
        self.assertEqual(p1.layout().ref(), 4)
        self.assertEqual(p2.layout().ref(), 1)


    def testSecondChanceForLayoutEquality(self):
        '''ticket #21: points try to morph to adapt to dataset if they cannot be naturally inserted'''
        ds = DataSet()
        p = Point()

        p.setName('Paris Hilton')
        p.load('data/04 - Cansei de Ser Sexy - Meeting Paris Hilton.mp3.sig')
        ds.addPoint(p)

        p.setName('2005')
        p.load('data/11_2005-fwyh.mp3.sig')
        ds.addPoint(p)

        self.assertEqual(ds.point('2005')['title'], '2005')


    def testMergePointsWithDifferentEnumerationMaps(self):
        '''ticket #74: when changing the layout of a point, we must also make sure that the enum maps are correctly mapped'''

        p1 = Point()
        p1.setName('p1')
        p1.setLayout(self.l1)
        p1['d'] = 'hello'

        p2 = Point()
        p2.setName('p2')
        p2.setLayout(self.l1)
        p2['d'] = 'world'

        ds = DataSet()
        ds.addPoint(p1)
        ds.addPoint(p2)

        self.assertEqual(ds.point('p1').label('d'), 'hello')
        self.assertEqual(ds.point('p2').label('d'), 'world')

        ds.removePoint('p2')
        ds = transform(ds, 'enumerate', { 'descriptorNames': 'd' })
        ds.addPoint(p2)

        self.assertEqual(ds.point('p1').label('d'), 'hello')
        self.assertEqual(ds.point('p2').label('d'), 'world')

    def testChronoIndependence(self):
        '''Layouts built using the same descriptors but in a different order should still be equal'''
        l = PointLayout()
        l.add('a', RealType)
        l.add('b', RealType)

        l2 = PointLayout()
        l2.add('b', RealType)
        l2.add('a', RealType)

        self.assert_(l == l2)


    def testIntersect(self):
        l1 = PointLayout()
        l1.add('a', RealType)
        l1.add('b', RealType)

        l2 = PointLayout()
        l2.add('a', RealType)
        l2.add('b', StringType)
        l2.add('d', RealType)

        lr = l1 & l2

        self.assertEqual(lr.descriptorNames(), ('.a',))

        self.assertEqual(l1.descriptorNames(), (l1 & l1).descriptorNames())
        self.assertEqual(l2.descriptorNames(), (l2 & l2).descriptorNames())

    def testLibyaml1024CharLimit(self):
        l = PointLayout()
        l.add('a'*2000, RealType)

        from gaia2 import fastyaml as yaml
        yaml.load(l.toYaml())

    def tearDown(self):
        del self.l1

suite = unittest2.TestLoader().loadTestsFromTestCase(TestLayout)

if __name__ == '__main__':
    unittest2.TextTestRunner(verbosity=2).run(suite)
