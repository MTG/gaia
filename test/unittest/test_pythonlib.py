#!/usr/bin/env python

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
