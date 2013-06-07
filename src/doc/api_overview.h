/**
@page api_overview Gaia 2.0 API Overview

This document is not meant to be an API reference, but rather a short and clear introduction to what you can
do with Gaia, by showing the most useful classes and methods, as well as the way to use them in the way they
are intended to.

@section basic_datatypes Basic Data Types

Gaia allows you to work with collections of tracks, each having a certain number of attributes (called
descriptors), which can be either real values or string values (labels).

The class to store all these tracks is called the DataSet. The DataSet is only intended for storage, and does
not provide any methods for analyzing or searching in it.

The representation of a track or song is called a Point in Gaia. In particular, a point has a layout (which is
a tree), which allows it to structure its information better (You can think of it as an XML DOM document).
All the points in a DataSet must share the same layout. Each leaf in the layout (terminal node) is called a
Descriptor, which can be either a single value or an array of values. Hence @c spectral_centroid will be one
descriptor, and @c MFCC will be one single descriptor as well, even though it is composed of multiple values.
@c Genre or @c artist_id are other examples of descriptors, which are strings.

@b Synopsis:

@verbatim
class DataSet:
    load(filename)     : loads the dataset specified by filename
    save(filename)     : saves current dataset to specified filename
    layout()           : returns the layout associated to the points in this dataset
    point(name)        : looks for point in dxataset and returns it
    addPoint(point)    : adds the given point to the dataset
    removePoint(name)  : remove the point with the given name from this dataset
@endverbatim

@verbatim
class Point:
    value(name)        : returns the value of the real descriptor which has the specified name
    label(name)        : returns the value of the string descriptor which has the specified name
    layout()           : returns the associated layout
@endverbatim

@verbatim
class PointLayout:
    toYaml()           : returns a yaml string representation of the layout
    descriptorNames(*) : returns the list of descriptors matching the given parameters (type of
                         descriptor and/or regular expressions on the name)
@endverbatim


@section transformations Transformations and Dimensionality-reduction Algorithms

Now that we have our basic datatypes ready, we want to do some work with them. In particular, we want to be
able to remove descriptors, normalize some of them, apply PCA, etc...

All these operations are called transformations, and consist in 2 major steps:
 - we first need to analyze the dataset, and retrieve the parameters of the transformation to apply. Indeed,
   the same transformation can lead to different steps when applying the actual transformation, eg: normalizing
   2 different DataSets will lead to 2 different set of parameters, even though the normalize operation is the
   same in both cases. This operation is done by an Analyzer instance.
 - once we have the parameters for the transformation to apply, we need to, well, actually apply them on the
   DataSet (or on another one, for that matters). This is done by the Applier instance.

Instantiation of the Analyzer classes is done through the mean of a Factory (specifically, an AnalyzerFactory).
You can also instantiate explicitly an Applier with the ApplierFactory, however for most users this shouldn't
be necessary, because a Transformation automatically knows what kind of Applier you need for each Analyzer,
and instantiate it for you.

A list of available transformations in Gaia can be found at the @ref algorithms page.

@b Synopsis:

@verbatim
class AnalyzerFactory:
    keys()                   : returns a list of all possible Analyzers that you can instantiate through this factory.
    create(name, parameters) : create an Analyzer with the given parameters (stored in a map).
@endverbatim

@verbatim
class Analyzer:
    analyze(dataset)         : analyzes the given DataSet and returns a Transformation instance, which contains all
                               the parameters necessary for doing the transformation.
@endverbatim

@verbatim
class Transformation:
    applyToDataSet(dataset)  : apply the transformation to given DataSet and returns the result.
                               The original DataSet is left untouched.
    applyToPoint(point)      : apply the transformation to given Point and returns the result.
                               The original Point is left untouched.
@endverbatim

@verbatim
class Applier:
    mapDataSet(dataset)      : applies the transformation to current DataSet and returns the newly created one.
                               The original DataSet is left untouched.
    mapPoint(point)          : applies the same transformation, but only on one Point.
                               The original Point is left untouched.
@endverbatim


@section queries Queries (NN-searches) on a DataSet

To be able to perform nearest-neighbors (NN) queries on a DataSet, you will have to instantiate a View on this DataSet
(remember the DataSet is only for storage, it does not provide any functionality). A View basically consist in the
needed search structure with an associated distance function. Once you have created a View with a given distance
function (created through a factory), you can start doing queries on the dataset using the variety of search
methods, with or without filters, by id or by example, etc...

Filters are strings describing an SQL-like filter. For a reference of the syntax,
look at @ref filtering.

A list of available distance functions in Gaia can be found at the @ref metrics page.

@b NB: @c DistanceFunctionFactory is aliased to @c MetricFactory if you're too lazy to
type the whole thing! :o)

@b Synopsis:

@verbatim
class DistanceFunctionFactory:
    keys()                           : returns a list of all DistanceFunctions available through this factory.
    create(name, layout, parameters) : create a DistanceFunction with the given parameters (stored in a map)

class DistanceFunction:
    operator()(p1, p2)               : returns the distance between the points p1 and p2.

class View:
    View(dataset, distanceFunction)  : create the view on the given DataSet which will use the given DistanceFunction
    nnSearch(point, filter)          : do the query-by-example search on the given point. filter is an optional parameter.
    nnSearch(id, filter)             : do the query-by-id search on the given point. filter is an optional parameter.
@endverbatim
 */
