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
