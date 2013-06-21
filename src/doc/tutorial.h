/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 *
 * Gaia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
/**

@page tutorial Gaia 2.0 Tutorial

This page presents a short tutorial to start using Gaia and do some useful stuff.
It is written using the python bindings, but it would be very similar using the
C++/Qt or the C++/STL interface, only with some pointers instead of references
at some points. The reader paying enough attention should be able to do the
required changes without too many difficulties...

This tutorial, to be useful for people, will be based on use-cases and how to
solve them. The plan is the following:
- @ref basic_handling "Basic handling of gaia types"
 - @ref merging "merge Essentia files into a Gaia DB"
 - @ref getting_values "get values from there, output some stuff"
 - @ref loading_saving "load/save datasets"
- @ref transformations "Doing some transformations on datasets"
 - @ref cleaner "the Cleaner transformation"
 - @ref normalize_pca
- @ref queries "Doing some queries on datasets"
 - @ref creating_metric
 - @ref querybyid
 - @ref querybyexample
 - @ref queryfilter

@section conventions Conventions

all the examples will imply the following:

@verbatim
from gaia2 import *
@endverbatim

All the classes you will see that are not from python will then be from the
module gaia2 (ie: DataSet, Point, etc... are actually gaia2.DataSet, gaia2.Point,
etc...)

There is a verbose flag that you can set which makes transformations
and other stuff display information while they do some computing.
It is the @c gaia2.cvar.verbose variable. Use it like this:

@verbatim
# display info:
cvar.verbose = True
# be silent:
cvar.verbose = False
@endverbatim

@b NB: in C++, it is the global variable @c gaia2::verbose

@section basic_handling Basic handling of datatypes

@subsection merging Merging points in a dataset

Merging points in a dataset is actually quite a simple operation, you just need
to add the Points one by one in a newly created dataset. Don't forget to set the
name for the points!
So that gives, for instance:

@verbatim
ds = DataSet()

p1 = Point()
p1.load('01.Elektrik Psylocibe.mp3.sig')
p1.setName('Elektrik Psylocibe')
ds.addPoint(p1)

p2 = Point()
p2.load('02.Funky Monkey.mp3.sig')
p2.setName('Funky Monkey')
ds.addPoint(p2)

p3 = Point()
# you get it by now...
@endverbatim

As this is a pretty common operation, there is a function already defined that
will do this for you given a file containing all the files to merge:

@verbatim
ds = DataSet.mergeFiles(yaml.load(open('filelist.yaml').read())
@endverbatim

The format of 'filelist.yaml' is a simple <a href="http://yaml.org">yaml</a> mapping
from the points names to their respective paths, for instance:

@verbatim
"Electrik Psylocibe": "01.Electrik Psylocibe.mp3.sig"
"Funky Monkey": "02.Funky Monkey.mp3.sig"
  ...
@endverbatim

@b NB: double-quotes are only necessary if you have spaces in your ids/filenames

As DataSet.mergeFiles expects a python map, you first have to load the filelist.yaml file
yourself, then have yaml parse this as a dictionary, and only then can you give it
to the DataSet.mergeFiles function.


@subsection getting_values Getting values, outputting stuff

You can access a point directly by their ID:

@verbatim
p = ds.point('Elektrik Psylocibe')
@endverbatim

To access attributes, you should use either the value() method or the label()
method depending on the type of the descriptor you want to fetch or, more
pythonically, just use points as if they were dictionaries:

@verbatim
# print some useful stuff
print 'BPM is:', p.value('tempotap_bpm')
print 'Variance of third MFCC coeff is:', p.value('mfcc.var')[2]
print 'Key/mode is: ', p.label('key_key'), p.label('key_mode')

# more pythonic way of doing it:
print 'BPM is:', p['tempotap_bpm']
print 'Key/mode is: ', p['key_key'], p['key_mode']
@endverbatim

@subsection loading_saving Loading/saving datasets

Loading and saving datasets is just as easy as saying it:

@verbatim
ds.save('mytrancemusic.db')
tranceDB = DataSet()
tranceDB.load('mytrancemusic.db')

# from there it's just as before:
print 'How fast does a monkey dance:', tranceDB.point('Funky Monkey').value('tempotap_bpm')
@endverbatim

@section transformations Doing transformations on datasets

To know more about the details of transformations, the how/why/etc..., please
have a look first at the @ref main or the @ref api_overview

A reference of all transformations can be found at the @ref algorithms page.

@subsection cleaner The Cleaner transformation

The cleaner transformation has a bit of a special status, because it is required
to be applied first on any dataset. Failure to do so will result in undefined
behaviour, and also lack of support from the author. So forget it at your own risk!

To apply a transformation, you first need to instantiate the Analyzer that you
require, and analyze the dataset. This will return a Transformation instance:

@verbatim
cleaner = AnalyzerFactory.create('cleaner')
cleaner_transfo = cleaner.analyze(tranceDB)
@endverbatim

Now, you could apply this transformation to any dataset, but here it just makes
more sense to apply it on the dataset you just analyzed:

@verbatim
cleanTranceDB = cleaner_transfo.applyToDataSet(tranceDB)
@endverbatim

You can even apply both steps at once after having instantiated your Analyzer:

@verbatim
cleaner = AnalyzerFactory.create('cleaner')
cleanTranceDB = cleaner.analyze(tranceDB).applyToDataSet(tranceDB)
@endverbatim

Actually, as most of the time you will want to apply the transformation to the
DataSet you just analyzed, there is a shorter way to do this:

@verbatim
cleanTranceDB = transform(tranceDB, 'cleaner')
@endverbatim

That's it, you now have a clean dataset ready to be further analyzed!

@subsection normalize_pca Normalize, PCA & friends

Next, you probably want to apply all sorts of transformations to make your
dataset play nicer with similarity queries. As the way to do this is exactly the
same as for the Cleaner transformation, we will just show some transformations here:

@verbatim
# remove MFCC, because we don't want them! (incidentally, mfcc also contains (as of essentia 0.4)
# covariance and inverse covariance matrix, which do NOT like to be normalized)
no_mfccDB = transform(cleanTranceDB, 'remove', { 'descriptorNames': '*mfcc*' })

# Normalize everything
normalizedDB = transform(no_mfccDB, 'normalize')

# PCA-30 on only the mean and variance of Real descriptors
pcaDB = transform(normalizedDB, 'pca', { 'dimension': 30, 'descriptorNames': [ '*.mean', '*.var' ] })
@endverbatim

Here you go! So it wasn't that hard, was it?

@b NB: when creating an analyzer through the AnalyzerFactory, you can specify
some parameters by using a specific type which is a gaia2.ParameterMap.
In Python (only), you can specify a python map instead of a ParameterMap, and it
will be automatically converted to a ParameterMap. This is what happened in the
previous example.


@section queries Doing queries on datasets

@subsection creating_metric Creating a distance function

As noted in the @ref api_overview, before creating a View that will allow you to do
queries on a dataset, you will first need to decide which distance function you
want to use. To instantiate it, you will need to go through the
DistanceFunctionFactory (also called the MetricFactory), and call the @c create method
with the layout you intend to use the distance function on (from the point or
the dataset).

A reference of all distance functions can be found at the @ref metrics page.

For instance, let's get 3 of these:

@verbatim
euclideanMetric = DistanceFunctionFactory.create('euclidean', pcaDB.layout())
manhattanMetric = DistanceFunctionFactory.create('manhattan', pcaDB.layout())

# define a composite distance which is an euclidean distance on only the mean and var descriptors,
# plus 2 times the manhattan distance on the min and max descriptors:

euclidParams = {
    'name': 'euclidean',
    'params': { 'descriptorNames': [ '*.mean', '*.var' ] },
    'weight': 1.0
}

manhattanParams = {
    'name': 'manhattan',
    'params': { 'descriptorNames': [ '*.min', '*.max' ] },
    'weight': 2.0
}

mixedParams = { 'euclidp': euclidParams, 'manp': manhattanParams }

mixedMetric = DistanceFunctionFactory.create('linearcombination', pcaDB.layout(), mixedParams)
@endverbatim

@subsection querybyid Query by ID

Now that we know which distance functions we want to use, we can create an assorted
View which is the structure that will allow us to do queries on a dataset:

@verbatim
euclideanView = View(pcaDB, euclideanMetric)
mixedDistanceView = View(cleanTranceDB, mixedMetric) # do queries on the original dataset, not the PCA one

similarSongs1 = euclideanView.nnSearch('Funky Monkey').get(10)
similarSongs2 = mixedDistanceView.nnSearch('Elektrik Psylocibe').get(5)
@endverbatim

@subsection querybyexample Query by Example

This is very similar to query by ID, except that you pass a point to the function
instead of its ID (because it's not in the dataset yet, for instance):

@verbatim
queryPoint = Point()
queryPoint.load('recentlyAnalyzedByEssentia.sig')

similarSongs3 = euclideanView.nnSearch(queryPoint).get(10)
@endverbatim

@subsection queryfilter Query with filters

You can filter queries (either by ID or by example) using a filter term which is
very much like an SQL @c where clause. The reference can be found at the @ref filtering page.

@verbatim
filter = 'WHERE value.tempotap_bpm > 120 AND label.key_key = "C#" AND label.key_mode = "minor"'
similarSongs4 = mixedDistanceView.nnSearch(queryPoint, filter).get(5)
@endverbatim


*/
