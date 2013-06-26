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
import gaia2.fastyaml as yaml
import os, glob, subprocess


class TestMerge(unittest.TestCase):

    def testValues(self):
        collection = yaml.load(open(testdata.TEST_DATABASE_FILES, 'r').read())

        # prepend 'data/' to the filenames
        for pid, filename in collection.items():
            collection[pid] = 'data/' + filename

        cvar.verbose = False
        ds = DataSet.mergeFiles(collection)
        cvar.verbose = True

        self.assertAlmostEqual(ds.point('Panic-The Smiths.mp3').value('danceability'),
                               0.5691167712)

        self.assertAlmostEqual(ds.point('11 Go.mp3').value('energy.mean'),
                               0.0231081359)

        self.assertAlmostEqual(ds.point('03 The Chopper [Shy FX Remix].mp3').value('chords_number_rate'),
                               0.0551007539)

        self.assertEqual(ds.point('08 I Can\'t Dance - Genesis.mp3').label('key_key'),
                         'D#')

        self.assertEqual(ds.point('06 Booo!.mp3').label('chords_mode'),
                         'major')

        ds.save(testdata.TEST_DATABASE)

    def testGaiaFusion(self):
        cmd = [ 'python', '../../src/tools/gaiafusion.py', '-c', '2', '-d', 'data/fusion', 'data/gaiatest.db' ]

        fusion = subprocess.Popen(cmd, stdout = subprocess.PIPE, stderr = subprocess.PIPE)
        stdout, stderr = fusion.communicate()

        # remove temporary data/gaiatest.db* file
        for f in glob.glob('data/gaiatest.db*'):
            os.remove(f)

        if fusion.returncode != 0:
            print 'STDOUT' + '*'*100
            print stdout
            print 'STDERR' + '*'*100
            print stderr

        self.assertEqual(fusion.returncode, 0)

    def testNamespaceClash(self):
        p = Point()
        p.load('data/namespace_clash.sig')


suite = unittest.TestLoader().loadTestsFromTestCase(TestMerge)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
