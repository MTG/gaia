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
import tempfile
import os

class TestSerialization(unittest.TestCase):

    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def testLayout(self):
        layout = testdata.createSimpleLayout()

        (tmpFile, tmpName) = tempfile.mkstemp()
        os.close(tmpFile)

        # write dataset with layout
        p = Point()
        p.setName('p1')
        p.setLayout(layout)
        p2 = Point()
        p2.setName('p2')
        p2.setLayout(layout)

        p['a.1'] = 23
        self.assertEqual(p['a.1'], 23)
        self.assertRaises(Exception, p.setValue, 'a.4', 34)

        ds1 = DataSet()
        ds1.setName('ds1')
        ds1.addPoint(p)
        ds1.addPoint(p2)
        ds1.save(tmpName)

        # reload dataset
        ds2 = DataSet()
        ds2.load(tmpName)
        self.assertEqual(layout, ds2.layout())
        self.assertEqual(ds2.point('p1')['a.1'], 23)

        # remove temp file
        os.remove(tmpName)

    def testHistory(self):
        ds = testdata.loadTestDB()
        ignored_descs = testdata.TEST_DATABASE_VARLENGTH_REAL

        testdata.resetSettings()
        ds_orig = testdata.loadTestDB()

        # cleaning, mandatory step
        ds = transform(ds, 'fixlength', { 'except': ignored_descs })
        cleaned_db = transform(ds, 'cleaner', { 'except': ignored_descs })

        # removing annoying descriptors, like mfcc.cov & mfcc.icov, who don't
        # like to be normalized like the other ones (constant value: dimension)
        no_mfcc_db = transform(cleaned_db, 'remove', { 'descriptorNames': '*mfcc*' })

        # normalize, to have everyone change values
        normalized_db = transform(no_mfcc_db, 'normalize', { 'except': ignored_descs })

        testPoints = [ '01 Oye Como Va - Santana.mp3',
                       '02 Carmen Burana- O Fortuna.mp3',
                       '07 Romeo and Juliet- the Knights\' Dance.mp3',
                       '11 Lambada.mp3' ]

        for pointName in testPoints:
            p1 = normalized_db.point(pointName)
            p2 = normalized_db.history().mapPoint(ds_orig.point(pointName))

            for name in p1.layout().descriptorNames():
                self.assertEqual(p1[name],
                                 p2[name])


        (tmpFile, tmpName) = tempfile.mkstemp()
        os.close(tmpFile)
        normalized_db.save(tmpName)
        reloaded_db = DataSet()
        reloaded_db.load(tmpName)

        for pointName in testPoints:
            p1 = normalized_db.point(pointName)
            p2 = normalized_db.history().mapPoint(ds_orig.point(pointName))
            p3 = reloaded_db.point(pointName)
            p4 = reloaded_db.history().mapPoint(ds_orig.point(pointName))

            self.assert_(p1.layout() == p2.layout())
            self.assert_(p2.layout() == p3.layout())
            self.assert_(p3.layout() == p4.layout())

            for name in p1.layout().descriptorNames():
                self.assertEqual(p1[name], p2[name])
                self.assertEqual(p2[name], p3[name])
                self.assertEqual(p3[name], p4[name])

        # remove temp file
        os.remove(tmpName)


    def testValueQuoting(self):
        labelPoints = [ 'a: label',
                        'a: "label"',
                        'a: [ label ]',
                        'a: [ "label" ]'
                        ]

        valueLPoints = [ 'a: "123"',
                         'a: [ "123" ]'
                         ]

        valuePoints = [ 'a: 123',
                        'a: [ 123 ]'
                        ]

        p = Point()
        for lp in labelPoints:
            p.loadFromString(lp)
            self.assertEqual(isinstance(p['a'], str), True)
            self.assertEqual(p['a'], 'label')

        for lp in valueLPoints:
            p.loadFromString(lp)
            self.assertEqual(isinstance(p['a'], str), True)
            self.assertEqual(p['a'], '123')

        for lp in valuePoints:
            p.loadFromString(lp)
            self.assertEqual(isinstance(p['a'], float), True)
            self.assertEqual(p['a'], 123)


    def testFixLength(self):
        testdata.useFixedLength = True
        self.testLayout()
        self.testHistory()

    def testEnumerate(self):
        testdata.useEnumerate = True
        self.testLayout()
        self.testHistory()

    def testEnumerateFixLength(self):
        testdata.useEnumerate = True
        self.testFixLength()



suite = unittest.TestLoader().loadTestsFromTestCase(TestSerialization)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
