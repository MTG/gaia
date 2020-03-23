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


import os
import shutil
import tempfile
import unittest

import gaia2.fastyaml as yaml
from gaia2.scripts.classification.generate_classification_project import generate_project


class TestGenerateClassificationProject(unittest.TestCase):
    def check_project(self, groundtruth_file, filelist_file, expected, force_consistency=False):
        tmp_dir = tempfile.mkdtemp()

        project_file = os.path.join(tmp_dir, expected)

        generate_project(groundtruth_file,
                         filelist_file,
                         project_file,
                         tmp_dir,
                         tmp_dir,
                         force_consistency=force_consistency)

        found = yaml.load(open(project_file, 'r'))['templateVersion']

        shutil.rmtree(tmp_dir)

        # Check that te script picked the correct templateVersion.
        self.assertEqual(expected, found)

    def test21beta2(self):
        groundtruth_file = 'data/classification_projects/grountruth.yaml'
        filelist_file = 'data/classification_projects/21beta2filelist.yaml'
        expected = '2.1-beta2'

        self.check_project(groundtruth_file, filelist_file, expected)

    def test21beta5(self):
        groundtruth_file = 'data/classification_projects/grountruth.yaml'
        filelist_file = 'data/classification_projects/21beta5filelist.yaml'
        expected = '2.1-beta5'

        self.check_project(groundtruth_file, filelist_file, expected)

    def testMixedFiles(self):
        groundtruth_file = 'data/classification_projects/grountruth.yaml'
        filelist_file = 'data/classification_projects/mixed_filelist.yaml'
        expected = '2.1-beta2'

        # If there are descriptor files with different versions should try
        # 2.1-beta2 by default.
        self.check_project(groundtruth_file, filelist_file, expected)

    def test21beta5MissingFiles(self):
        groundtruth_file = 'data/classification_projects/grountruth.yaml'
        filelist_file = 'data/classification_projects/21beta5filelist_missing_files.yaml'
        expected = '2.1-beta5'

        # Should work even if some files are missing.
        self.check_project(groundtruth_file, filelist_file, expected)

    def testForceConsistency(self):
        groundtruth_file = 'data/classification_projects/grountruth.yaml'
        filelist_file = 'data/classification_projects/mixed_filelist.yaml'
        expected = '2.1-beta2'

        # The test should pass with force_consistency=False and raise an
        # exception with force_consistency=True.
        self.check_project(groundtruth_file,
                        filelist_file,
                        expected,
                        force_consistency=False)

        with self.assertRaises(Exception) as context:
            self.check_project(groundtruth_file,
                            filelist_file,
                            expected,
                            force_consistency=True)

suite = unittest.TestLoader().loadTestsFromTestCase(TestGenerateClassificationProject)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
