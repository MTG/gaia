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
import sys
import unittest2


# make sure the databases are ready
decompress_script = '''
cd data; tar xjf dataset_small.tar.bz2
cd data; gaiamerge dataset_small.dbfiles.yaml dataset_small.db
cd data; tar xjf verysmall.tar.bz2
cd data; gaiamerge verysmall.dbfiles.yaml verysmall.db
cd data; tar xjf svmfiles.tar.bz2
'''

try:
    file('data/dataset_small.db')
    file('data/verysmall.db')
    file('data/train.1')
    file('data/test.1')
    file('data/test.1.predict')
except:
    for cmd in decompress_script.split('\n'):
        os.system(cmd)


# get tests automatically from the directory
import glob
testsList = [ filename[5:-3] for filename in glob.glob('test_*.py') ]


def importTest(name):
    cmd = 'import test_%s; setattr(sys.modules[__name__], \'%s\', test_%s.suite)' % (name, name, name)
    exec(cmd)
    

for test in testsList:
    importTest(test)

testObjectsList = [ getattr(sys.modules[__name__], name) for name in testsList ]

all = unittest2.TestSuite(testObjectsList)

if __name__ == '__main__':
    unittest2.TextTestRunner(verbosity=2).run(all)
