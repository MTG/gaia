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


def createDataSet():
    l = PointLayout()
    l.add('a', RealType)

    ds = DataSet()

    # p1.a = (0.0, 0.0)
    p = Point()
    p.setName('p1')
    p.setLayout(l)
    p['a'] = (0.0, 0.0)
    ds.addPoint(p)

    # p2.a = (0.5, 1.0)
    p = Point()
    p.setName('p2')
    p.setLayout(l)
    p['a'] = (0.5, 1.0)
    ds.addPoint(p)

    if testdata.useFixedLength:
        ds = testdata.fixLength(ds)

    if testdata.useEnumerate:
        ds = testdata.enumerateStrings(ds)

    return ds


class TestAlgorithms(unittest.TestCase):

    def compareResults(self, r1, r2):
        self.assertEqual(len(r1), len(r2))
        for i in range(len(r1)):
            self.assertEqual(r1[i][0], r2[i][0])
            self.assertAlmostEqual(r1[i][1], r2[i][1], 5)


    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def testNormalize(self):
        ds = createDataSet()
        ds = transform(ds, 'fixlength')
        dsn = transform(ds, 'normalize')

        # by default, vector-normalization is used
        self.assertEqual(dsn.point('p2').value('a'), (0.5, 1.0))

        dsn2 = transform(ds, 'normalize', { 'independent': True })
        self.assertEqual(dsn2.point('p2').value('a'), (1.0, 1.0))

    def testGaussianize(self):
        ds = testdata.loadTestDB()
        ds = transform(ds, 'removevl')
        ds = transform(ds, 'fixlength')
        self.assertEqual(ds[0]['tempotap_bpm'], 104.28208160400391)
        ds = transform(ds, 'gaussianize')
        self.assertEqual(ds[0]['tempotap_bpm'], -0.1928621232509613)


    def compareDataSets(self, ds1, ds2):
        # test if datasets have the same point names
        checkIsomorphDataSets(ds1, ds2)

        # test if the contents of the points are the same
        for pname in ds1.pointNames():
            self.assertEqual(ds1.point(pname), ds2.point(pname))


    def testMerge(self):
        #setDebugLevel(GAlgorithms)
        ds = testdata.loadTestDB()
        ds1 = transform(ds, 'select', { 'descriptorNames': '*.mean' })
        ds2 = transform(ds, 'select', { 'descriptorNames': '*.var' })
        ds12 = transform(ds, 'select', { 'descriptorNames': [ '*.mean', '*.var'] })

        ds_merged = mergeDataSets(ds1, ds2)

        # we need to do this because to add a Point we need it with the
        # original layout, not the FixedLength one
        testdata.resetSettings()
        ds_orig = testdata.loadTestDB()
        sp = ds_orig.samplePoint()

        # test if we can add points normally
        ds_merged.removePoint(sp.name())
        ds_merged.addPoint(sp)

        # compare datasets contents
        self.compareDataSets(ds12, ds_merged)

        # test the mapDataSet function of the Merge applier
        ds_remapped = ds_merged.history().mapDataSet(ds_orig)

        self.compareDataSets(ds12, ds_remapped)
        self.compareDataSets(ds_merged, ds_remapped)

    def testAddField(self):
        ds = testdata.loadTestDB()

        dance = ds.samplePoint().value('danceability')

        # test exception when adding a field that already exists
        af = AnalyzerFactory.create('addfield', { 'string': ['metadata.artist', 'metadata.album'],
                                                  'real': 'danceability' })
        self.assertRaises(Exception, af.analyze, ds)

        # test normal operation
        dsm = transform(ds, 'addfield', { 'string': ['metadata.artist', 'metadata.album'],
                                          'real': ['metadata.year', 'metadata.track', 'metadata.ratings'],
                                          'default': { 'metadata.album': 'unknown',
                                                       'metadata.year': 2012,
                                                       'metadata.ratings': [ 12, 23, 34 ]}})
        p = dsm.samplePoint()
        self.assertEqual(p.value('metadata.year'), 2012)
        self.assertEqual(p.value('metadata.ratings'), (12, 23, 34))
        self.assertEqual(p.value('metadata.track'), 0)
        self.assertEqual(p.label('metadata.artist'), '')
        self.assertEqual(p.label('metadata.album'), 'unknown')
        self.assertEqual(p.value('danceability'), dance)


    def testAddFieldFixedLength(self):
        ds = testdata.loadTestDB()
        ds_fl = transform(ds, 'fixlength')
        ds_addvl = transform(ds, 'addfield', { 'real': 'hello' })
        ds_fl_addvl = transform(ds_fl, 'addfield', { 'real': 'hello' })

        self.assertEqual(ds_addvl.layout().descriptorLocation('hello').lengthType(), VariableLength)
        self.assertEqual(ds_fl_addvl.layout().descriptorLocation('hello').lengthType(), VariableLength)

        ds_addvl_fl = transform(ds_addvl, 'fixlength')
        ds_fl_addvl_fl = transform(ds_fl_addvl, 'fixlength')

        self.assertEqual(ds_addvl_fl.layout(), ds_fl_addvl_fl.layout())

        ds_fl_addfl = transform(ds_fl, 'addfield', { 'real': 'hello', 'size': { 'hello': 1 } })
        self.assertEqual(ds_fl_addfl.layout(), ds_fl_addvl_fl.layout())
        self.assertEqual(ds_fl_addfl[0]['hello'], 0)

        ds_fl_addfl2 = transform(ds_fl, 'addfield', { 'real': 'hello',
                                                      'string': 'youhou',
                                                      'size': { 'hello': 3, 'youhou': 6 },
                                                      'default': { 'hello': [ 2, 5, 3 ],
                                                                   'youhou': [ 'a', 'b', 'c', 'd', 'e', 'f' ] }
                                                      })

        self.assertEqual(ds_fl_addfl2.layout().descriptorLocation('hello').dimension(), 3)
        self.assertEqual(ds_fl_addfl2.layout().descriptorLocation('youhou').dimension(), 6)

        self.assertEqual(ds_fl_addfl2[0]['hello'], (2, 5, 3))



    def testEnumerateKey(self):
        db = testdata.loadTestDB()

        testdata.useEnumerate = True
        dbe = testdata.loadTestDB()

        # also make sure we can map single points correctly
        # we need to load it separately and not take it from the dataset to ensure
        # that it'll have a different enum map
        p = Point()
        p.load('data/dataset_small/Vocal and Acapella/04 Blue Skies.mp3.sig')
        print p.name()

        #also create a transfo that forwards enums after we did the enumerate transfo
        dbes = transform(dbe, 'select', { 'descriptorNames': '*key*' })
        pe = dbes.history().mapPoint(p)

        self.assertEqual(p['key_mode'], pe['key_mode'])
        self.assertEqual(p['key_key'],  pe['key_key'])

        self.assertNotEqual(db.layout(), dbe.layout())

        for p in db.points():
            pe = dbe.point(p.name())

            self.assertEqual(p.label('key_key'),
                             pe.label('key_key'))

            self.assertEqual(p.label('key_mode'),
                             pe.label('key_mode'))


    def testEnumerateThenFixLength(self):
        db = testdata.loadTestDB()

        dbe = testdata.enumerateStrings(db, exclude = testdata.TEST_DATABASE_VARLENGTH_STRING)
        dbefl = testdata.fixLength(dbe)

        for p in db.points():
            pe = dbefl.point(p.name())

            self.assertEqual(p.label('key_key'),
                             pe.label('key_key'))

            self.assertEqual(p.label('key_mode'),
                             pe.label('key_mode'))


    def testSVM(self):
        trainingDS = testdata.readLibSVMDataSet(testdata.SVM_TRAINING_SET)
        trainingDS = transform(trainingDS, 'fixlength')

        trained = transform(trainingDS, 'svmtrain', { 'descriptorNames': 'value',
                                                      'className': 'class',
                                                      # setting this to True make the results
                                                      # different... bug or libsvm feature?
                                                      #'probability': True
                                                      })

        testDS = testdata.readLibSVMDataSet(testdata.SVM_TESTING_SET)
        predicted = trained.history().mapDataSet(testDS)

        expected = [ l.strip() for l in open(testdata.SVM_RESULT).readlines() ]
        for p, expectedClass in zip(predicted.points(), expected):
            self.assertEqual(p.label('class'), expectedClass)

    def testSVM2(self):
        import yaml
        # TODO: need to add a test for text descriptors (ie: key*, ...)
        for t in [ 'singledesc', 'multidimdesc', 'all', 'probability' ]:
            history = TransfoChain()
            history.load('data/svm/test_svm_%s.history' % t)
            ds = testdata.loadTestDB()

            ds = history.mapDataSet(ds)
            gt = yaml.load(open('data/svm/test_svm_%s.gt.yaml' % t).read())

            for p in ds.points():
                self.assertEquals(p['genre'], gt[p.name()])


    def testRCA(self):
        ds = testdata.loadTestDB()
        ds = transform(ds, 'removevl')
        ds = transform(ds, 'fixlength')
        ds = transform(ds, 'remove', { 'descriptorNames': '*cov' })
        ds = transform(ds, 'cleaner')
        ds = transform(ds, 'normalize')
        ds = transform(ds, 'pca', { 'resultName': 'pca15',
                                    'dimension': 15 })
        ds_rca = transform(ds, 'rca', { 'resultName': 'rca10',
                                        'dimension': 10,
                                        'classFile': testdata.RCA_GENRE_GT })

        v = View(ds_rca)
        dist = MetricFactory.create('euclidean', ds_rca.layout())
        self.compareResults(v.nnSearch('01 Cigarettes And Alcohol - Oasis.mp3', dist).get(10),
                            testdata.RCA_GENRE_RESULTS)

        # try by passing directly the groundtruth map
        import gaia2.fastyaml as yaml
        ds_rca = transform(ds, 'rca', { 'resultName': 'rca10',
                                        'dimension': 10,
                                        'classMap': yaml.load(open(testdata.RCA_GENRE_GT).read()) })

        v = View(ds_rca)
        dist = MetricFactory.create('euclidean', ds_rca.layout())
        self.compareResults(v.nnSearch('01 Cigarettes And Alcohol - Oasis.mp3', dist).get(10),
                            testdata.RCA_GENRE_RESULTS)

    def testCenter(self):
        ds = testdata.createSimpleDataSet()
        l = testdata.createSimpleLayout()
        for i in range(4):
            p = Point()
            p.setName('p%d' % i)
            p.setLayout(l)
            ds.addPoint(p)
        ds.removePoint('p')

        ds.point('p0')['a.1'] = [ 0, 1 ]
        ds.point('p1')['a.1'] = [ 4, 3 ]
        ds.point('p2')['a.1'] = [ 6, 9 ]
        ds.point('p3')['a.1'] = [ 2, 27 ]
        # mean = [ 3, 10 ]

        ds = transform(ds, 'fixlength')
        dsc = transform(ds, 'center', { 'descriptorNames': 'a.1' })

        self.assertEqual(dsc.point('p0')['a.1'], (-3, -9))
        self.assertEqual(dsc.point('p1')['a.1'], ( 1, -7))
        self.assertEqual(dsc.point('p2')['a.1'], ( 3, -1))
        self.assertEqual(dsc.point('p3')['a.1'], (-1, 17))


    def testFixLength(self):
        testdata.useFixedLength = True
        self.testNormalize()
        self.testMerge()
        self.testAddField()
        self.testSVM()
        #self.testSVM2() # why does this work, we shouldn't be able to apply a transfo on an already transformed dataset...
        self.testRCA()
        self.testCenter()

    def testEnumerate(self):
        testdata.useEnumerate = True
        self.testNormalize()
        self.testMerge()
        self.testAddField()
        #self.testSVM2()
        self.testCenter()
        self.testRCA()

    def testEnumerateFixLength(self):
        testdata.useEnumerate = True
        self.testFixLength()


suite = unittest.TestLoader().loadTestsFromTestCase(TestAlgorithms)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
