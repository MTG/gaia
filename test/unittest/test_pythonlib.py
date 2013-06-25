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


class TestPythonLib(unittest.TestCase):

    def testCombinations(self):
        from gaia2.utils import combinations, dictcombinations

        choices = [ [ 1, 2 ],
                    [ 3, 4 ] ]

        combs = [ (1, 3), (1, 4), (2, 3), (2, 4) ]

        self.assertEqual(list(combinations(choices)), combs)

        choices = [ [ 'a', 'b', 'e' ],
                    [ 'c' ],
                    [ 4, 7 ] ]

        combs = [ ('a', 'c', 4), ('a', 'c', 7),
                  ('b', 'c', 4), ('b', 'c', 7),
                  ('e', 'c', 4), ('e', 'c', 7) ]

        self.assertEqual(list(combinations(choices)), combs)

        choices = { 'a': [ 1, 2, 3 ],
                    'b': [ 'frotz' ],
                    'c': [ 2.3, 4.7 ]
                    }

        combs = [ { 'a': 1, 'b': 'frotz', 'c': 2.3 },
                  { 'a': 1, 'b': 'frotz', 'c': 4.7 },
                  { 'a': 2, 'b': 'frotz', 'c': 2.3 },
                  { 'a': 2, 'b': 'frotz', 'c': 4.7 },
                  { 'a': 3, 'b': 'frotz', 'c': 2.3 },
                  { 'a': 3, 'b': 'frotz', 'c': 4.7 } ]

        for r in list(dictcombinations(choices)):
            self.assert_(r in combs)


suite = unittest.TestLoader().loadTestsFromTestCase(TestPythonLib)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
