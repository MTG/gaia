#!/usr/bin/env python

from gaia2 import *
import unittest
import testdata


def search(dataset, id, n):
    v = View(dataset)
    dist = MetricFactory.create('euclidean', dataset.layout())
    return v.nnSearch(id, dist).get(n)

class TestTransfo(unittest.TestCase):

    def compareResults(self, r1, r2):
        self.assertEqual(len(r1), len(r2))
        for i in range(len(r1)):
            self.assertEqual(r1[i][0], r2[i][0])
            self.assertAlmostEqual(r1[i][1], r2[i][1], 5)


    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def testDoesntBlowup(self):
        ds = testdata.loadTestDB()
        ignored_descs = testdata.TEST_DATABASE_VARLENGTH_REAL
        ds = transform(ds, 'fixlength', { 'except': ignored_descs })

        dsc = transform(ds, 'cleaner', { 'except': ignored_descs })
        dsr = transform(dsc, 'remove', { 'descriptorNames': '*mfcc*' })
        dsr2 = transform(dsc, 'remove', { 'descriptorNames': [ '*mfcc*' ] })
        del dsr2
        del dsc
        del ds
        dsn = transform(dsr, 'normalize', { 'except': ignored_descs })
        dspca = transform(dsn, 'pca', { 'resultName': 'pca30',
                                        'dimension': 30,
                                        'descriptorNames': [ '*.mean', '*.var' ] })

    def testQt46FloatParameterBug(self):
        # Note: this was triggered by Qt 4.6 introducing a QVariant(float) constructor, which resulted
        #       in pmapToPython to fail with an unknown type error (followed by a segfault)...
        ds = testdata.loadTestDB()
        ds = transform(ds, 'fixlength')
        ds = transform(ds, 'removevl')
        ds = transform(ds, 'normalize')
        self.assertEqual(ds.history().toPython()[-1]['Applier parameters']['coeffs']['.barkbands.mean']['a'][0],
                         24.922689437866211)

    def testRegressionGaia14(self):
        ds = testdata.loadSmallDB()
        ds = transform(ds, 'fixlength')

        to_remove = testdata.TEST_SMALLDB_VARLENGTH
        dsr = transform(ds, 'remove', { 'descriptorNames': to_remove })

        self.compareResults(search(dsr, '1_ethno.wav', 5), testdata.SMALL_DB_RAW_RESULTS)

        dsc = transform(dsr, 'cleaner')
        self.compareResults(search(dsc, '1_ethno.wav', 5), testdata.SMALL_DB_CLEAN_RESULTS)

        dsn = transform(dsc, 'normalize')
        self.compareResults(search(dsn, '1_ethno.wav', 5), testdata.SMALL_DB_NORM_RESULTS)

        dspca = transform(dsn, 'pca', { 'resultName': 'pca30',
                                        'dimension': 30,
                                        'descriptorNames': '*' })
        self.compareResults(search(dspca, '1_ethno.wav', 5), testdata.SMALL_DB_PCA_RESULTS)


    def testWrongArgument(self):
        ds = testdata.loadTestDB()
        ds = transform(ds, 'fixlength')
        ds = transform(ds, 'removevl')
        ds = transform(ds, 'cleaner')
        ds = transform(ds, 'normalize')
        # missing param: className
        self.assertRaises(Exception, transform, ds, 'svmtrain', { 'descriptorNames': '*.mean' })
        # wrong param: descriptorName
        self.assertRaises(Exception, transform, ds, 'svmtrain', { 'className': 'kloug',
                                                                  'descriptorName': '*.mean' })

        # missing param: resultName
        self.assertRaises(Exception, transform, ds, 'pca', { 'dimension': 15, 'resultName': '' })


    def testSimplifyHistory(self):
        ds = testdata.createSimpleDataSet()
        p = Point()
        p.setName('p2')
        p.setLayout(ds.layout())
        p['a.2'] = [ 1.2, 2.3 ]
        ds.addPoint(p)
        ds0 = ds.copy()

        ds1 = ds.copy()
        ds1.simplifyHistory()
        self.assertEqual(ds1.history().size(), 0)

        ds = transform(ds, 'removevl')
        ds2 = ds.copy()
        ds2.simplifyHistory()

        self.assertEqual(ds2.history().toPython(),
                         [{ 'Analyzer name': 'remove',
                            'Analyzer parameters': { 'descriptorNames': [ '.a.2' ] },
                            'Applier name': 'removedesc',
                            'Applier parameters': {'descriptorNames': [ '.a.2' ] },
                            'Additional info': {}
                            }])

        ds = transform(ds, 'fixlength')
        ds3 = ds.copy()
        ds3.simplifyHistory()

        self.assertEqual(ds3.history().toPython(),
                         [{ 'Analyzer name': 'remove',
                            'Analyzer parameters': { 'descriptorNames': [ '.a.2' ] },
                            'Applier name': 'removedesc',
                            'Applier parameters': {'descriptorNames': [ '.a.2' ] },
                            'Additional info': {}
                            },
                          { 'Analyzer name': 'fixlength',
                            'Analyzer parameters': { 'descriptorNames': [ '.a.1', '.b', '.c', '.d' ] },
                            'Applier name': 'fixlengthapplier',
                            'Applier parameters': {'descriptorNames': [ '.a.1', '.b', '.c', '.d' ] },
                            'Additional info': {}
                            }])

        ds = transform(ds, 'remove', { 'descriptorNames': 'a.*' })
        ds4 = ds.copy()
        ds4.simplifyHistory()

        self.assertEqual(ds4.history().toPython(),
                         [{ 'Analyzer name': 'remove',
                            'Analyzer parameters': { 'descriptorNames': [ '.a.1', '.a.2' ] },
                            'Applier name': 'removedesc',
                            'Applier parameters': {'descriptorNames': [ '.a.1', '.a.2' ] },
                            'Additional info': {}
                            },
                          { 'Analyzer name': 'fixlength',
                            'Analyzer parameters': { 'descriptorNames': [ '.b', '.c', '.d' ] },
                            'Applier name': 'fixlengthapplier',
                            'Applier parameters': {'descriptorNames': [ '.b', '.c', '.d' ] },
                            'Additional info': {}
                            }])

        ds = transform(ds, 'select', { 'descriptorNames': [ 'b', 'c' ] })
        ds5 = ds.copy()
        ds5.simplifyHistory()

        self.assertEqual(ds5.history().toPython(),
                         [{ 'Analyzer name': 'remove',
                            'Analyzer parameters': { 'descriptorNames': [ '.a.1', '.a.2', '.d' ] },
                            'Applier name': 'removedesc',
                            'Applier parameters': {'descriptorNames': [ '.a.1', '.a.2', '.d' ] },
                            'Additional info': {}
                            },
                          { 'Analyzer name': 'fixlength',
                            'Analyzer parameters': { 'descriptorNames': [ '.b', '.c' ] },
                            'Applier name': 'fixlengthapplier',
                            'Applier parameters': {'descriptorNames': [ '.b', '.c' ] },
                            'Additional info': {}
                            }])


        p2 = Point()
        p2.setLayout(ds0.layout())
        p2['b'] = 23
        p2['c'] = 78
        p2['a.2'] = [ 1, 2, 3, 4 ]

        p2m = ds5.history().mapPoint(p2)

        self.assertEqual(p2m.layout().descriptorNames(), ('.b', '.c'))
        self.assertEqual(p2m['b'], 23.)
        self.assertEqual(p2m['c'], 78.)




    def testFixLength(self):
        testdata.useFixedLength = True
        self.testDoesntBlowup()
        self.testRegressionGaia14()

    def testEnumerate(self):
        testdata.useEnumerate = True
        self.testDoesntBlowup()
        self.testRegressionGaia14()

    def testEnumerateFixLength(self):
        testdata.useEnumerate = True
        self.testFixLength()


suite = unittest.TestLoader().loadTestsFromTestCase(TestTransfo)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
