#!/usr/bin/env python

from gaia2 import *
import os, os.path
import re

'''
IMPORTANT NOTE:

usage of this script is a bit peculiar.
when you want to regenerate the docstrings, you first need to compile and install the
latest version of the python bindings.
Then you need to call this script, and finally you need to recompile the python bindings.

It is a good idea to only do this before a tag, as the file containing the docstrings
is committed to the repo as well.
'''


def currentPath():
    '''Returns the path in which the calling file is located.'''
    return os.path.dirname(os.path.join(os.getcwd(), sys._getframe(1).f_globals['__file__']))

os.chdir(os.path.join(currentPath(), '..', '..'))


docstringFile = 'src/bindings/docstring.i'

cmds = '''
doxygen
python src/doc/doxy2swig.py build/doc/xml/index.xml %s''' % docstringFile

for cmd in cmds.split('\n'):
    os.system(cmd)

allDocstrings = open(docstringFile).read()

analyzerDict = {}
metricDict = {}


def getDoc(className):
    rexp = re.compile('gaia2::%s "(.*?)";\n' % className, re.MULTILINE | re.DOTALL)
    try:
        docstr = rexp.search(allDocstrings).group(1)

        # clean up a bit
        docstr = docstr.replace('\\"', '"')

        return docstr

    except:
        print 'Could not find doc for:', className


for analyzer in AnalyzerFactory.keys():
    analyzerDict[analyzer.lower()] = getDoc(analyzer)


for metric in MetricFactory.keys():
    metricDict[metric.lower()] = getDoc(metric + 'Distance')


docDict = { 'analyzer': analyzerDict,
            'metric': metricDict
            }


out = open(docstringFile[:-2] + '_factory.i', 'w')
out.write('''
%pythoncode {
  _gaia2_docDict = ''')

out.write(repr(docDict))
out.write('\n}\n')
out.close()

print 'Everything OK!'
