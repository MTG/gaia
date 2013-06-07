#!/usr/bin/env python

from gaia2 import *
import unittest
import testdata

def testValidPoint(dataset, clause, fromList = None):
    # search the point using the clause:
    # if we have a result, the clause was true
    # if we have no result, the clause was false
    v = View(dataset)
    dist = MetricFactory.create('null', dataset.layout())
    filtr = 'WHERE ' + clause
    if fromList:
        filtr = 'FROM ' + fromList + ' ' + filtr
    result = v.nnSearch(dataset.samplePoint(), dist, filtr).get(1)

    if len(result) == 1:
        return True
    return False

def testClause(clause):
    ds = testdata.createSimpleDataSet()
    return testValidPoint(ds, clause)


class TestParser(unittest.TestCase):

    def setUp(self):
        cvar.verbose = False

    def tearDown(self):
        testdata.resetSettings()

    def testSimpleExpression(self):
        listTests = {
            '10 < 20': True,
            ' 23.34 == 45': False,
            ' 23.34 = 45': False,
            '12 ==12.0': True,
            '23>23': False,
            '23\t > 23   ': False,
            '1<2 and 2<3': True,
            '1<2 and 2>3': False,
            'NOT (True)': False,
            '1<2 and not (2>3)': True,
            '12 in ( 1, 2, 3, 4, 5 )': False,
            '23 in (1, 23, 37, 42, 5)': True,
            '"ABC" IN ("FG")': False,
            '"acid" in ("this", "is", "an", "acid", "test")': True,
            '"smarties" NOT IN ("my pocket", "your pocket")': True,
            '2.3 BETWEEN 1e-4 AND 2e7': True,
            '2 between 2 and 3': True,
            '2 between 3 and 2': True,
            '4 between 3 and 2': False,
            'true': True
            }

        for clause in listTests:
            result = listTests[clause]
            self.assertEqual(testClause(clause), result, clause)

        # also test mix of these (1 OR 2, 1 AND 2)
        for clauseA in listTests:
            for clauseB in listTests:
                self.assertEqual(testClause('(' + clauseA + ') AND (' + clauseB + ')'),
                                 listTests[clauseA] and listTests[clauseB])
                self.assertEqual(testClause('(' + clauseA + ') OR (' + clauseB + ')'),
                                 listTests[clauseA] or listTests[clauseB])

    def testParserStillInValidStateAfterParserError(self):
        '''ticket #20: parser is in invalid state after parser error'''
        ds = testdata.createSimpleDataSet()
        dist = MetricFactory.create('null', ds.layout())
        v = View(ds)

        result = v.nnSearch(ds.samplePoint(), dist, 'WHERE true').get(1)
        clause = 'WHERE label.tonal_key_mode.value = \\"major"'
        try: result = v.nnSearch(ds.samplePoint(), dist, clause).get(1)
        except: pass # filter correctly failed to compile
        result = v.nnSearch(ds.samplePoint(), dist, 'WHERE true').get(1)


    def testVariables(self):
        d = testdata.createSimpleDataSet()

        def test(clause, expected):
            self.assertEqual(testValidPoint(d, clause), expected)

        d.point('p')['d'] = 'Hello'

        test('label.d = "Hello"', True)
        test('label.d = "goodbye"', False)

        d.point('p')['b'] = 23.

        test('value.b < 23', False)
        test('value.b <= 23', True)
        test('value.b != 23', False)
        test('value.b > 23', False)
        test('value.b == 23', True)
        test('value.b = 23', True)

        test('value.b <= 23', True)

        test('value.b = 23.0 and label.d = "Hello"', True)
        test('value.b = 23.0 or label.d = "Ho ho"', True)
        test('value.b < 23.0 and label.d = "Hello"', False)
        test('value.b = 23.0 and label.d = "Hell"', False)

        d.point('p')['a.1'] = 17

        test('value.a.1 == 17', True)
        test('value.a.1 < 20 and value.b > 20 and label.d != "ooh yeah"', True)

        test('point.id IN ("c", "a", "t")', False)
        test('point.id NOT IN ("a", "p", "u")', False)
        test('point.id NOT IN ("s", "u", "n")', True)
        test('point.id == "p"', True)
        test('point.id != "rock\'n\'roll"', True)


    def testFixLength(self):
        testdata.useFixedLength = True
        self.testSimpleExpression()
        self.testParserStillInValidStateAfterParserError()
        self.testVariables()

    def testEnumerate(self):
        testdata.useEnumerate = True
        self.testSimpleExpression()
        self.testParserStillInValidStateAfterParserError()
        self.testVariables()

    def testEnumerateFixLength(self):
        testdata.useEnumerate = True
        self.testFixLength()


suite = unittest.TestLoader().loadTestsFromTestCase(TestParser)

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite)
