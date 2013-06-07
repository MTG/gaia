#!/usr/bin/env python

from gaia2 import *
import unittest
import testdata


class TestBindings(unittest.TestCase):

    def testPointLayoutCorrectlyWrapped(self):
        ds = DataSet()
        self.assertEqual(type(ds.layout()),
                         type(ds.originalLayout()))


    def testDescriptorComparison(self):
        self.assertEqual(RealDescriptor(3), 3.0)
        self.assertEqual(RealDescriptor([3, 4]), [ 3.0, 4 ])


    def testOwnershipIssues(self):
        # 1: layout of a temp point becomes invalid
        Point().layout().descriptorNames()

        # 2: sample point of a dataset becomes invalid
        ds = testdata.loadSmallDB()
        p = ds.samplePoint()
        del ds
        p.name()


suite = unittest.TestLoader().loadTestsFromTestCase(TestBindings)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
