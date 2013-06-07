#!/usr/bin/env python
# encoding: utf-8

from collections import defaultdict
from confusionmatrix import ConfusionMatrix


# should we take the first (ie: test != training) or the second result (test included in training)?


def search(ds, view, p):
    #print view.nnSearch(p, 2)
    return ds.point(view.nnSearch(p, 2)[1][0]).label('genre')

def evaluate_1NN(dataset, groundTruth, distance, params = {}):
    from gaia2 import View, MetricFactory
    view = View(dataset, MetricFactory.create(distance, dataset.layout(), params))

    #classifier = lambda p: dataset.point(view.nnSearch(p, 2)[0][0]).label('genre')
    classifier = lambda p: search(dataset, view, p)

    confusion = evaluate(dataset, classifier, groundTruth)

    return confusion



def evaluate(dataset, classifier, groundTruth):
    '''evaluate the classifier on the given dataset, using only the points
    that are in the groundTruth. It then returns the confusion matrix.

    - dataset is the dataset from which to get the points
    - classifier is a function which given a point returns its class
    - groundTruth is a map from the points to classify to their respective class
    '''

    total = len(groundTruth)
    done = 0

    confusion = ConfusionMatrix()

    for (pointId, expected) in groundTruth.items():
        try:
            found = classifier(dataset.point(pointId))
            confusion.add(expected, found, pointId)
            
        except Exception, e:
            print 'WARNING: Could not classify point', pointId, 'because', str(e)

        done += 1
        print '\r[%d%%]' % int(done*100 / total),


    return confusion

