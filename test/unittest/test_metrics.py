#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gaia2 import *
import unittest
import testdata

def newPoint(name):
    l = PointLayout()
    l.add('a', RealType)

    p = Point()
    p.setName(name)
    p.setLayout(l)

    return p


def createDataSet():
    ds = DataSet()

    # p0.a = (0.0, 0.0) (α = undefined)
    p0 = newPoint('p0')
    p0['a'] = (0.0, 0.0)

    # p1.a = (1.0, 0.0) (α = 0)
    p1 = newPoint('p1')
    p1['a'] = (1.0, 0.0)

    # p2.a = (0.0, 1.0) (α = π/2)
    p2 = newPoint('p2')
    p2['a'] = (0.0, 1.0)

    # p3.a = (-1.0, 0.0) (α = π)
    p3 = newPoint('p3')
    p3['a'] = (-1.0, 0.0)

    # p4.a = (1.0, 1.0) (α = π/4)
    p4 = newPoint('p4')
    p4['a'] = (1.0, 1.0)

    # p5.a = (1.0, -1.0) (α = -π/4)
    p5 = newPoint('p5')
    p5['a'] = (1.0, -1.0)

    ds.addPoints([ p0, p1, p2, p3, p4, p5 ])

    if testdata.useFixedLength:
        ds = testdata.fixLength(ds)

    if testdata.useEnumerate:
        ds = testdata.enumerateStrings(ds)

    return ds


class TestMetrics(unittest.TestCase):

    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def testAngleDistance(self):
        ds = createDataSet()
        ds = transform(ds, 'fixlength')
        dist = MetricFactory.create('CosineAngle', ds.layout())

        self.assertEqual(dist(ds.point('p1'), ds.point('p1')), 0.0)
        self.assertEqual(dist(ds.point('p1'), ds.point('p2')), 0.5)
        self.assertEqual(dist(ds.point('p1'), ds.point('p3')), 1.0)
        self.assertEqual(dist(ds.point('p1'), ds.point('p4')), 0.25)
        self.assertEqual(dist(ds.point('p1'), ds.point('p5')), 0.25)
        self.assertEqual(dist(ds.point('p4'), ds.point('p5')), 0.5)

        self.assertRaises(Exception, dist, ds.point('p0'), ds.point('p1'))

        d = MetricFactory.create('CosineAngle', ds.layout(), { 'defaultValue': 0.5 })
        self.assertEqual(d(ds.point('p0'), ds.point('p1')), 0.5)

    def testExponentialCompress(self):
        ds = createDataSet()
        ds = transform(ds, 'fixlength')
        dist = MetricFactory.create('ExponentialCompress', ds.layout(), { 'distance': 'euclidean' })

        self.assertEqual(dist(ds.point('p1'), ds.point('p1')), 0.0)
        self.assertAlmostEqual(dist(ds.point('p1'), ds.point('p0')), 0.63212056) # 1-exp(-1)
        self.assertAlmostEqual(dist(ds.point('p1'), ds.point('p3')), 0.86466472) # 1-exp(-2)

    def testRhythmDistance(self):
        p1 = testdata.createSimpleDataSet().point('p')
        p2 = Point(p1)

        dist = MetricFactory.create('Rhythm', p1.layout(), { 'descriptorName': 'a.1',
                                                             'indexRange': [ 1, 2, 4, 8 ],
                                                             'alpha': 0.8 })

        p1['a.1'] = 3
        p2['a.1'] = 2
        self.assertAlmostEqual(dist(p1, p1), 0.0)
        self.assertAlmostEqual(dist(p1, p2), 0.4)
        self.assertAlmostEqual(dist(p2, p1), 0.4)

        p1['a.1'] = 3.14
        self.assertAlmostEqual(dist(p1, p2), 0.344)

        p1['a.1'] = 6.23
        self.assertAlmostEqual(dist(p1, p2), 0.45312)

    def testWeightedPearson(self):
        ds = testdata.createSimpleDataSet()
        ds.point('p')['a.1'] = [ 0, 0 ] # need to have 2 values before fixing length
        p1 = transform(ds, 'fixlength').point('p')
        p2 = Point(p1)

        dist = MetricFactory.create('WeightedPearson', p1.layout(), { 'weights': { '1': 0.3,
                                                                                   'c': 0.7 }
                                                                      })
        p1['a.1'] = [ 0.12, 2.71 ]
        p1['c'] = 4.32
        p2['1'] = [ 0.46, 1.12 ]
        p2['c'] = 2.4242

        self.assertAlmostEqual(dist(p1, p2), 0.038222129799, 6)

    def testKullbackLeibler(self):
        ds = transform(testdata.loadTestDB(), 'fixlength')

        # creates a test with more than 1000 points otherwise the test is useless because
        # we split the workload in chunks of 1000 points when computing the distance
        dstest = DataSet()
        ncopy = 20
        for cidx in range(ncopy):
            points = list(ds.points())
            for p in points:
                p.setName(p.name() + '-%d' % cidx)
            dstest.addPoints(points)

        # test whether KL doesn't break with multithreading (did in 2.2.1)
        v = View(dstest)
        dist = MetricFactory.create('kullbackleibler',
                                    dstest.layout(),
                                    { 'descriptorName': 'mfcc' })


        results = v.nnSearch(ds.samplePoint(), dist).get(6*ncopy)
        expected = [ 0.0 ]*2*ncopy + [ 6.1013755798339844 ]*ncopy
        expected += [ 6.4808731079101562 ]*2*ncopy + [ 6.7828292846679688 ]*ncopy

        for r, e in zip(results, expected):
            self.assertAlmostEqual(r[1], e, 5)

    def testForceIdentity(self):
        l = PointLayout()
        l.add('a', RealType, FixedLength, 1)

        p = Point()
        p.setLayout(l)

        cd = MetricFactory.create('cosinesimilarity', p.layout(), { 'defaultValue': 0.5 })
        self.assertEquals(cd(p, p), 0.5)

        ficd = MetricFactory.create('forceidentity',
                                    p.layout(),
                                    { 'distance': 'cosinesimilarity',
                                      'params': { 'defaultValue': 0.5 }
                                      })

        self.assertEquals(ficd(p, p), 0.0)

        p2 = Point(p)
        p2.setName('p2')
        self.assertEquals(ficd(p, p2), 0.5)


    def testFixLength(self):
        testdata.useFixedLength = True
        self.testAngleDistance()
        self.testExponentialCompress()
        self.testRhythmDistance()
        #self.testWeightedPearson() # can't be here cause p['a.1'] is of size 1

    def testEnumerate(self):
        testdata.useEnumerate = True
        # no tests yet using string descriptors
        pass

    def testEnumerateFixLength(self):
        testdata.useEnumerate = True
        self.testFixLength()


suite = unittest.TestLoader().loadTestsFromTestCase(TestMetrics)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
