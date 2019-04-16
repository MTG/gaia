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


from math import sqrt
import unittest
from gaia2.classification import ConfusionMatrix


class TestConfusionMatrix(unittest.TestCase):

    def populateFold(self, cm, a_as_a, a_as_b, b_as_b, b_as_a, fold=0):
        for i in range(a_as_a):
            cm.addNfold(0, 0, 'aa_{}_{}'.format(i, fold), fold)

        for i in range(a_as_b):
            cm.addNfold(0, 1, 'ab_{}_{}'.format(i, fold), fold)

        for i in range(b_as_a):
            cm.addNfold(1, 0, 'ba_{}_{}'.format(i, fold), fold)

        for i in range(b_as_b):
            cm.addNfold(1, 1, 'bb_{}_{}'.format(i, fold), fold)

        return cm

    def testAccuraciesNFold(self):
        cm = ConfusionMatrix()

        # Fold 0 with acc = 100%
        cm = self.populateFold(cm, 1, 0, 1, 0, fold=0)

        # Fold 1 with acc = 0%
        cm = self.populateFold(cm, 0, 1, 0, 1, fold=1)

        # Resulting accuracy should be the average
        self.assertEqual(cm.accuracy(), 50.0)

    def testNormalizedAccuracy(self):
        cm = ConfusionMatrix()

        # Consider the following confussion matrix,
        #      A   B
        #  A | 3 | 1 |
        #  B | 6 | 2 |
        # raw accuracy is 5 / 12 ~= 41.67%
        # However, normalized accuracy divides each
        # class contrubion by the class population:
        # A accuracy: 3 / 4, B accuracy: 2 / 8
        # normalized accuracy is 50%
        cm = self.populateFold(cm, 3, 1, 2, 6, fold=0)

        self.assertEqual(cm.accuracy(),  100 * 5. / 12)
        self.assertEqual(cm.normalizedAccuracy(), 50.0)

    def testStdNfold(self):
        cm = ConfusionMatrix()

        # Fold 0 with acc = 0%
        cm = self.populateFold(cm, 0, 1, 0, 1, fold=0)

        # Fold 1 with acc = 10%
        cm = self.populateFold(cm, 1, 9, 1, 9, fold=1)

        # Fold 2 with acc = 20%
        cm = self.populateFold(cm, 2, 8, 2, 8, fold=2)

        nfolds = 3.
        values = [-10. * -10., 0., 10. * 10.]  # each fold contribution to the
                                               # std after substractig the mean
        analitic_std = sqrt(sum(values) / nfolds)

        self.assertEqual(cm.stdNfold(), analitic_std)

    def testStdNfoldNormalizedAccuracies(self):
        # Same test considering normalized accuracies
        cm = ConfusionMatrix()

        # Fold 0 with normalized acc = 0%
        cm = self.populateFold(cm, 0, 1, 0, 1, fold=0)

        # Fold 1 with normalized acc = 10%
        cm = self.populateFold(cm, 4, 16, 0, 10, fold=1)

        # Fold 2 with normalized acc = 20%
        cm = self.populateFold(cm, 2, 8, 2, 8, fold=2)
        # resulting accuracy should be the average
        nfolds = 3.
        values = [-10. * -10., 0., 10. * 10.]  # each fold contribution to the
                                               # std after substractig the mean
        analitic_std = sqrt(sum(values) / nfolds)

        self.assertEqual(cm.stdNfold(normalizedAccuracies=True), analitic_std)

        # Also make sure that this test does not work without the normalization
        self.assertNotEqual(cm.stdNfold(normalizedAccuracies=False), analitic_std)


suite = unittest.TestLoader().loadTestsFromTestCase(TestConfusionMatrix)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
