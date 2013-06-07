#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import *
from gaia2.cyclops import Cyclops
import gaia2.fastyaml as yaml
import subprocess, os, time

def prepareDataSet():
    ds = DataSet()
    ds.load('../../../test/unittest/data/dataset_small.db')
    ds = transform(ds, 'removevl')
    ds = transform(ds, 'fixlength')

    ds.save('/tmp/cyclops_unittest.db')

def launchMasterSlaves():
    config = { 'port': 8090,
               'slaves': [ { 'host': 'localhost',
                             'port': 8091 },
                           { 'host': 'localhost',
                             'port': 8092 }
                           ]
               }

    yaml.dump(config, open('/tmp/cyclops_unittest_config.yaml', 'w'))

    pids = []
    for slave in config['slaves']:
        pids += [ subprocess.Popen([ 'cyclops', '-p', str(slave['port']) ],
                                   stdout = subprocess.PIPE,
                                   stderr = subprocess.PIPE).pid ]

    time.sleep(1)
    pids += [ subprocess.Popen([ 'cyclopsmaster', '/tmp/cyclops_unittest_config.yaml' ],
                               stdout = subprocess.PIPE,
                               stderr = subprocess.PIPE).pid ]

    time.sleep(1)

    return pids


def setup():
    c = Cyclops('localhost:8090')

    c.load('ds', '/tmp/cyclops_unittest.db')

    c.createDistance('euc', 'ds', 'euclidean')
    c.createDistance('euc2', 'ds', 'euclidean', { 'descriptorNames': '*.mean' })

    print 'datasets loaded:', c.datasetNames()
    print 'distances created:', c.distanceNames()



def testMemleak():
    c = Cyclops('localhost:8090')
    for i in range(100000):
        print i,
        list(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                            'ds', { 'distance': 'euclidean',
                                    'params': { 'descriptorNames': '*.mean' }}).get(10))




def testNoCrashInvalidArguments():
    c = Cyclops('localhost:8090')

    try: c.getPoints([ 'ds', 'gloup' ])
    except: pass

    try: c.nnSearchById([ 'ds', 'gloup' ])
    except: pass

    try:
        print list(c.nnSearchById("gloup.mp3",
                                  'ds', { 'distance': 'euclidean',
                                          'params': { 'descriptorNames': '*.mean' }}).get(10, keys = [ 'tempotap_bpm' ]))
    except:
        pass


def cyc2py(results):
    return tuple((pid, dist) for (pid, dist, keys) in results)

def assertEqual(name1, r1, name2, r2):
    r1 = list(r1)
    r2 = list(r2)

    if r1 != r2:
        print '*'*100
        print 'ERROR: not equal'
        print name1 + ':', r1
        print name2 + ':', r2
        print '*'*100
        #raise

def testRegression():
    c = Cyclops('localhost:8090')

    ds = DataSet()
    ds.load('/tmp/cyclops_unittest.db')

    euc = MetricFactory.create('euclidean', ds.layout())
    euc2 = MetricFactory.create('euclidean', ds.layout(), { 'descriptorNames': '*.mean' })

    v = View(ds)

    assertEqual('CYCLOPS',
                cyc2py(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                                      'ds', { 'distance': 'euclidean', 'params': { 'descriptorNames': '*.mean' }}).get(10)),
                'GAIA2PY',
                v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3", euc2).get(10))

    assertEqual('CYCLOPS',
                cyc2py(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                                      'ds', { 'distance': 'euclidean', 'params': { 'descriptorNames': '*.mean' }}).get(10, 10)),
                'GAIA2PY',
                v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3", euc2).get(10, 10))

    assertEqual('CYCLOPS',
                cyc2py(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                                      'ds', { 'distance': 'euclidean', 'params': { 'descriptorNames': '*.mean' }}).get(10, 20)),
                'GAIA2PY',
                v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3", euc2).get(10, 20))


    assertEqual('CYCLOPS BYEX',
                cyc2py(c.nnSearchByExample(c.getPoint('ds', "(Sittin' On) The Dock Of The Bay.mp3"),
                                      'ds', { 'distance': 'euclidean', 'params': { 'descriptorNames': '*.mean' }}).get(10, 20)),
                'GAIA2PY',
                v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3", euc2).get(10, 20))


#print list(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
#                          'ds', { 'distance': 'euclidean',
#                                  'params': { 'descriptorNames': '*.mean' }}).get(10, keys = [ 'tempotap_bpm' ]))


def testIdOrExample():
    c = Cyclops('localhost:8090')

    assertEqual('BY ID',
                cyc2py(c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                                      'ds', 'euc2').get(10)),
                'BY EXAMPLE',
                cyc2py(c.nnSearchByExample(c.getPoint('ds', "(Sittin' On) The Dock Of The Bay.mp3"),
                                           'ds', 'euc2').get(10)))


def testInputSpace():
    rset = c.nnSearchByIdWithFilter("(Sittin' On) The Dock Of The Bay.mp3",
                                    'ds', 'euc2', 'where value.tempotap_bpm > 130')

    result =  c.nnSearchById("(Sittin' On) The Dock Of The Bay.mp3",
                             'ds', 'euc2', inputSpace = rset).get(10)

    for pid, dist, values in result:
        print pid, '- bpm =', c.getPoint('ds', pid)['tempotap_bpm']



    rset = v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3",
                      'where value.tempotap_bpm > 130',
                      dist)

    result = v.nnSearch("(Sittin' On) The Dock Of The Bay.mp3", rset, dist).get(10)

    for pid, dist in result:
        print pid, '- bpm =', c.getPoint('ds', pid)['tempotap_bpm']



if __name__ == '__main__':
    prepareDataSet()
    pids = launchMasterSlaves()

    try:
        setup()

        print 'test ID or example'
        testIdOrExample()

        print 'test regression (including offset)'
        testRegression()

        print 'test invalid args'
        testNoCrashInvalidArguments()

    except:
        os.system('kill %s' % ' '.join([ str(pid) for pid in pids ]))
        raise

    os.system('kill %s' % ' '.join([ str(pid) for pid in pids ]))


