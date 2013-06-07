#!/usr/bin/env python

import os
import sys
import unittest


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

all = unittest.TestSuite(testObjectsList)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(all)
