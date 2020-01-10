
// File: index.xml

// File: class____TODO____.xml
%feature("docstring") __TODO__ "

(WARN_IF_UNDOCUMENTED = YES)

C++ includes: gaia.h ";


// File: classgaia2_1_1AddField.xml
%feature("docstring") gaia2::AddField "

The AddField analyzer allows you to add new fields in a layout of an
already existing DataSet.

By default, the real fields will have a value of 0 and the string
fields will have a value of \"\".

Parameters:
-----------

real:  the names of the fields of type real to be added.

string:  the names of the fields of type string to be added.

default:  a map containing the (optional) default values for the new
fields to be added.

size:  a map containing the (optional) default size for the new fields
to be added. If a field is specified here, it will be added as a
fixed- length field in the layout, otherwise it will be a variable-
length field.

C++ includes: addfield.h ";

%feature("docstring")  gaia2::AddField::AddField "gaia2::AddField::AddField(const ParameterMap &params) ";

%feature("docstring")  gaia2::AddField::~AddField "virtual
gaia2::AddField::~AddField() ";

%feature("docstring")  gaia2::AddField::analyze "Transformation
gaia2::AddField::analyze(const DataSet *data) const ";


// File: classgaia2_1_1AddFieldApplier.xml
%feature("docstring") gaia2::AddFieldApplier "

AddFieldApplier applier class.

Adds the specified fields to the current layout.

C++ includes: addfieldapplier.h ";

%feature("docstring")  gaia2::AddFieldApplier::AddFieldApplier "AddFieldApplier::AddFieldApplier(const Transformation &transfo) ";

%feature("docstring")  gaia2::AddFieldApplier::~AddFieldApplier "AddFieldApplier::~AddFieldApplier() ";

%feature("docstring")  gaia2::AddFieldApplier::mapPoint "Point *
AddFieldApplier::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";

%feature("docstring")  gaia2::AddFieldApplier::mapDataSet "DataSet *
AddFieldApplier::mapDataSet(const DataSet *dataset) const

Optimized version so that we don't have to recompute the index
mappings for each point. ";


// File: classgaia2_1_1Analyzer.xml
%feature("docstring") gaia2::Analyzer "

the Analyzer abstract base class.

If you want to implement a specific analyzer, you need to inherit from
this base class and implement either one of the 2 analyze methods.

Analyzer::analyze(DataSet*) just gives you the DataSet and you're good
to go, whereas Analyzer::analyze(DataSet*, Region&) gives you an
already parsed Region of descriptors as well (got from the
descriptorNames and exclude parameter.

: if you use the analyze(DataSet*) method, you should also call the
checkDataSet() method to make sure you were not given a null pointer!

C++ includes: analyzer.h ";

%feature("docstring")  gaia2::Analyzer::Analyzer "gaia2::Analyzer::Analyzer(const ParameterMap &params) ";

%feature("docstring")  gaia2::Analyzer::~Analyzer "virtual
gaia2::Analyzer::~Analyzer() ";

%feature("docstring")  gaia2::Analyzer::analyze "Transformation
gaia2::Analyzer::analyze(const DataSet *dataset) const ";

%feature("docstring")  gaia2::Analyzer::analyze "Transformation
gaia2::Analyzer::analyze(const DataSet *dataset, const Region &region)
const ";

%feature("docstring")  gaia2::Analyzer::checkDataSet "void
gaia2::Analyzer::checkDataSet(const DataSet *dataset) const

Checks that the given dataset is valid.

At the moment, it does: checks that it is not a null pointer

checks that all the points share the same layout memory object

Exceptions:
-----------

GaiaException if there was a problem with the dataset. ";

%feature("docstring")  gaia2::Analyzer::checkMinPoints "void
gaia2::Analyzer::checkMinPoints(const DataSet *dataset, int n) const

Checks that the given dataset as at least the specified number of
points.

Exceptions:
-----------

GaiaException if that is not the case. ";

%feature("docstring")  gaia2::Analyzer::checkFixedLength "const
Region & gaia2::Analyzer::checkFixedLength(const Region &region, const
PointLayout &layout) const

Checks that the given Region only contains fixed-length descriptors
and throws an exception if not.

Exceptions:
-----------

GaiaException if the region contained variable-length descriptors. ";


// File: classgaia2std_1_1Analyzer.xml
%feature("docstring") gaia2std::Analyzer "";

%feature("docstring")  gaia2std::Analyzer::Analyzer "gaia2std::Analyzer::Analyzer(const std::string &name, const
ParameterMap &params) ";

%feature("docstring")  gaia2std::Analyzer::analyze "proxy_ptr<
Transformation > gaia2std::Analyzer::analyze(const DataSet *data)
const ";


// File: classgaia2_1_1filter_1_1AndFilter.xml
%feature("docstring") gaia2::filter::AndFilter "";

%feature("docstring")  gaia2::filter::AndFilter::AndFilter "gaia2::filter::AndFilter::AndFilter(const QList< Filter *> &preds) ";

%feature("docstring")  gaia2::filter::AndFilter::AndFilter "gaia2::filter::AndFilter::AndFilter(const std::vector< Filter *>
&preds) ";


// File: classgaia2_1_1Applier.xml
%feature("docstring") gaia2::Applier "

The Applier abstract base class.

Derived classes will have to implement a constructor taking all its
arguments as a string, and a mapPoint method. They will also have to
set _originalDimension and _targetDimension in the constructor, as
well as the other specific params they need for themselves. A helper
function checkSize should be called at the very beginning of the
mapPoint method to ensure consistency in error reporting. Do not
forget as well to call the transferHistory() helper method at the end
of your mapDataSet() function, or you might end up with invalid
datasets (NB: it needs to be at the end, not at the beginning).

C++ includes: applier.h ";

%feature("docstring")  gaia2::Applier::Applier "gaia2::Applier::Applier(const Transformation &transfo) ";

%feature("docstring")  gaia2::Applier::~Applier "virtual
gaia2::Applier::~Applier() ";

%feature("docstring")  gaia2::Applier::checkLayout "void
gaia2::Applier::checkLayout(const PointLayout &layout) const

Check that the given layout is the same as the one that was used
during the analysis part of this transformation.

Exceptions:
-----------

GaiaException if the layout was different than the current one. ";

%feature("docstring")  gaia2::Applier::checkLayout "void
gaia2::Applier::checkLayout(const DataSet *dataset) const

Check that the given dataset has the same layout as the one that was
used during the analysis part of this transformation.

Also check that the dataset is consistent.

Exceptions:
-----------

GaiaException if the layout was different than the current one. ";

%feature("docstring")  gaia2::Applier::mapPoint "virtual Point*
gaia2::Applier::mapPoint(const Point *p) const =0

Ownership of resulting point is handed to caller of this function. ";

%feature("docstring")  gaia2::Applier::mapDataSet "DataSet *
gaia2::Applier::mapDataSet(const DataSet *dataset) const

Provides a default function that maps a whole dataset but which can be
overriden in derived classes to provide a more efficient
implementation. ";


// File: classgaia2_1_1Array.xml
%feature("docstring") gaia2::Array "";

%feature("docstring")  gaia2::Array::Array "gaia2::Array< T
>::Array() ";

%feature("docstring")  gaia2::Array::Array "gaia2::Array< T
>::Array(int size) ";

%feature("docstring")  gaia2::Array::Array "gaia2::Array< T
>::Array(int size, const T &value) ";

%feature("docstring")  gaia2::Array::at "const T& gaia2::Array< T
>::at(int i) const ";

%feature("docstring")  gaia2::Array::front "T& gaia2::Array< T
>::front() ";

%feature("docstring")  gaia2::Array::front "const T& gaia2::Array< T
>::front() const ";

%feature("docstring")  gaia2::Array::fill "Array& gaia2::Array< T
>::fill(const T &val, int size) ";

%feature("docstring")  gaia2::Array::size "int gaia2::Array< T
>::size() const ";

%feature("docstring")  gaia2::Array::constBegin "const T*
gaia2::Array< T >::constBegin() const ";

%feature("docstring")  gaia2::Array::begin "const T* gaia2::Array< T
>::begin() const ";

%feature("docstring")  gaia2::Array::begin "T* gaia2::Array< T
>::begin() ";

%feature("docstring")  gaia2::Array::constEnd "const T* gaia2::Array<
T >::constEnd() const ";

%feature("docstring")  gaia2::Array::end "const T* gaia2::Array< T
>::end() const ";

%feature("docstring")  gaia2::Array::end "T* gaia2::Array< T >::end()
";


// File: classgaia2_1_1BaseQueryOptimizer.xml
%feature("docstring") gaia2::BaseQueryOptimizer "

The QueryOptimizer class tries to optimize a query by reducing the
SearchSpace on which it is supposed to operate, mostly by the means of
the associated filter.

It should tap into a SearchSpace pool and return the final
SearchSpace, as well as the possibly modified Filter (contained inside
the SearchSpace), on which to perform the query.

C++ includes: queryoptimizer.h ";

%feature("docstring")  gaia2::BaseQueryOptimizer::BaseQueryOptimizer "gaia2::BaseQueryOptimizer< SearchPointType, DataSetType
>::BaseQueryOptimizer(SearchSpacePoolType *spool) ";

%feature("docstring")  gaia2::BaseQueryOptimizer::optimize "BaseSearchSpace< SearchPointType, DataSetType > *
gaia2::BaseQueryOptimizer< SearchPointType, DataSetType
>::optimize(Filter *filter)

Ownership of the SearchSpace is yielded to the caller of the function.

Ownership of the filter is given to this function (either it will be
destroyed and replaced by an optimized filter inside the SearchSpace,
or it will put directly into the SearchSpace. In any case, you
shouldn't try to delete it after having called this function). ";


// File: classgaia2_1_1BaseResultSet.xml
%feature("docstring") gaia2::BaseResultSet "

This class serves as a ref-counted wrapper for a SearchSpace, which is
much more convenient to deal with than having to remember who owns the
results or who's responsible for deleting it.

C++ includes: searchspace.h ";

%feature("docstring")  gaia2::BaseResultSet::BaseResultSet "BaseResultSet::BaseResultSet() ";

%feature("docstring")  gaia2::BaseResultSet::BaseResultSet "BaseResultSet::BaseResultSet(BaseSearchSpace< SearchPointType,
DataSetType > *sspace) ";

%feature("docstring")  gaia2::BaseResultSet::get "SearchResults
BaseResultSet::get(int n, int offset=0)

Returns the list of search results, which are pairs of (pointName,
distance).

Parameters:
-----------

n:  the number of desired results

offset:  the start offset ";

%feature("docstring")  gaia2::BaseResultSet::size "int
gaia2::BaseResultSet< SearchPointType, DataSetType >::size() const ";

%feature("docstring")  gaia2::BaseResultSet::limit "BaseResultSet
gaia2::BaseResultSet< SearchPointType, DataSetType >::limit(int n)

This function limits the number of results contained in this
ResultSet.

If there were fewer points before than the given number, the ResultSet
is left untouched, otherwise it will have a maximum of n points after
the call. Returns this object so it is possible to chain this method
call with a get() one. ";

%feature("docstring")  gaia2::BaseResultSet::thresholdLimit "BaseResultSet gaia2::BaseResultSet< SearchPointType, DataSetType
>::thresholdLimit(float maxDist)

This method limits the number of results contained in this ResultSet.

All the results which have a distance higher than the specified
maxDist will be discarded from this SearchSpace. Returns this object
so it is possible to chain this method call with a get() one. ";

%feature("docstring")  gaia2::BaseResultSet::addPoints "void
BaseResultSet::addPoints(const DataSetType *dataset, const QList<
QString > &ids) ";

%feature("docstring")  gaia2::BaseResultSet::removePoints "void
BaseResultSet::removePoints(const QList< QString > &ids) ";

%feature("docstring")  gaia2::BaseResultSet::searchSpace "BaseSearchSpace<SearchPointType, DataSetType>* gaia2::BaseResultSet<
SearchPointType, DataSetType >::searchSpace() ";


// File: classgaia2_1_1BaseSearchSpace.xml
%feature("docstring") gaia2::BaseSearchSpace "

A SearchSpace is a structure dedicated to the task of storing pointers
to Point with an associated distance and is the structure on which the
nearest-neighbours queries are performed.

It is also what is returned by the view as a search result, and allows
to get any number of points with a given offset if desired.

Some pre-indexed SearchSpace instances will reside in a SearchSpace
pool which is owned by a specific View. It acts as an index, and the
pool has a way to either add more of those, or return a given subset
that satisfies certain conditions. As such, when a View needs to be
revalidated, it will also revalidate all linked SearchSpace.

WARNING: As we make copies of SearchSpace to perform queries, it is
perfectly safe to make multithreaded queries. However, it is NOT safe
to add or remove points while there is still a query being processed,
or a SearchSpace returned from a previous query still in use (ie: View
instances revalidate themselves automatically when a DataSet changes,
but SearchSpace ones don't)

C++ includes: searchspace.h ";

%feature("docstring")  gaia2::BaseSearchSpace::BaseSearchSpace "gaia2::BaseSearchSpace< SearchPointType, DataSetType
>::BaseSearchSpace(const Filter *filter=0, bool ownsFilter=false) ";

%feature("docstring")  gaia2::BaseSearchSpace::BaseSearchSpace "BaseSearchSpace::BaseSearchSpace(const BaseSearchSpace<
SearchPointType, DataSetType > &other)

Copy constructor.

WARNING: this does not copy the filter nor any other member variables,
only the points contained in the SearchSpace. ";

%feature("docstring")  gaia2::BaseSearchSpace::~BaseSearchSpace "BaseSearchSpace::~BaseSearchSpace() ";

%feature("docstring")  gaia2::BaseSearchSpace::copyPointsFrom "void
BaseSearchSpace::copyPointsFrom(const BaseSearchSpace<
SearchPointType, DataSetType > &other)

Copy all the points from the other SearchSpace into this one.

WARNING: this does not copy the filter nor any other member variables,
only the points contained in the SearchSpace. ";

%feature("docstring")  gaia2::BaseSearchSpace::size "int
BaseSearchSpace::size() const

Returns the total number of points contained in this SearchSpace. ";

%feature("docstring")  gaia2::BaseSearchSpace::unfilteredSize "int
BaseSearchSpace::unfilteredSize() const

Returns the size of this SearchSpace, before filtering the points.

WARNING: use with care, this value might not be consistent with what
you expect. ";

%feature("docstring")  gaia2::BaseSearchSpace::clear "void
BaseSearchSpace::clear()

Clears the whole SearchSpace. ";

%feature("docstring")  gaia2::BaseSearchSpace::get "SearchResults
BaseSearchSpace::get(int n, int offset=0)

Returns the list of search results, which are pairs of (pointName,
distance).

Parameters:
-----------

n:  the number of desired results

offset:  the start offset ";

%feature("docstring")  gaia2::BaseSearchSpace::limit "void
BaseSearchSpace::limit(int n)

This method limits the number of results contained in this
SearchSpace.

If there were fewer points before than the given number, the
SearchSpace is left untouched, otherwise it will have a maximum of n
points after the call. ";

%feature("docstring")  gaia2::BaseSearchSpace::thresholdLimit "void
BaseSearchSpace::thresholdLimit(float maxDist)

This method limits the bymber of results contained in this
SearchSpace.

All the results which have a distance higher than the specified
maxDist will be discarded from this SearchSpace. ";

%feature("docstring")  gaia2::BaseSearchSpace::filter "const Filter*
gaia2::BaseSearchSpace< SearchPointType, DataSetType >::filter() ";

%feature("docstring")  gaia2::BaseSearchSpace::setFilter "void
BaseSearchSpace::setFilter(const Filter *filter, bool
ownsFilter=false) ";

%feature("docstring")  gaia2::BaseSearchSpace::invalidate "void
gaia2::BaseSearchSpace< SearchPointType, DataSetType >::invalidate()

Mark this SearchSpace as neither sorted nor filtered. ";

%feature("docstring")  gaia2::BaseSearchSpace::filterAll "void
BaseSearchSpace::filterAll()

Filters all points so that the remaining points at the end all comply
to the Filter. ";

%feature("docstring")  gaia2::BaseSearchSpace::pointerSort "void
BaseSearchSpace::pointerSort()

Sorts the SearchPoints contained into this SearchSpace by order of
their pointer address. ";

%feature("docstring")  gaia2::BaseSearchSpace::addPoints "void
BaseSearchSpace::addPoints(const DataSetType *dataset, const QList<
QString > &ids)

Add the points from the dataset with the given IDs to this
SearchSpace. ";

%feature("docstring")  gaia2::BaseSearchSpace::removePoints "void
BaseSearchSpace::removePoints(const QList< QString > &ids)

Remove the points with the given IDs from this SearchSpace. ";

%feature("docstring")  gaia2::BaseSearchSpace::setIntersection "void
BaseSearchSpace::setIntersection(const BaseSearchSpace<
SearchPointType, DataSetType > *other)

This computes the intersection of this SearchSpace with the other one.

NB: This function assumes that both SearchSpaces have their points
ordered by pointer address. ";

%feature("docstring")  gaia2::BaseSearchSpace::setUnion "void
BaseSearchSpace::setUnion(const BaseSearchSpace< SearchPointType,
DataSetType > *other)

This computes the union of this SearchSpace with the other one.

NB: This function assumes that both SearchSpaces have their points
ordered by pointer address. ";

%feature("docstring")  gaia2::BaseSearchSpace::setDifference "void
BaseSearchSpace::setDifference(const BaseSearchSpace< SearchPointType,
DataSetType > *other)

This computes the difference of this SearchSpace and the other one
(ie: this - other).

NB: This function assumes that both SearchSpaces have their points
ordered by pointer address. ";

%feature("docstring")  gaia2::BaseSearchSpace::pointName "QString
gaia2::BaseSearchSpace< SearchPointType, DataSetType
>::pointName(const SearchPointType &p) const ";

%feature("docstring")  gaia2::BaseSearchSpace::refPoint "const Point*
gaia2::BaseSearchSpace< SearchPointType, DataSetType >::refPoint(const
SearchPointType &p) const ";

%feature("docstring")  gaia2::BaseSearchSpace::validPoint "bool
gaia2::BaseSearchSpace< SearchPointType, DataSetType
>::validPoint(const SearchPointType &p) const ";

%feature("docstring")  gaia2::BaseSearchSpace::computeDistance "void
gaia2::BaseSearchSpace< SearchPointType, DataSetType
>::computeDistance(const PointType &query, const DistanceType *dist)

Compute the distance from the given query point to all those contained
in this SearchSpace, and presort a few points to have near instant
access to them. ";

%feature("docstring")  gaia2::BaseSearchSpace::setDataSet "void
gaia2::BaseSearchSpace< SearchPointType, DataSetType
>::setDataSet(const DataSetType *dataset) ";

%feature("docstring")  gaia2::BaseSearchSpace::dataSet "const
DataSetType* gaia2::BaseSearchSpace< SearchPointType, DataSetType
>::dataSet() const ";

%feature("docstring")  gaia2::BaseSearchSpace::filterAndSort "void
BaseSearchSpace::filterAndSort(int n=1000)

Filters the points in this SearchSpace using the given Filter and sort
them by increasing distance.

Parameters:
-----------

n:  only filter and sort the n points with the smallest distance, this
has the advantage of being much faster than sorting all the points. If
n <= 0, then all the points will be sorted. ";

%feature("docstring")  gaia2::BaseSearchSpace::cleanSearchSpace "void
BaseSearchSpace::cleanSearchSpace() ";


// File: classgaia2_1_1BaseSearchSpacePool.xml
%feature("docstring") gaia2::BaseSearchSpacePool "

The SearchSpacePool accomplishes 2 main functions:

it holds references to precomputed SearchSpaces which correspond to
indexed descriptors

it acts as a pool of preallocated SearchSpaces, to avoid the cost of
creating / deleting them each time.

C++ includes: searchspacepool.h ";

%feature("docstring")  gaia2::BaseSearchSpacePool::BaseSearchSpacePool
"BaseSearchSpacePool::BaseSearchSpacePool(const DataSetType *dataset)

By default, the SearchSpacePool constructor takes a DataSet which it
will use for creating indices.

It is necessary to call the indexOn() method explicitly, though. ";

%feature("docstring")
gaia2::BaseSearchSpacePool::~BaseSearchSpacePool "BaseSearchSpacePool::~BaseSearchSpacePool() ";

%feature("docstring")  gaia2::BaseSearchSpacePool::recreate "void
BaseSearchSpacePool::recreate()

Recreates the SearchSpacePool and all the indices (needed for instance
when the dataset it is pointing to has been changed). ";

%feature("docstring")  gaia2::BaseSearchSpacePool::getAllPoints "BaseSearchSpace< SearchPointType, DataSetType > *
BaseSearchSpacePool::getAllPoints() const

Returns a SearchSpace containing all points.

FIXME: implementation needs to get a space from ::acquire() instead of
creating a new one each time ";

%feature("docstring")  gaia2::BaseSearchSpacePool::indexOn "void
BaseSearchSpacePool::indexOn(const QString &descriptorName)

Indexes on the given descriptorName.

Queries which then use this descriptor should be faster. ";

%feature("docstring")  gaia2::BaseSearchSpacePool::hasIndex "bool
BaseSearchSpacePool::hasIndex(const QString &descriptorName) const

Returns whether an index exists for the given descriptorName. ";

%feature("docstring")  gaia2::BaseSearchSpacePool::getSubSpace "BaseSearchSpace< SearchPointType, DataSetType > *
BaseSearchSpacePool::getSubSpace(const parser::Predicate *pred) const
";

%feature("docstring")  gaia2::BaseSearchSpacePool::dataset "const
DataSetType* gaia2::BaseSearchSpacePool< SearchPointType, DataSetType
>::dataset() const ";


// File: classgaia2_1_1BaseView.xml
%feature("docstring") gaia2::BaseView "

The View class contains a specific view on a dataset.

It has a specific distance function and can also work on subsets of
the whole dataset. A dataset can then be considered as a collection of
different datasets and can query any, some or all of them for
similarity searches.

C++ includes: view.h ";

%feature("docstring")  gaia2::BaseView::BaseView "gaia2::BaseView<
DataSetType, PointType, SearchPointType, DistanceType
>::BaseView(DataSetType *dataset) ";

%feature("docstring")  gaia2::BaseView::~BaseView "gaia2::BaseView<
DataSetType, PointType, SearchPointType, DistanceType >::~BaseView()
";

%feature("docstring")  gaia2::BaseView::nnSearch "BaseResultSet<
SearchPointType, DataSetType > gaia2::BaseView< DataSetType,
PointType, SearchPointType, DistanceType >::nnSearch(const PointType
&p, const DistanceType *dist, const Filter *filter=0) ";

%feature("docstring")  gaia2::BaseView::nnSearch "BaseResultSet<
SearchPointType, DataSetType > gaia2::BaseView< DataSetType,
PointType, SearchPointType, DistanceType >::nnSearch(const PointType
&p, const DistanceType *dist, const QString &filter) ";

%feature("docstring")  gaia2::BaseView::nnSearch "BaseResultSet<
SearchPointType, DataSetType > gaia2::BaseView< DataSetType,
PointType, SearchPointType, DistanceType >::nnSearch(const PointType
&p, ResultSetType inputSpace, const DistanceType *dist, const Filter
*filter=0) ";

%feature("docstring")  gaia2::BaseView::nnSearch "BaseResultSet<
SearchPointType, DataSetType > gaia2::BaseView< DataSetType,
PointType, SearchPointType, DistanceType >::nnSearch(const PointType
&p, ResultSetType inputSpace, const DistanceType *dist, const QString
&filter) ";

%feature("docstring")  gaia2::BaseView::nnSearch "ResultSetType
gaia2::BaseView< DataSetType, PointType, SearchPointType, DistanceType
>::nnSearch(const QString &id, T x) ";

%feature("docstring")  gaia2::BaseView::nnSearch "ResultSetType
gaia2::BaseView< DataSetType, PointType, SearchPointType, DistanceType
>::nnSearch(const QString &id, T x, U y) ";

%feature("docstring")  gaia2::BaseView::nnSearch "ResultSetType
gaia2::BaseView< DataSetType, PointType, SearchPointType, DistanceType
>::nnSearch(const QString &id, T x, U y, V z) ";

%feature("docstring")  gaia2::BaseView::invalidate "void
gaia2::BaseView< DataSetType, PointType, SearchPointType, DistanceType
>::invalidate()

This function is called whenever the DataSet it points to changes, so
it knows it needs to recompute SortingStruct, maybe its filters,
etc... ";

%feature("docstring")  gaia2::BaseView::validate "void
gaia2::BaseView< DataSetType, PointType, SearchPointType, DistanceType
>::validate()

This function is called to make sure that the View is valid (ie: its
sorting structure and filters actually point to existing objects). ";

%feature("docstring")  gaia2::BaseView::indexOn "void
gaia2::BaseView< DataSetType, PointType, SearchPointType, DistanceType
>::indexOn(const QString &descriptorName)

This methods creates an index on the given descriptor name, in the
hope that future queries using this descriptor in a filter will be
faster. ";


// File: classgaia2_1_1parser_1_1BooleanConstant.xml
%feature("docstring") gaia2::parser::BooleanConstant "";

%feature("docstring")  gaia2::parser::BooleanConstant::BooleanConstant
"gaia2::parser::BooleanConstant::BooleanConstant(bool value) ";

%feature("docstring")  gaia2::parser::BooleanConstant::copy "virtual
Predicate* gaia2::parser::BooleanConstant::copy() const ";

%feature("docstring")  gaia2::parser::BooleanConstant::value "virtual
bool gaia2::parser::BooleanConstant::value() const ";

%feature("docstring")  gaia2::parser::BooleanConstant::toString "virtual QString gaia2::parser::BooleanConstant::toString() const ";


// File: classgaia2_1_1BPFDistance_1_1BPF.xml


// File: classgaia2_1_1BPFDistance.xml
%feature("docstring") gaia2::BPFDistance "

This class composes the breakpoint function specified by its pairs of
(split point, value) with the given distance.

Parameters:
-----------

distance:  the name of the distance to apply the BPF to.

params:  the parameters for creating the distance we compose with.

breakPoints:  the pairs (split point, value) that define the BPF.

C++ includes: bpfdistance.h ";

%feature("docstring")  gaia2::BPFDistance::BPFDistance "BPFDistance::BPFDistance(const PointLayout &layout, const ParameterMap
&params) ";

%feature("docstring")  gaia2::BPFDistance::~BPFDistance "BPFDistance::~BPFDistance() ";


// File: structgaia2_1_1DescriptorTree_1_1CachedValues.xml


// File: classgaia2_1_1Center.xml
%feature("docstring") gaia2::Center "

The Center analyzer centers the points around the mean of each
descriptor.

(it substracts the mean of a each dimension to all instances).

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
normalization. Only accepts real descriptors, can use wildcards.
(default: \"*\").

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

C++ includes: center.h ";

%feature("docstring")  gaia2::Center::Center "gaia2::Center::Center(const ParameterMap &params) ";

%feature("docstring")  gaia2::Center::~Center "virtual
gaia2::Center::~Center() ";

%feature("docstring")  gaia2::Center::analyze "Transformation
gaia2::Center::analyze(const DataSet *data, const Region &region)
const ";


// File: classgaia2_1_1Cleaner.xml
%feature("docstring") gaia2::Cleaner "

The Cleaner analyzer checks for values that may cause problems when
doing mathematical analysis (such as normalize, pca, ...), and removes
them.

This analyzer checks for Real descriptors which have either Infinite
or NaN values, and the ones that are constant (< 1e-7, actually, hence
they can not be normalized, and do not add any information anyway),
and returns a transformation that removes them.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
checking. Only accepts real descriptors, can use wildcards. (default:
\"*\").

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

C++ includes: cleaner.h ";

%feature("docstring")  gaia2::Cleaner::Cleaner "gaia2::Cleaner::Cleaner(const ParameterMap &params) ";

%feature("docstring")  gaia2::Cleaner::~Cleaner "virtual
gaia2::Cleaner::~Cleaner() ";

%feature("docstring")  gaia2::Cleaner::analyze "Transformation
gaia2::Cleaner::analyze(const DataSet *data, const Region &region)
const ";


// File: classgaia2_1_1CosineAngleDistance.xml
%feature("docstring") gaia2::CosineAngleDistance "

This class computes the angle in radians between 2 points and divides
it by Pi (so that it's normalized between 0 and 1).

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
computation of the distance. Only accepts real descriptors, can use
wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards.

defaultValue:  when 1 of the 2 points is null, then the angle is not
defined. If defaultValue is given, then it will be returned, otherwise
an exception will be thrown.

C++ includes: cosineangledistance.h ";

%feature("docstring")  gaia2::CosineAngleDistance::CosineAngleDistance
"CosineAngleDistance::CosineAngleDistance(const PointLayout &layout,
const ParameterMap &params) ";

%feature("docstring")
gaia2::CosineAngleDistance::~CosineAngleDistance "CosineAngleDistance::~CosineAngleDistance() ";


// File: classgaia2_1_1CosineDistance.xml
%feature("docstring") gaia2::CosineDistance "";

%feature("docstring")  gaia2::CosineDistance::CosineDistance "gaia2::CosineDistance::CosineDistance(int dimension) ";


// File: classgaia2_1_1CosineSimilarity.xml
%feature("docstring") gaia2::CosineSimilarity "

This class computes the cosine similarity of a given Region.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
computation of the distance. Only accepts real descriptors, can use
wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards.

defaultValue:  when 1 of the 2 points is null, then the similarity is
not defined. If defaultValue is given, then it will be returned,
otherwise an exception will be thrown.

C++ includes: cosinesimilarity.h ";

%feature("docstring")  gaia2::CosineSimilarity::CosineSimilarity "CosineSimilarity::CosineSimilarity(const PointLayout &layout, const
ParameterMap &params) ";


// File: classcounted__ptr.xml
%feature("docstring") counted_ptr "";

%feature("docstring")  counted_ptr::counted_ptr "counted_ptr< X
>::counted_ptr(X *p=0) ";

%feature("docstring")  counted_ptr::~counted_ptr "counted_ptr< X
>::~counted_ptr() ";

%feature("docstring")  counted_ptr::counted_ptr "counted_ptr< X
>::counted_ptr(const counted_ptr &r)  throw ()";

%feature("docstring")  counted_ptr::get "X* counted_ptr< X >::get()
const  throw ()";

%feature("docstring")  counted_ptr::unique "bool counted_ptr< X
>::unique() const  throw ()";


// File: classgaia2std_1_1counted__ptr.xml
%feature("docstring") gaia2std::counted_ptr "";

%feature("docstring")  gaia2std::counted_ptr::counted_ptr "gaia2std::counted_ptr< X >::counted_ptr(X *p=0) ";

%feature("docstring")  gaia2std::counted_ptr::~counted_ptr "gaia2std::counted_ptr< X >::~counted_ptr() ";

%feature("docstring")  gaia2std::counted_ptr::counted_ptr "gaia2std::counted_ptr< X >::counted_ptr(const counted_ptr &r)  throw
()";

%feature("docstring")  gaia2std::counted_ptr::get "X*
gaia2std::counted_ptr< X >::get() const  throw ()";

%feature("docstring")  gaia2std::counted_ptr::unique "bool
gaia2std::counted_ptr< X >::unique() const  throw ()";


// File: structcounted__ptr_1_1counter.xml


// File: structgaia2std_1_1counted__ptr_1_1counter.xml


// File: classgaia2_1_1CrossSegmentDistance.xml
%feature("docstring") gaia2::CrossSegmentDistance "

This class calculates a distance between segments from two points.

The function used to calculate the distance between two segments is
given by the distance parameter. What it does is the following: for a
given segment of p1, it computes its distance with all the segments of
p2, then uses func1 to aggregate the results.

it does the previous for all segments of p1, then uses func2 to
aggregate the results.

WARNING: this metric is not final, and it might change its behavior in
the future.

Parameters:
-----------

distance:  the name of the distance to be composed with.

params:  the parameters for creating the distance.

func1:  the first aggregation function (min, max or mean).

func2:  the second aggregation function (min, max or mean).

C++ includes: crosssegmentdistance.h ";

%feature("docstring")
gaia2::CrossSegmentDistance::CrossSegmentDistance "CrossSegmentDistance::CrossSegmentDistance(const PointLayout &layout,
const ParameterMap &params) ";

%feature("docstring")
gaia2::CrossSegmentDistance::~CrossSegmentDistance "CrossSegmentDistance::~CrossSegmentDistance() ";


// File: classgaia2std_1_1DataSet.xml
%feature("docstring") gaia2std::DataSet "";

%feature("docstring")  gaia2std::DataSet::DataSet "gaia2std::DataSet::DataSet() ";

%feature("docstring")  gaia2std::DataSet::name "string
gaia2std::DataSet::name() const ";

%feature("docstring")  gaia2std::DataSet::setName "void
gaia2std::DataSet::setName(const std::string &name) ";

%feature("docstring")  gaia2std::DataSet::setReferenceDataSet "void
gaia2std::DataSet::setReferenceDataSet(DataSet *dataset) ";

%feature("docstring")  gaia2std::DataSet::pointNames "vector< string
> gaia2std::DataSet::pointNames() const ";

%feature("docstring")  gaia2std::DataSet::point "proxy_ptr< const
Point > gaia2std::DataSet::point(const std::string &id) const ";

%feature("docstring")  gaia2std::DataSet::point "proxy_ptr< Point >
gaia2std::DataSet::point(const std::string &id) ";

%feature("docstring")  gaia2std::DataSet::addPoint "void
gaia2std::DataSet::addPoint(Point *point) ";

%feature("docstring")  gaia2std::DataSet::removePoint "void
gaia2std::DataSet::removePoint(const std::string &id) ";

%feature("docstring")  gaia2std::DataSet::empty "bool
gaia2std::DataSet::empty() const ";

%feature("docstring")  gaia2std::DataSet::size "int
gaia2std::DataSet::size() const ";

%feature("docstring")  gaia2std::DataSet::load "void
gaia2std::DataSet::load(const std::string &filename) ";

%feature("docstring")  gaia2std::DataSet::save "void
gaia2std::DataSet::save(const std::string &filename) ";


// File: classgaia2_1_1DataSet.xml
%feature("docstring") gaia2::DataSet "

This class represents a dataset and all related information.

A DataSet is a set of points which all share the same structure. The
points are sorted internally to allow fast lookups in O(log(N)).

The structure contains information such as dimension names, a pointer
to the original dataset, the history of the applied transformations,
and a point layout which maps: names of dimensions <-> indices inside
the point data.

The DataSet also provides methods for adding & removing points,
merging with another DataSet, and serialization functions.

C++ includes: dataset.h ";

%feature("docstring")  gaia2::DataSet::DataSet "DataSet::DataSet() ";

%feature("docstring")  gaia2::DataSet::~DataSet "DataSet::~DataSet()
";

%feature("docstring")  gaia2::DataSet::name "const QString&
gaia2::DataSet::name() const

Return the name of this dataset. ";

%feature("docstring")  gaia2::DataSet::setName "void
gaia2::DataSet::setName(const QString &name)

Set the name for this dataset. ";

%feature("docstring")  gaia2::DataSet::point "const Point *
DataSet::point(const QString &id) const

Find a point with a given ID.

the point with the given ID

Exceptions:
-----------

GaiaException when the point was not found in the dataset ";

%feature("docstring")  gaia2::DataSet::point "Point *
DataSet::point(const QString &id)

Find a point with a given ID.

the point with the given ID

Exceptions:
-----------

GaiaException when the point was not found in the dataset ";

%feature("docstring")  gaia2::DataSet::contains "bool
DataSet::contains(const QString &id) const

Return whether this dataset contains a point with the given ID. ";

%feature("docstring")  gaia2::DataSet::pointNames "QStringList
DataSet::pointNames() const

Return a list of the names of the points contained in this dataset. ";

%feature("docstring")  gaia2::DataSet::layout "const PointLayout&
gaia2::DataSet::layout() const

Return the layout of this dataset. ";

%feature("docstring")  gaia2::DataSet::originalLayout "const
PointLayout & DataSet::originalLayout() const

Return the original layout of this dataset. ";

%feature("docstring")  gaia2::DataSet::checkAllPointsShareSameLayout "void DataSet::checkAllPointsShareSameLayout(const QVector< Point *>
*points=0) const

Check that all given points have the same layout object as this
dataset.

If no points are given, it will take those from this dataset.

Exceptions:
-----------

GaiaException if there are some points with a different layout object
";

%feature("docstring")  gaia2::DataSet::history "const TransfoChain&
gaia2::DataSet::history() const

Return the history of this dataset (the list of all transformations
that have been applied). ";

%feature("docstring")  gaia2::DataSet::setHistory "void
DataSet::setHistory(const TransfoChain &history)

Set a predefined history for this dataset.

This only works on empty datasets, as it is forbidden to change the
history of preexisting points/datasets.

This can be useful in the case where you copy points from one dataset
into another, and want to preserve their history. ";

%feature("docstring")  gaia2::DataSet::forgetHistory "void
DataSet::forgetHistory()

Reset the history to an empty one but does not touch any of the
points. ";

%feature("docstring")  gaia2::DataSet::simplifyHistory "void
DataSet::simplifyHistory()

Simplify the history of the transformations to have it in a
\"normalized\" state, which consist at most of a Remove transformation
followed by a FixLength one.

You can only call this method on a DataSet whose history contains the
following allowed transformations: [ Select, Remove, RemoveVL,
Cleaner, FixLength ]. ";

%feature("docstring")  gaia2::DataSet::setReferenceDataSet "void
DataSet::setReferenceDataSet(DataSet *dataset=0, bool
checkOriginalLayout=true)

Set the reference dataset, ie: the one used for fetching the values
when doing filtered queries.

Passing it a null pointer (or no argument) will set the calling
dataset as a reference dataset. It is highly recommended to always
check for the original layout, as it is impossible to add a point to 2
linked datasets that don't have the same original layout. ";

%feature("docstring")  gaia2::DataSet::referenceDataSet "const
DataSet * DataSet::referenceDataSet() const

Return the reference dataset.

A dataset always has a reference dataset, where it looks for the
values used when filtering queries. If no dataset has been set as a
reference dataset, the current dataset will be used. ";

%feature("docstring")  gaia2::DataSet::addPoint "void
DataSet::addPoint(const Point *point)

Add the specified point to this dataset.

The dataset makes a copy of the point for its own use, so no ownership
is taken. If you don't want the point anymore after having added it to
the dataset, it is your responsibility to free the memory for it.

Exceptions:
-----------

GaiaException if there was already a point with the same ID in this
dataset. ";

%feature("docstring")  gaia2::DataSet::addPoints "void
DataSet::addPoints(const QVector< Point *> &points)

Add the specified points to this dataset.

The dataset makes a copy of the points for its own use, so no
ownership is taken. This method is equivalent to calling addPoint
repeatedly, but is much faster.

Exceptions:
-----------

GaiaException if adding all points would result in a dataset with
duplicates. ";

%feature("docstring")  gaia2::DataSet::appendDataSet "void
DataSet::appendDataSet(const DataSet *dataset)

Append the points from the second dataset to the first one.

They must have the same layout and transformation history for this to
work.

Exceptions:
-----------

GaiaException if there were duplicate IDs in the 2 dataset, if the
layouts were not the same or if the transformation histories were not
the same. ";

%feature("docstring")  gaia2::DataSet::removePoint "void
DataSet::removePoint(const QString &id)

Remove a single point from the dataset given its ID.

Exceptions:
-----------

GaiaException if no point could be found with this ID. ";

%feature("docstring")  gaia2::DataSet::removePoints "void
DataSet::removePoints(const QList< QString > &ids)

Remove a list of points from the dataset given their IDs.

This is much faster than calling DataSet::removePoint() repeatedly.

Exceptions:
-----------

GaiaException if at least one ID could not be found. In that case, no
points will have been removed. ";

%feature("docstring")  gaia2::DataSet::addView "void
DataSet::addView(View *view)

Register a view on this dataset.

Registered Views are notified when the underlying dataset change (ie:
points are added, removed, ...). ";

%feature("docstring")  gaia2::DataSet::removeView "void
DataSet::removeView(View *view)

Remove a View from the list of registered views for this dataset. ";

%feature("docstring")  gaia2::DataSet::copy "DataSet *
DataSet::copy() const

Performs a deep copy of this dataset (ie: the contained points are
copied as well). ";

%feature("docstring")  gaia2::DataSet::load "void DataSet::load(const
QString &filename, int start=0, int end=-1)

Load a dataset from disk.

A value of end < 0 means that we should load all the points.

Parameters:
-----------

filename:  the path to the dataset file

start:  index of the first point to be loaded

end:  the index of the last point to be loaded + 1 (ie: past iterator)
";

%feature("docstring")  gaia2::DataSet::loadNthPart "void
DataSet::loadNthPart(const QString &filename, int idx=0, int total=1)

Load the n-th part of a dataset from disk.

Parameters:
-----------

filename:  the path to the dataset file

idx:  the index of the part to be loaded (0 <= idx < total)

total:  the number of parts in which the dataset should be split. ";

%feature("docstring")  gaia2::DataSet::save "void DataSet::save(const
QString &filename) const

Save this dataset to disk. ";

%feature("docstring")  gaia2::DataSet::fromBase64 "void
DataSet::fromBase64(const std::string &data)

Load a DataSet from its base64 representation. ";

%feature("docstring")  gaia2::DataSet::fromBase64 "void
DataSet::fromBase64(const QByteArray &data)

Load a DataSet from its base64 representation. ";

%feature("docstring")  gaia2::DataSet::toBase64 "std::string
DataSet::toBase64() const

Return a base64 representation for this DataSet. ";


// File: classgaia2_1_1DescCompare.xml
%feature("docstring") gaia2::DescCompare "

Compare the descriptor names using the following criteria:

Fixed-Length come before Variable-Length

within the same length type, descriptors are sorted alphabetically

C++ includes: algoutils.h ";

%feature("docstring")  gaia2::DescCompare::DescCompare "gaia2::DescCompare::DescCompare(const PointLayout &layout) ";


// File: classgaia2_1_1DescriptorTree.xml
%feature("docstring") gaia2::DescriptorTree "C++ includes:
descriptortree.h ";

%feature("docstring")  gaia2::DescriptorTree::DescriptorTree "DescriptorTree::DescriptorTree() ";

%feature("docstring")  gaia2::DescriptorTree::DescriptorTree "DescriptorTree::DescriptorTree(const QString &name, DescriptorType
type, DescriptorLengthType ltype=VariableLength, int size=1) ";

%feature("docstring")  gaia2::DescriptorTree::~DescriptorTree "DescriptorTree::~DescriptorTree() ";

%feature("docstring")  gaia2::DescriptorTree::subtreeOf "bool
DescriptorTree::subtreeOf(const DescriptorTree *other) const

Returns whether this tree is a subset of the given one (e.g.

: all the branches and leaves of this tree are also contained in the
given one). ";

%feature("docstring")  gaia2::DescriptorTree::copy "DescriptorTree *
DescriptorTree::copy() const

Make a deep-copy of this region-tree.

If this operation is called on a node which is not the root of the
tree in which it belongs, it will throw an exception. You need to call
this function if you want to make a copy of a DescriptorTree as the
copy-constructor is protected. ";

%feature("docstring")  gaia2::DescriptorTree::clear "void
DescriptorTree::clear()

Resets this tree to a single root node.

Deletes all the children it had before. ";

%feature("docstring")  gaia2::DescriptorTree::name "QString
gaia2::DescriptorTree::name() const

Returns the name of the current node. ";

%feature("docstring")  gaia2::DescriptorTree::fullName "QString
DescriptorTree::fullName(bool recurseDown=true) const

Returns the fully-qualified name of this node, eg: using the names of
the nodes from the root down to this one.

Parameters:
-----------

recurseDown:  if this node only leads to a single leaf node and
recurseDown is true, then this method returns the full name of the
leaf node; otherwise it returns the full name of the current node. ";

%feature("docstring")  gaia2::DescriptorTree::type "DescriptorType
DescriptorTree::type() const

Returns the type of this node.

This method will throw an exception if it is not called on a leaf
(terminal node). ";

%feature("docstring")  gaia2::DescriptorTree::lengthType "DescriptorLengthType DescriptorTree::lengthType() const

Returns the length type of this node.

This method will throw an exception if it is not called on a leaf
(terminal node). ";

%feature("docstring")  gaia2::DescriptorTree::parent "const
DescriptorTree* gaia2::DescriptorTree::parent() const ";

%feature("docstring")  gaia2::DescriptorTree::parent "DescriptorTree*
gaia2::DescriptorTree::parent() ";

%feature("docstring")  gaia2::DescriptorTree::isLeaf "bool
gaia2::DescriptorTree::isLeaf() const

Returns whether this node is a leaf (terminal node) or not. ";

%feature("docstring")  gaia2::DescriptorTree::root "const
DescriptorTree * DescriptorTree::root() const

Returns the root of the tree which this node pertains to.

Const version. ";

%feature("docstring")  gaia2::DescriptorTree::root "DescriptorTree*
gaia2::DescriptorTree::root()

Returns the root of the tree which this node pertains to. ";

%feature("docstring")  gaia2::DescriptorTree::addChild "void
DescriptorTree::addChild(DescriptorTree *t)

Add the given DescriptorTree to this node's children.

Takes ownership of the child passed as argument. ";

%feature("docstring")  gaia2::DescriptorTree::addChild "bool
DescriptorTree::addChild(const QString &name, DescriptorType type,
DescriptorLengthType ltype=VariableLength, int size=1)

Add a child with specified name, type and lengthType.

If ltype = VariableLength, then the size argument has to be 1. If
child already existed before return false (and do nothing), else if
the new child was succesfully inserted, return true. ";

%feature("docstring")  gaia2::DescriptorTree::removeChild "void
DescriptorTree::removeChild(DescriptorTree *child)

Removes given DescriptorTree from this node's children.

This method throws an exception if the given tree was not a child of
this node. ";

%feature("docstring")  gaia2::DescriptorTree::removeNode "void
DescriptorTree::removeNode(const QString &name, bool recurse=true)

Remove the node with the given fully-qualified name.

If recurse is true, and the node removed was the last child of its
parent, then also remove the parent (and go on recursively until the
node removed wasn't the last child). ";

%feature("docstring")  gaia2::DescriptorTree::removeNodes "void
DescriptorTree::removeNodes(const QStringList &names, bool
recurse=true)

Remove the list of nodes with the given fully-qualified name.

If recurse is true and a removed node was the last child of its
parent, then also remove the parent (and go on recursively until the
node removed wasn't the last child). ";

%feature("docstring")  gaia2::DescriptorTree::fixLength "void
DescriptorTree::fixLength(int length)

Sets this descriptor to be of the specified fixed length.

This method will throw an exception if this DescriptorTree does not
correspond to a single descriptor (ie: is not a leaf). ";

%feature("docstring")  gaia2::DescriptorTree::enumerate "void
DescriptorTree::enumerate()

Sets this descriptor to be an enumeration.

This method will throw an exception if this DescriptorTree does not
correspond to a single descriptor of if it is not of type StringType.
";

%feature("docstring")  gaia2::DescriptorTree::descriptorName "QString
DescriptorTree::descriptorName(DescriptorType type,
DescriptorLengthType ltype, int index) const

Returns the name of the descriptor which is located at given position.
";

%feature("docstring")  gaia2::DescriptorTree::descriptorNames "QStringList DescriptorTree::descriptorNames() const

Returns a list of all the descriptor names contained in this layout.

If this layout contains only the root node, the returned list is
empty. ";

%feature("docstring")  gaia2::DescriptorTree::correspondingRegion "Region DescriptorTree::correspondingRegion() const

Returns a Region formed by the merged segments of all the children of
this node. ";

%feature("docstring")  gaia2::DescriptorTree::updateLayout "void
DescriptorTree::updateLayout()

This function takes all the segments of the current DescriptorTree,
orders them by type and by alphabetical order, and assign new Segments
to each leaf so that they are contiguous. ";

%feature("docstring")  gaia2::DescriptorTree::findSimple "DescriptorTree * DescriptorTree::findSimple(const QString &name)

Find the node in this tree that has the given (short) name. ";

%feature("docstring")  gaia2::DescriptorTree::find "const
DescriptorTree * DescriptorTree::find(const QString &name) const

Find the node with given name (be it a leaf or not).

If the string is empty, returns the root node. ";

%feature("docstring")  gaia2::DescriptorTree::find "DescriptorTree*
gaia2::DescriptorTree::find(const QString &name) ";

%feature("docstring")  gaia2::DescriptorTree::findFromRoot "const
DescriptorTree * DescriptorTree::findFromRoot(const QString &name)
const

Find the node with the given name (be it a leaf or not).

The given name must the the full name, anchored at the root node. If
the string is empty, returns the root node. ";

%feature("docstring")  gaia2::DescriptorTree::findFromRoot "DescriptorTree* gaia2::DescriptorTree::findFromRoot(const QString
&name) ";

%feature("docstring")  gaia2::DescriptorTree::findLeaf "const
DescriptorTree * DescriptorTree::findLeaf(const QString &name) const

Find the leaf with given name. ";

%feature("docstring")  gaia2::DescriptorTree::findLeaf "DescriptorTree* gaia2::DescriptorTree::findLeaf(const QString &name)
";

%feature("docstring")  gaia2::DescriptorTree::regionSize "int
DescriptorTree::regionSize(DescriptorType type, DescriptorLengthType
ltype) const ";

%feature("docstring")  gaia2::DescriptorTree::printLeafSegments "void
DescriptorTree::printLeafSegments() const ";

%feature("docstring")  gaia2::DescriptorTree::toYaml "QString
DescriptorTree::toYaml(int spaces=0) const

Returns a yaml string representing the hierachy of descriptors that
this tree represents.

The root node is called \"descriptors\" (instead of the empty string).
";

%feature("docstring")  gaia2::DescriptorTree::checkConsistency "void
DescriptorTree::checkConsistency() const ";

%feature("docstring")  gaia2::DescriptorTree::modify "void
DescriptorTree::modify()

Function to be called by each non const function as soon as it
modifies the tree, so as to know when to regenerate cached values. ";


// File: classgaia2_1_1DistAggr.xml
%feature("docstring") gaia2::DistAggr "";

%feature("docstring")  gaia2::DistAggr::initValue "virtual Real
gaia2::DistAggr::initValue() const ";

%feature("docstring")  gaia2::DistAggr::postProcess "virtual Real
gaia2::DistAggr::postProcess(Real x, int nsegs) const ";

%feature("docstring")  gaia2::DistAggr::~DistAggr "virtual
gaia2::DistAggr::~DistAggr() ";


// File: classgaia2std_1_1DistanceFunction.xml
%feature("docstring") gaia2std::DistanceFunction "";

%feature("docstring")  gaia2std::DistanceFunction::DistanceFunction "gaia2std::DistanceFunction::DistanceFunction(const std::string &name,
const DataSet *layout, const ParameterMap &params) ";


// File: classgaia2_1_1DistanceFunction.xml
%feature("docstring") gaia2::DistanceFunction "C++ includes:
distancefunction.h ";

%feature("docstring")  gaia2::DistanceFunction::DistanceFunction "gaia2::DistanceFunction::DistanceFunction(const PointLayout &layout,
const ParameterMap &params) ";

%feature("docstring")  gaia2::DistanceFunction::~DistanceFunction "virtual gaia2::DistanceFunction::~DistanceFunction() ";

%feature("docstring")  gaia2::DistanceFunction::layout "const
PointLayout& gaia2::DistanceFunction::layout() const ";

%feature("docstring")  gaia2::DistanceFunction::checkFixedLength "const Region & gaia2::DistanceFunction::checkFixedLength(const Region
&region) const

Checks that the given Region only contains fixed-length descriptors
and throws an exception if not.

Exceptions:
-----------

GaiaException if the region contained variable-length descriptors. ";


// File: classgaia2_1_1Distribute.xml
%feature("docstring") gaia2::Distribute "

Distribute applier class.

Rerank descriptor and gives it a new value so that it fits in a
specific distribution (gaussian for now, but could be extended to any
type of distribution).

C++ includes: distribute.h ";

%feature("docstring")  gaia2::Distribute::Distribute "Distribute::Distribute(const Transformation &transfo) ";

%feature("docstring")  gaia2::Distribute::~Distribute "Distribute::~Distribute() ";

%feature("docstring")  gaia2::Distribute::mapPoint "Point *
Distribute::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classgaia2_1_1DotProduct.xml
%feature("docstring") gaia2::DotProduct "

DotProduct applier class.

Multiplies each coordinate of the given point by the ones given in the
constructor and adds (optional) initial values.

C++ includes: dotproduct.h ";

%feature("docstring")  gaia2::DotProduct::DotProduct "DotProduct::DotProduct(const Transformation &transfo) ";

%feature("docstring")  gaia2::DotProduct::~DotProduct "DotProduct::~DotProduct() ";

%feature("docstring")  gaia2::DotProduct::mapPoint "Point *
DotProduct::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classdoxy2swig_1_1Doxy2SWIG.xml
%feature("docstring") doxy2swig::Doxy2SWIG "

Converts Doxygen generated XML files into a file containing docstrings
that can be used by SWIG-1.3.x that have support for
feature(\"docstring\").  Once the data is parsed it is stored in
self.pieces. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::__init__ "def
doxy2swig.Doxy2SWIG.__init__(self, src,
include_function_definition=True, quiet=False)

Initialize the instance given a source object.  `src` can be a file or
filename.  If you do not want to include function definitions from
doxygen then set `include_function_definition` to `False`.  This is
handy since this allows you to use the swig generated function
definition using %feature(\"autodoc\", [0,1]). ";

%feature("docstring")  doxy2swig::Doxy2SWIG::generate "def
doxy2swig.Doxy2SWIG.generate(self)

Parses the file set in the initialization.  The resulting data is
stored in `self.pieces`. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::parse "def
doxy2swig.Doxy2SWIG.parse(self, node)

Parse a given node.  This function in turn calls the
`parse_<nodeType>` functions which handle the respective nodes. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::parse_Document "def
doxy2swig.Doxy2SWIG.parse_Document(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::parse_Text "def
doxy2swig.Doxy2SWIG.parse_Text(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::parse_Element "def
doxy2swig.Doxy2SWIG.parse_Element(self, node)

Parse an `ELEMENT_NODE`.  This calls specific `do_<tagName>` handers
for different elements.  If no handler is available the
`generic_parse` method is called.  All tagNames specified in
`self.ignores` are simply ignored. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::parse_Comment "def
doxy2swig.Doxy2SWIG.parse_Comment(self, node)

Parse a `COMMENT_NODE`.  This does nothing for now. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::add_text "def
doxy2swig.Doxy2SWIG.add_text(self, value)

Adds text corresponding to `value` into `self.pieces`. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::get_specific_nodes "def
doxy2swig.Doxy2SWIG.get_specific_nodes(self, node, names)

Given a node and a sequence of strings in `names`, return a dictionary
containing the names as keys and child `ELEMENT_NODEs`, that have a
`tagName` equal to the name. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::generic_parse "def
doxy2swig.Doxy2SWIG.generic_parse(self, node, pad=0)

A Generic parser for arbitrary tags in a node.  Parameters:   - node:
A node in the DOM.  - pad: `int` (default: 0)     If 0 the node data
is not padded with newlines.  If 1 it    appends a newline after
parsing the childNodes.  If 2 it    pads before and after the nodes
are processed.  Defaults to    0. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::space_parse "def
doxy2swig.Doxy2SWIG.space_parse(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_compoundname "def
doxy2swig.Doxy2SWIG.do_compoundname(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_compounddef "def
doxy2swig.Doxy2SWIG.do_compounddef(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_includes "def
doxy2swig.Doxy2SWIG.do_includes(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_parameterlist "def
doxy2swig.Doxy2SWIG.do_parameterlist(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_para "def
doxy2swig.Doxy2SWIG.do_para(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_parametername "def
doxy2swig.Doxy2SWIG.do_parametername(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_parameterdefinition "def doxy2swig.Doxy2SWIG.do_parameterdefinition(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_detaileddescription "def doxy2swig.Doxy2SWIG.do_detaileddescription(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_briefdescription "def
doxy2swig.Doxy2SWIG.do_briefdescription(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_memberdef "def
doxy2swig.Doxy2SWIG.do_memberdef(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_definition "def
doxy2swig.Doxy2SWIG.do_definition(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_sectiondef "def
doxy2swig.Doxy2SWIG.do_sectiondef(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_header "def
doxy2swig.Doxy2SWIG.do_header(self, node)

For a user defined section def a header field is present which should
not be printed as such, so we comment it in the output. ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_simplesect "def
doxy2swig.Doxy2SWIG.do_simplesect(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_argsstring "def
doxy2swig.Doxy2SWIG.do_argsstring(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_member "def
doxy2swig.Doxy2SWIG.do_member(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::do_doxygenindex "def
doxy2swig.Doxy2SWIG.do_doxygenindex(self, node) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::write "def
doxy2swig.Doxy2SWIG.write(self, fname) ";

%feature("docstring")  doxy2swig::Doxy2SWIG::clean_pieces "def
doxy2swig.Doxy2SWIG.clean_pieces(self, pieces)

Cleans the list of strings given as `pieces`.  It replaces multiple
newlines by a maximum of 2 and returns a new list. It also wraps the
paragraphs nicely. ";


// File: classEmptyDataSet.xml
%feature("docstring") EmptyDataSet "";

%feature("docstring")  EmptyDataSet::load "int
EmptyDataSet::load(const QString &filename) ";


// File: classgaia2_1_1Enumerate.xml
%feature("docstring") gaia2::Enumerate "

The Enumerate analyzer allows you to specify a list of string
descriptors which you want to be stored as enumerations.

Typically, for string descriptors that only take a subset of values
(eg: key and mode), this will reduce the memory used for storage and
it will also be much faster when filtering.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be enumerated. Can
use wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

C++ includes: enumerate.h ";

%feature("docstring")  gaia2::Enumerate::Enumerate "gaia2::Enumerate::Enumerate(const ParameterMap &params) ";

%feature("docstring")  gaia2::Enumerate::~Enumerate "virtual
gaia2::Enumerate::~Enumerate() ";

%feature("docstring")  gaia2::Enumerate::analyze "Transformation
gaia2::Enumerate::analyze(const DataSet *data) const ";


// File: classgaia2_1_1EnumerateApplier.xml
%feature("docstring") gaia2::EnumerateApplier "

EnumerateApplier applier class.

Fixes the length of the specified descriptors.

C++ includes: enumerateapplier.h ";

%feature("docstring")  gaia2::EnumerateApplier::EnumerateApplier "EnumerateApplier::EnumerateApplier(const Transformation &transfo) ";

%feature("docstring")  gaia2::EnumerateApplier::~EnumerateApplier "EnumerateApplier::~EnumerateApplier() ";

%feature("docstring")  gaia2::EnumerateApplier::mapPoint "Point *
EnumerateApplier::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";

%feature("docstring")  gaia2::EnumerateApplier::mapDataSet "DataSet *
EnumerateApplier::mapDataSet(const DataSet *dataset) const

Optimized version so that we don't have to recompute the index
mappings for each point. ";


// File: classgaia2_1_1EuclideanDistance.xml
%feature("docstring") gaia2::EuclideanDistance "

This class computes the euclidean distance of a given Region.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
computation of the distance. Only accepts real descriptors, can use
wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards.

C++ includes: euclideandistance.h ";

%feature("docstring")  gaia2::EuclideanDistance::EuclideanDistance "EuclideanDistance::EuclideanDistance(const PointLayout &layout, const
ParameterMap &params) ";

%feature("docstring")  gaia2::EuclideanDistance::EuclideanDistance "gaia2::EuclideanDistance::EuclideanDistance(int dimension) ";


// File: classgaia2_1_1ExponentialCompressDistance.xml
%feature("docstring") gaia2::ExponentialCompressDistance "

This class compresses a distance from [0; inf) into [0; 1) using the
following formula: dist = 1 - exp(-alpha*dist), alpha being a
compression factor.

Parameters:
-----------

distance:  the name of the distance to be compressed.

params:  the parameters for creating the distance to be compressed.

alpha:  the compression factor.

C++ includes: exponentialcompressdistance.h ";

%feature("docstring")
gaia2::ExponentialCompressDistance::ExponentialCompressDistance "ExponentialCompressDistance::ExponentialCompressDistance(const
PointLayout &layout, const ParameterMap &params) ";

%feature("docstring")
gaia2::ExponentialCompressDistance::~ExponentialCompressDistance "ExponentialCompressDistance::~ExponentialCompressDistance() ";


// File: classgaia2_1_1Extract.xml
%feature("docstring") gaia2::Extract "

The Extract analyzer allows you to extract only specific dimensions
from any given number of descriptors.

It works by specifying the list of dimensions to be extracted as
standalone descriptors in the form descriptorName[dim]. NB: at the
moment, this transformation only works on Real descriptors.

Parameters:
-----------

dimensions:  the names of the descriptors/dimensions to be extracted.

C++ includes: extract.h ";

%feature("docstring")  gaia2::Extract::Extract "gaia2::Extract::Extract(const ParameterMap &params) ";

%feature("docstring")  gaia2::Extract::~Extract "virtual
gaia2::Extract::~Extract() ";

%feature("docstring")  gaia2::Extract::analyze "Transformation
gaia2::Extract::analyze(const DataSet *data) const ";


// File: classgaia2_1_1ExtractApplier.xml
%feature("docstring") gaia2::ExtractApplier "

Extract applier class.

Extracts specified dimensions from descriptors and promotes them to
individual descriptors.

C++ includes: extractapplier.h ";

%feature("docstring")  gaia2::ExtractApplier::ExtractApplier "ExtractApplier::ExtractApplier(const Transformation &transfo) ";

%feature("docstring")  gaia2::ExtractApplier::~ExtractApplier "ExtractApplier::~ExtractApplier() ";

%feature("docstring")  gaia2::ExtractApplier::mapPoint "Point *
ExtractApplier::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classgaia2_1_1Factory.xml
%feature("docstring") gaia2::Factory "

A basic implementation of a generic factory class.

We can choose the identifier type (usually string) and the base type
that we want to implement. Registration is done via the creation of a
static object of type Factory<IdentifierType,
AbstractProduct>::Registrar<ConcreteProduct>. There is also a third
template parameter that can be used, the type of an optional argument.
That means we will be able to call: Factory::create(\"PCA\",
\"dim=3\");

C++ includes: factory.h ";

%feature("docstring")  gaia2::Factory::create_i "AbstractProduct*
gaia2::Factory< IdentifierType, AbstractProduct, ArgumentType
>::create_i(const IdentifierType &id, const ArgumentType &arg) const
";


// File: classgaia2_1_1Filter.xml
%feature("docstring") gaia2::Filter "

The Filter class allows to check whether a predicate is true for a
given Point.

To use it, you first need to instantiate it (usually returned by the
FilterParser::parse() method), and then bind it to a specific layout
(so that variables used in the query are bound to the variables in the
layout). You can then call Filter::isTrue() method with a Point as
argument to know whether the predicate is true for the given Point or
not.

C++ includes: filter.h ";

%feature("docstring")  gaia2::Filter::Filter "gaia2::Filter::Filter()
";

%feature("docstring")  gaia2::Filter::Filter "gaia2::Filter::Filter(const Filter &filter) ";

%feature("docstring")  gaia2::Filter::updateVariables "void
gaia2::Filter::updateVariables()

Look in the predicate/value tree for all the variables it can find and
put them inside their respective *VarList member variables.

This needs to be called if you want your filter to be valid, otherwise
it might do weird things (such as crash your program, corrupt your
data, ...). ";

%feature("docstring")  gaia2::Filter::needsBinding "bool
gaia2::Filter::needsBinding() const

Returns whether this filter needs to be bound to a specific layout.

A filter will need to be bound when it has at least one value or label
variable, otherwise it is free (and most probably useless!). ";

%feature("docstring")  gaia2::Filter::bindToLayout "void
gaia2::Filter::bindToLayout(const PointLayout &layout) ";

%feature("docstring")  gaia2::Filter::bindToPointAttributes "void
gaia2::Filter::bindToPointAttributes() ";

%feature("docstring")  gaia2::Filter::~Filter "gaia2::Filter::~Filter() ";

%feature("docstring")  gaia2::Filter::isTrue "bool
gaia2::Filter::isTrue(const Point *p) const

Returns whether the filter predicate is true for the given point. ";

%feature("docstring")  gaia2::Filter::isAlwaysTrue "bool
gaia2::Filter::isAlwaysTrue() const

Returns whether the filter predicate is always true, regardless of the
given point. ";

%feature("docstring")  gaia2::Filter::pred "const parser::Predicate*
gaia2::Filter::pred() const ";

%feature("docstring")  gaia2::Filter::toString "QString
gaia2::Filter::toString() const ";


// File: classgaia2_1_1parser_1_1FilterParser.xml
%feature("docstring") gaia2::parser::FilterParser "

The FilterParser class is used to parse filter queries given as
strings and create the corresponding Filter instance that can be used
on Points.

C++ includes: filter.h ";


// File: classgaia2_1_1filter_1_1FilterWrapper.xml
%feature("docstring") gaia2::filter::FilterWrapper "

The FilterWrapper class serves as an easy wrapper for a subset of all
possible filters, but with a much more intuitive API that allows
filters to be constructed programatically on the fly instead of being
parsed from a string representation.

C++ includes: filterwrapper.h ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const QString &varname,
const QString &op, Real value) ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const QString &varname,
const QString &op, const QList< Real > &values) ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const QString &varname,
const QString &op, const QString &label) ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const QString &varname,
const QString &op, const QList< QString > &labels) ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const std::string
&varname, const std::string &op, Real value) ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const std::string
&varname, const std::string &op, const std::vector< Real > &values) ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const std::string
&varname, const std::string &op, const std::string &label) ";

%feature("docstring")  gaia2::filter::FilterWrapper::FilterWrapper "gaia2::filter::FilterWrapper::FilterWrapper(const std::string
&varname, const std::string &op, const std::vector< std::string >
&labels) ";


// File: classgaia2_1_1FixLength.xml
%feature("docstring") gaia2::FixLength "

The FixLength analyzer allows you to specify a list of descriptors for
which you want to fix the length definitely.

This is very useful for saving memory.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be fixed. Can use
wildcards. If you don't specify a value here, it will fix the length
of all the descriptors which are of fixed size inside this dataset. In
that case, the exclude parameter will be ignored.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

C++ includes: fixlength.h ";

%feature("docstring")  gaia2::FixLength::FixLength "gaia2::FixLength::FixLength(const ParameterMap &params) ";

%feature("docstring")  gaia2::FixLength::~FixLength "virtual
gaia2::FixLength::~FixLength() ";

%feature("docstring")  gaia2::FixLength::analyze "Transformation
gaia2::FixLength::analyze(const DataSet *data) const ";


// File: classgaia2_1_1FixLengthApplier.xml
%feature("docstring") gaia2::FixLengthApplier "

FixLengthApplier applier class.

Fixes the length of the specified descriptors.

C++ includes: fixlengthapplier.h ";

%feature("docstring")  gaia2::FixLengthApplier::FixLengthApplier "FixLengthApplier::FixLengthApplier(const Transformation &transfo) ";

%feature("docstring")  gaia2::FixLengthApplier::~FixLengthApplier "FixLengthApplier::~FixLengthApplier() ";

%feature("docstring")  gaia2::FixLengthApplier::mapPoint "Point *
FixLengthApplier::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classgaia2_1_1ForceIdentityDistance.xml
%feature("docstring") gaia2::ForceIdentityDistance "

This metric forces the identity condition to be valid on the composed
distance by checking first if the 2 points are equal and returning 0
if that is the case.

Otherwise, it just returns the value of the composed distance.

NB: Equality is decided using the name of the points, not their
contents.

Parameters:
-----------

distance:  the name of the distance to be compressed.

params:  the parameters for creating the distance to be compressed

C++ includes: forceidentitydistance.h ";

%feature("docstring")
gaia2::ForceIdentityDistance::ForceIdentityDistance "ForceIdentityDistance::ForceIdentityDistance(const PointLayout
&layout, const ParameterMap &params) ";

%feature("docstring")
gaia2::ForceIdentityDistance::~ForceIdentityDistance "ForceIdentityDistance::~ForceIdentityDistance() ";


// File: classgaia2_1_1FrozenCosineAngleDistance.xml
%feature("docstring") gaia2::FrozenCosineAngleDistance "";

%feature("docstring")
gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance "gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance(const
FrozenDataSet &dataset, int offset, int size) ";

%feature("docstring")
gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance "gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance(const
FrozenDataSet &dataset, const QString &descriptorName) ";

%feature("docstring")
gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance "gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance(const
FrozenDataSet &dataset, const std::string &descriptorName) ";

%feature("docstring")
gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance "gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance(const
FrozenDataSet &dataset, const char *descriptorName) ";

%feature("docstring")
gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance "gaia2::FrozenCosineAngleDistance::FrozenCosineAngleDistance(const
FrozenDataSet &dataset, const ParameterMap &params) ";

%feature("docstring")  gaia2::FrozenCosineAngleDistance::init "void
gaia2::FrozenCosineAngleDistance::init(const QString &descriptorName)
";

%feature("docstring")  gaia2::FrozenCosineAngleDistance::init "void
gaia2::FrozenCosineAngleDistance::init(int offset, int size) ";

%feature("docstring")  gaia2::FrozenCosineAngleDistance::prepare "void gaia2::FrozenCosineAngleDistance::prepare(const FrozenPoint
&query) ";

%feature("docstring")  gaia2::FrozenCosineAngleDistance::acos "Real
gaia2::FrozenCosineAngleDistance::acos(Real x) const ";


// File: classgaia2_1_1FrozenDataSet.xml
%feature("docstring") gaia2::FrozenDataSet "

A FrozenDataSet is a dataset that has been flagged as immutable.

No more fields can be added, nor can any field be removed from it
(hence the name frozen). There are also conditions that need to be
filled before creating a FrozenDataSet, such as the fact that it only
accepts real-type fixed-length descriptors at the moment. It has
however a few other advantages over a normal dataset, namely that the
memory used for it is contiguous (and thus much faster to iterate
over, such as in the case of nn- queries).

C++ includes: frozendataset.h ";

%feature("docstring")  gaia2::FrozenDataSet::FrozenDataSet "FrozenDataSet::FrozenDataSet() ";

%feature("docstring")  gaia2::FrozenDataSet::FrozenDataSet "FrozenDataSet::FrozenDataSet(int npoints, int dimension) ";

%feature("docstring")  gaia2::FrozenDataSet::load "void
FrozenDataSet::load(const QString &filename)

Load a dataset from disk. ";

%feature("docstring")  gaia2::FrozenDataSet::save "void
FrozenDataSet::save(const QString &filename) const

Save this dataset to disk. ";

%feature("docstring")  gaia2::FrozenDataSet::size "int
FrozenDataSet::size() const

Return the number of points contained in this dataset. ";

%feature("docstring")  gaia2::FrozenDataSet::dimension "int
FrozenDataSet::dimension() const

Return the number of dimensions of this dataset. ";

%feature("docstring")  gaia2::FrozenDataSet::pointNames "QStringList
FrozenDataSet::pointNames() const ";

%feature("docstring")  gaia2::FrozenDataSet::pointName "const QString
& FrozenDataSet::pointName(int i) const

Return the name of the point with the given index. ";

%feature("docstring")  gaia2::FrozenDataSet::pointIndex "int
FrozenDataSet::pointIndex(const QString &name) const

Return the index of the point with the given name.

Exceptions:
-----------

GaiaException if no such point exists ";

%feature("docstring")  gaia2::FrozenDataSet::point "FrozenPoint
FrozenDataSet::point(const QString &name) const ";

%feature("docstring")  gaia2::FrozenDataSet::descriptorLocation "QPair< int, int > FrozenDataSet::descriptorLocation(const QString
&descriptorName) const

Return the column indices of beginning and end fo the region spanning
the descriptor.

NOTE: contrary to a normal dataset's layout, here you always have to
give the fully-qualified name. ";

%feature("docstring")  gaia2::FrozenDataSet::descriptorNames "QStringList FrozenDataSet::descriptorNames() const ";

%feature("docstring")  gaia2::FrozenDataSet::fromDataSet "void
FrozenDataSet::fromDataSet(const DataSet &dataset)

Note: this method is not made static as a named-constructor, because
then we would have to return the FrozenDataSet by value which would be
very inefficient.

There could be 2 solutions to this, none of which seems really worth
investing the time in it (curiosity would of course want me to do it,
but oh well... :-) ) expression templates

C++0x move semantics ";

%feature("docstring")  gaia2::FrozenDataSet::setReferenceDataSet "void FrozenDataSet::setReferenceDataSet(DataSet *dataset=0)

Set the reference dataset, ie: the one used for fetching the values
when doing filtered queries.

Passing it a null pointer (or no argument) will mark this dataset has
having no reference dataset. ";

%feature("docstring")  gaia2::FrozenDataSet::referenceDataSet "const
DataSet * FrozenDataSet::referenceDataSet() const

Return the reference dataset.

If no reference dataset has been previously set, then this method will
throw an exception. ";

%feature("docstring")  gaia2::FrozenDataSet::addView "void
gaia2::FrozenDataSet::addView(FrozenView *view) ";

%feature("docstring")  gaia2::FrozenDataSet::removeView "void
gaia2::FrozenDataSet::removeView(FrozenView *view) ";


// File: classgaia2_1_1FrozenDistance.xml
%feature("docstring") gaia2::FrozenDistance "";

%feature("docstring")  gaia2::FrozenDistance::FrozenDistance "gaia2::FrozenDistance::FrozenDistance(const FrozenDataSet &dataset) ";

%feature("docstring")  gaia2::FrozenDistance::~FrozenDistance "virtual gaia2::FrozenDistance::~FrozenDistance() ";

%feature("docstring")  gaia2::FrozenDistance::prepare "virtual void
gaia2::FrozenDistance::prepare(const FrozenPoint &query) ";

%feature("docstring")  gaia2::FrozenDistance::dataset "const
FrozenDataSet& gaia2::FrozenDistance::dataset() const ";


// File: classgaia2_1_1FrozenEuclideanDistance.xml
%feature("docstring") gaia2::FrozenEuclideanDistance "";

%feature("docstring")
gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance "gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance(const
FrozenDataSet &dataset, int offset, int size) ";

%feature("docstring")
gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance "gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance(const
FrozenDataSet &dataset, const QString &descriptorName) ";

%feature("docstring")
gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance "gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance(const
FrozenDataSet &dataset, const std::string &descriptorName) ";

%feature("docstring")
gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance "gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance(const
FrozenDataSet &dataset, const char *descriptorName) ";

%feature("docstring")
gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance "gaia2::FrozenEuclideanDistance::FrozenEuclideanDistance(const
FrozenDataSet &dataset, const ParameterMap &params) ";

%feature("docstring")  gaia2::FrozenEuclideanDistance::init "void
gaia2::FrozenEuclideanDistance::init(const QString &descriptorName) ";

%feature("docstring")  gaia2::FrozenEuclideanDistance::init "void
gaia2::FrozenEuclideanDistance::init(int offset, int size) ";


// File: classgaia2_1_1FrozenExponentialCompressDistance.xml
%feature("docstring") gaia2::FrozenExponentialCompressDistance "";

%feature("docstring")
gaia2::FrozenExponentialCompressDistance::FrozenExponentialCompressDistance
"gaia2::FrozenExponentialCompressDistance::FrozenExponentialCompressDistance(const
FrozenDataSet &dataset, const ParameterMap &params) ";

%feature("docstring")
gaia2::FrozenExponentialCompressDistance::~FrozenExponentialCompressDistance
"gaia2::FrozenExponentialCompressDistance::~FrozenExponentialCompressDistance()
";

%feature("docstring")
gaia2::FrozenExponentialCompressDistance::prepare "void
gaia2::FrozenExponentialCompressDistance::prepare(const FrozenPoint
&query) ";


// File: classgaia2_1_1FrozenLinearCombinationDistance.xml
%feature("docstring") gaia2::FrozenLinearCombinationDistance "";

%feature("docstring")
gaia2::FrozenLinearCombinationDistance::FrozenLinearCombinationDistance
"gaia2::FrozenLinearCombinationDistance::FrozenLinearCombinationDistance(const
FrozenDataSet &dataset, const ParameterMap &params) ";

%feature("docstring")
gaia2::FrozenLinearCombinationDistance::~FrozenLinearCombinationDistance
"gaia2::FrozenLinearCombinationDistance::~FrozenLinearCombinationDistance()
";

%feature("docstring")  gaia2::FrozenLinearCombinationDistance::prepare
"void gaia2::FrozenLinearCombinationDistance::prepare(const
FrozenPoint &query) ";


// File: classgaia2_1_1FrozenSearchPoint.xml
%feature("docstring") gaia2::FrozenSearchPoint "";

%feature("docstring")  gaia2::FrozenSearchPoint::FrozenSearchPoint "gaia2::FrozenSearchPoint::FrozenSearchPoint() ";

%feature("docstring")  gaia2::FrozenSearchPoint::FrozenSearchPoint "gaia2::FrozenSearchPoint::FrozenSearchPoint(int iidx) ";


// File: classgaia2_1_1GaiaException.xml
%feature("docstring") gaia2::GaiaException "

Exception class that can take up to 3 arguments of any type, which
will be serialized into a QString for the error message.

C++ includes: gaiaexception.h ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException() ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException(const QString &msg) ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException(const char *msg) ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException(const std::string &msg) ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException(const QStringList &msg) ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException(const T &a, const U &b) ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException(const T &a, const U &b, const V
&c) ";

%feature("docstring")  gaia2::GaiaException::GaiaException "gaia2::GaiaException::GaiaException(const T &a, const U &b, const V
&c, const W &d) ";

%feature("docstring")  gaia2::GaiaException::~GaiaException "virtual
gaia2::GaiaException::~GaiaException()  throw ()";

%feature("docstring")  gaia2::GaiaException::what "virtual const
char* gaia2::GaiaException::what() const  throw ()";

%feature("docstring")  gaia2::GaiaException::msg "const QString&
gaia2::GaiaException::msg() const  throw ()";

%feature("docstring")  gaia2::GaiaException::msg "QString&
gaia2::GaiaException::msg()  throw ()";


// File: classgaia2_1_1GaiaMap.xml
%feature("docstring") gaia2::GaiaMap "

Map class (derived from QMap) but that throws a custom Exception
instead of returning a default constructed value when the key isn't in
there.

C++ includes: types.h ";

%feature("docstring")  gaia2::GaiaMap::value "const Value&
gaia2::GaiaMap< Key, Value, ExceptionType >::value(const Key &key)
const ";

%feature("docstring")  gaia2::GaiaMap::value "const Value&
gaia2::GaiaMap< Key, Value, ExceptionType >::value(const Key &key,
const Value &defaultValue) const ";

%feature("docstring")  gaia2::GaiaMap::value "Value& gaia2::GaiaMap<
Key, Value, ExceptionType >::value(const Key &key) ";


// File: classgaia2_1_1Gaussianize.xml
%feature("docstring") gaia2::Gaussianize "

The Gaussianize analyzer fits each descriptor into a gaussian
distribution.

It does it by sorting all the values, and giving them new values as if
they were forming a perfect gaussian distribution. In doing so it
completely discards the value it had before (only keeps the rank), and
as such may be not as correct or precise as a BoxCox transformation,
but it is much faster to compute.

NB: This algorithm does not yet work with multi-segments points.

Parameters:
-----------

distributionSize:  when analyzing the distribution of a lot of values,
it is not necessary to keep all points as reference, but only a subset
of them. This value represent the maximum number of reference values
used to model the distribution. In most cases, 10'000 points should
give a precise enough distribution while still being efficient to
compute. (default: 10000).

C++ includes: gaussianize.h ";

%feature("docstring")  gaia2::Gaussianize::Gaussianize "gaia2::Gaussianize::Gaussianize(const ParameterMap &params) ";

%feature("docstring")  gaia2::Gaussianize::~Gaussianize "virtual
gaia2::Gaussianize::~Gaussianize() ";

%feature("docstring")  gaia2::Gaussianize::analyze "Transformation
gaia2::Gaussianize::analyze(const DataSet *data, const Region &region)
const ";


// File: classgaia2std_1_1GException.xml
%feature("docstring") gaia2std::GException "";

%feature("docstring")  gaia2std::GException::GException "gaia2std::GException::GException() ";

%feature("docstring")  gaia2std::GException::GException "gaia2std::GException::GException(const char *msg) ";

%feature("docstring")  gaia2std::GException::GException "gaia2std::GException::GException(const std::string &msg) ";

%feature("docstring")  gaia2std::GException::~GException "virtual
gaia2std::GException::~GException()  throw ()";

%feature("docstring")  gaia2std::GException::what "virtual const
char* gaia2std::GException::what() const  throw ()";

%feature("docstring")  gaia2std::GException::message "const
std::string& gaia2std::GException::message() const  throw ()";


// File: classGVarLengthArray.xml
%feature("docstring") GVarLengthArray "";

%feature("docstring")  GVarLengthArray::GVarLengthArray "Q_INLINE_TEMPLATE GVarLengthArray< T, Prealloc >::GVarLengthArray(int
size=0) ";

%feature("docstring")  GVarLengthArray::GVarLengthArray "GVarLengthArray< T, Prealloc >::GVarLengthArray(const GVarLengthArray<
T, Prealloc > &other) ";

%feature("docstring")  GVarLengthArray::~GVarLengthArray "GVarLengthArray< T, Prealloc >::~GVarLengthArray() ";

%feature("docstring")  GVarLengthArray::removeLast "void
GVarLengthArray< T, Prealloc >::removeLast() ";

%feature("docstring")  GVarLengthArray::size "int GVarLengthArray< T,
Prealloc >::size() const ";

%feature("docstring")  GVarLengthArray::count "int GVarLengthArray<
T, Prealloc >::count() const ";

%feature("docstring")  GVarLengthArray::isEmpty "bool
GVarLengthArray< T, Prealloc >::isEmpty() const ";

%feature("docstring")  GVarLengthArray::resize "Q_INLINE_TEMPLATE
void GVarLengthArray< T, Prealloc >::resize(int size) ";

%feature("docstring")  GVarLengthArray::clear "void GVarLengthArray<
T, Prealloc >::clear() ";

%feature("docstring")  GVarLengthArray::capacity "int
GVarLengthArray< T, Prealloc >::capacity() const ";

%feature("docstring")  GVarLengthArray::reserve "Q_INLINE_TEMPLATE
void GVarLengthArray< T, Prealloc >::reserve(int size) ";

%feature("docstring")  GVarLengthArray::append "void GVarLengthArray<
T, Prealloc >::append(const T &t) ";

%feature("docstring")  GVarLengthArray::append "Q_OUTOFLINE_TEMPLATE
void GVarLengthArray< T, Prealloc >::append(const T *buf, int size) ";

%feature("docstring")  GVarLengthArray::data "T* GVarLengthArray< T,
Prealloc >::data() ";

%feature("docstring")  GVarLengthArray::data "const T*
GVarLengthArray< T, Prealloc >::data() const ";

%feature("docstring")  GVarLengthArray::constData "const T*
GVarLengthArray< T, Prealloc >::constData() const ";


// File: classgaia2_1_1InnerDim.xml
%feature("docstring") gaia2::InnerDim "";

%feature("docstring")  gaia2::InnerDim::InnerDim "gaia2::InnerDim::InnerDim(DescriptorLengthType ltype_, int desc_, int
dim_) ";


// File: classgaia2_1_1InverseProjection.xml
%feature("docstring") gaia2::InverseProjection "

The inverse projection Analyzer computes the inverse the previous
transformation, given it was a linear projection (such as PCA or RCA).

Actually, as not all projections are invertible, in effect it does a
projection using the transposed matrix of the previous one.

In the case of PCA and RCA, it does correspond to the inverse
projection, because the projection matrix is the unitary basis for the
covariance matrix, which is positive-(semi)definite.

The resulting layout will be the same as if you had used the
MergeRegion transformation on the dataset before calling the
projection transformation.

NB: this transformation requires that the last transformation applied
to the dataset is a projection.

C++ includes: inverseprojection.h ";

%feature("docstring")  gaia2::InverseProjection::InverseProjection "gaia2::InverseProjection::InverseProjection(const ParameterMap
&params) ";

%feature("docstring")  gaia2::InverseProjection::~InverseProjection "virtual gaia2::InverseProjection::~InverseProjection() ";

%feature("docstring")  gaia2::InverseProjection::analyze "Transformation gaia2::InverseProjection::analyze(const DataSet
*dataset) const ";


// File: classgaia2_1_1Key.xml
%feature("docstring") gaia2::Key "";

%feature("docstring")  gaia2::Key::Key "gaia2::Key::Key(QString key)
";

%feature("docstring")  gaia2::Key::dist "Real gaia2::Key::dist(const
Key &k) const ";


// File: classgaia2_1_1KeyDistance.xml
%feature("docstring") gaia2::KeyDistance "

This class computes the harmonic distance between keys.

Range is from 0 to 1. This distance has been written by Nico and is
more likely to serve as an example than a useful distance, but
still...

Parameters:
-----------

name:  the name of the descriptor containing the key value. Default is
\"key_key\".

C++ includes: keydistance.h ";

%feature("docstring")  gaia2::KeyDistance::KeyDistance "gaia2::KeyDistance::KeyDistance(const PointLayout &layout, const
ParameterMap &params) ";


// File: classgaia2_1_1KullbackLeiblerDistance.xml
%feature("docstring") gaia2::KullbackLeiblerDistance "

This class computes the symmetric Kullback-Leibler distance of a given
Region.

Parameters:
-----------

descriptorName:  the name of the descriptor for which you want to
compute the KL- distance. This descriptor needs to have 3 sub-
descriptors: mean, cov, and icov. (representing, resp., the mean of
the frames, the covariance matrix, and the inverse of the covariance
matrix.

C++ includes: kullbackleiblerdistance.h ";

%feature("docstring")
gaia2::KullbackLeiblerDistance::KullbackLeiblerDistance "KullbackLeiblerDistance::KullbackLeiblerDistance(const PointLayout
&layout, const ParameterMap &params) ";


// File: classgaia2_1_1parser_1_1Label.xml
%feature("docstring") gaia2::parser::Label "";

%feature("docstring")  gaia2::parser::Label::~Label "virtual
gaia2::parser::Label::~Label() ";

%feature("docstring")  gaia2::parser::Label::value "virtual QString
gaia2::parser::Label::value() const =0 ";

%feature("docstring")  gaia2::parser::Label::toString "virtual
QString gaia2::parser::Label::toString() const =0 ";

%feature("docstring")  gaia2::parser::Label::copy "virtual Label*
gaia2::parser::Label::copy() const =0 ";


// File: classgaia2_1_1parser_1_1LabelConstant.xml
%feature("docstring") gaia2::parser::LabelConstant "";

%feature("docstring")  gaia2::parser::LabelConstant::LabelConstant "gaia2::parser::LabelConstant::LabelConstant(const QString &value) ";

%feature("docstring")  gaia2::parser::LabelConstant::copy "virtual
Label* gaia2::parser::LabelConstant::copy() const ";

%feature("docstring")  gaia2::parser::LabelConstant::value "virtual
QString gaia2::parser::LabelConstant::value() const ";

%feature("docstring")  gaia2::parser::LabelConstant::toString "virtual QString gaia2::parser::LabelConstant::toString() const ";


// File: classgaia2_1_1BaseSearchSpacePool_1_1LabelIndex.xml


// File: classgaia2_1_1parser_1_1LabelPointID.xml
%feature("docstring") gaia2::parser::LabelPointID "";

%feature("docstring")  gaia2::parser::LabelPointID::LabelPointID "gaia2::parser::LabelPointID::LabelPointID() ";

%feature("docstring")  gaia2::parser::LabelPointID::copy "virtual
Label* gaia2::parser::LabelPointID::copy() const ";

%feature("docstring")  gaia2::parser::LabelPointID::value "virtual
QString gaia2::parser::LabelPointID::value() const ";

%feature("docstring")  gaia2::parser::LabelPointID::bind "void
gaia2::parser::LabelPointID::bind(const Point **pointLocation) ";

%feature("docstring")  gaia2::parser::LabelPointID::toString "virtual
QString gaia2::parser::LabelPointID::toString() const ";


// File: classgaia2_1_1parser_1_1LabelVariable.xml
%feature("docstring") gaia2::parser::LabelVariable "";

%feature("docstring")  gaia2::parser::LabelVariable::LabelVariable "gaia2::parser::LabelVariable::LabelVariable(const QString &name) ";

%feature("docstring")  gaia2::parser::LabelVariable::name "QString
gaia2::parser::LabelVariable::name() const ";

%feature("docstring")  gaia2::parser::LabelVariable::copy "virtual
Label* gaia2::parser::LabelVariable::copy() const ";

%feature("docstring")  gaia2::parser::LabelVariable::value "virtual
QString gaia2::parser::LabelVariable::value() const ";

%feature("docstring")  gaia2::parser::LabelVariable::bind "void
gaia2::parser::LabelVariable::bind(const PointLayout &layout, const
Point **pointLocation) ";

%feature("docstring")  gaia2::parser::LabelVariable::toString "virtual QString gaia2::parser::LabelVariable::toString() const ";


// File: classgaia2_1_1LayoutAwareFactory.xml
%feature("docstring") gaia2::LayoutAwareFactory "

A tuned implementation of a factory.

It is based on the generic Factory class, except that it requires for
all objects to have a constructor with a PointLayout* (or
FrozenDataSet) as first argument, and then another one specified by
the ArgumentType template argument.

C++ includes: layoutawarefactory.h ";

%feature("docstring")  gaia2::LayoutAwareFactory::create_i "AbstractProduct* gaia2::LayoutAwareFactory< IdentifierType,
AbstractProduct, LayoutType, ArgumentType >::create_i(const
IdentifierType &id, const LayoutType &layout, const ArgumentType &arg)
const ";


// File: classgaia2_1_1LinearCombinationDistance.xml
%feature("docstring") gaia2::LinearCombinationDistance "

This class computes a distance that is the linear combination of any
number of other DistanceFunctions.

For instance, in a recommender system, a set of weights could be seen
as a user profile, each user having its own preferences regarding
which distance is more useful to him.

Parameters:
-----------

parameterMap:  the parameterMap passed as argument should contain
mapping from a name to a map containing the following fields:
\"distance\", \"params\" and \"weight\".

distance:  the name of the metric to create.

params:  the ParameterMap to be used when creating the specified
metric.

weight:  the weight to be assigned to this metric in the linear
combination.

C++ includes: linearcombinationdistance.h ";

%feature("docstring")
gaia2::LinearCombinationDistance::LinearCombinationDistance "LinearCombinationDistance::LinearCombinationDistance(const PointLayout
&layout, const ParameterMap &params) ";

%feature("docstring")
gaia2::LinearCombinationDistance::~LinearCombinationDistance "LinearCombinationDistance::~LinearCombinationDistance() ";

%feature("docstring")  gaia2::LinearCombinationDistance::addDistance "void LinearCombinationDistance::addDistance(DistanceFunction *f, Real
coeff) ";


// File: classLoadPointJob.xml
%feature("docstring") LoadPointJob "

This class represent a job that loads a single point, and returns it
when done.

On error, it will print a message and return a null pointer. ";

%feature("docstring")  LoadPointJob::LoadPointJob "LoadPointJob::LoadPointJob(const QString &pname, const QString
&filename, const QStringList *descsSelect, const QStringList
*descsExclude, PointLayout **layout, int *ncompleted, TextProgress
*info, QMutex *lock) ";

%feature("docstring")  LoadPointJob::~LoadPointJob "LoadPointJob::~LoadPointJob() ";

%feature("docstring")  LoadPointJob::run "Point* LoadPointJob::run()
";


// File: classgaia2_1_1Logger.xml
%feature("docstring") gaia2::Logger "

Asynchronous thread-safe logger object.

(TODO: implementation is still not thread-safe)

C++ includes: debugging.h ";

%feature("docstring")  gaia2::Logger::Logger "gaia2::Logger::Logger()
";

%feature("docstring")  gaia2::Logger::debug "void
gaia2::Logger::debug(DebuggingModule module, const QString &msg, bool
resetHeader=false) ";

%feature("docstring")  gaia2::Logger::info "void
gaia2::Logger::info(const QString &msg) ";

%feature("docstring")  gaia2::Logger::warning "void
gaia2::Logger::warning(const QString &msg) ";

%feature("docstring")  gaia2::Logger::error "void
gaia2::Logger::error(const QString &msg) ";


// File: classgaia2_1_1ManhattanDistance.xml
%feature("docstring") gaia2::ManhattanDistance "

This class computes the Manhattan distance of a given Region.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
computation of the distance. Only accepts real descriptors, can use
wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards.

C++ includes: manhattandistance.h ";

%feature("docstring")  gaia2::ManhattanDistance::ManhattanDistance "ManhattanDistance::ManhattanDistance(const PointLayout &layout, const
ParameterMap &params) ";


// File: classgaia2_1_1MatMult.xml
%feature("docstring") gaia2::MatMult "

MatMult class.

C++ includes: matmult.h ";

%feature("docstring")  gaia2::MatMult::MatMult "MatMult::MatMult(const Transformation &transfo) ";

%feature("docstring")  gaia2::MatMult::~MatMult "MatMult::~MatMult()
";

%feature("docstring")  gaia2::MatMult::mapPoint "Point *
MatMult::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classMaxAggr.xml
%feature("docstring") MaxAggr "";

%feature("docstring")  MaxAggr::initValue "Real MaxAggr::initValue()
const ";

%feature("docstring")  MaxAggr::postProcess "Real
MaxAggr::postProcess(Real x, int nsegs) const ";

%feature("docstring")  MaxAggr::~MaxAggr "virtual MaxAggr::~MaxAggr()
";


// File: classMeanAggr.xml
%feature("docstring") MeanAggr "";

%feature("docstring")  MeanAggr::initValue "Real
MeanAggr::initValue() const ";

%feature("docstring")  MeanAggr::postProcess "Real
MeanAggr::postProcess(Real x, int nsegs) const ";

%feature("docstring")  MeanAggr::~MeanAggr "virtual
MeanAggr::~MeanAggr() ";


// File: classgaia2_1_1Merge.xml
%feature("docstring") gaia2::Merge "

Merge applier class.

Merges together 2 points which have had a different history of
transformations, but which both stem from the same original point.

C++ includes: merge.h ";

%feature("docstring")  gaia2::Merge::Merge "Merge::Merge(const
Transformation &transfo) ";

%feature("docstring")  gaia2::Merge::~Merge "Merge::~Merge() ";

%feature("docstring")  gaia2::Merge::mapPoint "Point *
Merge::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";

%feature("docstring")  gaia2::Merge::mapDataSet "DataSet *
Merge::mapDataSet(const DataSet *dataset) const

Optimized version so that we don't have to recompute the index
mappings for each point. ";


// File: classgaia2_1_1MergeRegion.xml
%feature("docstring") gaia2::MergeRegion "

The MergeRegion analyzer allows you to merge multiple descriptors into
a single one.

You lose the ability to discern between them, but you gain a lot of
memory space by doing that.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be merged. Can use
wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

resultName:  the name of the resulting descriptor (default:
\"merged\").

C++ includes: mergeregion.h ";

%feature("docstring")  gaia2::MergeRegion::MergeRegion "gaia2::MergeRegion::MergeRegion(const ParameterMap &params) ";

%feature("docstring")  gaia2::MergeRegion::~MergeRegion "virtual
gaia2::MergeRegion::~MergeRegion() ";

%feature("docstring")  gaia2::MergeRegion::analyze "Transformation
gaia2::MergeRegion::analyze(const DataSet *data) const ";


// File: classgaia2_1_1MergeRegionApplier.xml
%feature("docstring") gaia2::MergeRegionApplier "

MergeRegion applier class.

Merges multiple real descriptors into a single one.

C++ includes: mergeregionapplier.h ";

%feature("docstring")  gaia2::MergeRegionApplier::MergeRegionApplier "MergeRegionApplier::MergeRegionApplier(const Transformation &transfo)
";

%feature("docstring")  gaia2::MergeRegionApplier::~MergeRegionApplier
"MergeRegionApplier::~MergeRegionApplier() ";

%feature("docstring")  gaia2::MergeRegionApplier::mapPoint "Point *
MergeRegionApplier::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classMinAggr.xml
%feature("docstring") MinAggr "";

%feature("docstring")  MinAggr::initValue "Real MinAggr::initValue()
const ";

%feature("docstring")  MinAggr::postProcess "Real
MinAggr::postProcess(Real x, int nsegs) const ";

%feature("docstring")  MinAggr::~MinAggr "virtual MinAggr::~MinAggr()
";


// File: classgaia2_1_1MinkowskiDistance.xml
%feature("docstring") gaia2::MinkowskiDistance "

This class computes the Minkowski distance of a Region.

It is defined as: $ \\\\left( \\\\sum_{i} (a_i - b_i)^x \\\\right) ^
{1/x}$ , for a given x.

Parameters:
-----------

exponent:  the exponent for this distance (the 'x' in the formula) NB:
This class is not implemented at the moment.

C++ includes: minkowskidistance.h ";

%feature("docstring")  gaia2::MinkowskiDistance::MinkowskiDistance "MinkowskiDistance::MinkowskiDistance(const PointLayout &layout, const
ParameterMap &params) ";


// File: classgaia2_1_1yaml_1_1Node.xml
%feature("docstring") gaia2::yaml::Node "";

%feature("docstring")  gaia2::yaml::Node::Node "gaia2::yaml::Node::Node() ";

%feature("docstring")  gaia2::yaml::Node::Node "gaia2::yaml::Node::Node(const QString &str) ";

%feature("docstring")  gaia2::yaml::Node::Node "gaia2::yaml::Node::Node(const char *str) ";

%feature("docstring")  gaia2::yaml::Node::Node "gaia2::yaml::Node::Node(const std::string &str) ";

%feature("docstring")  gaia2::yaml::Node::Node "gaia2::yaml::Node::Node(const Sequence &seq) ";

%feature("docstring")  gaia2::yaml::Node::Node "gaia2::yaml::Node::Node(const Mapping &mapping) ";

%feature("docstring")  gaia2::yaml::Node::Node "gaia2::yaml::Node::Node(const Node &n) ";

%feature("docstring")  gaia2::yaml::Node::toString "QString
gaia2::yaml::Node::toString() const ";

%feature("docstring")  gaia2::yaml::Node::type "NodeType
gaia2::yaml::Node::type() const ";

%feature("docstring")  gaia2::yaml::Node::scalar "const Scalar&
gaia2::yaml::Node::scalar() const ";

%feature("docstring")  gaia2::yaml::Node::scalar "Scalar&
gaia2::yaml::Node::scalar() ";

%feature("docstring")  gaia2::yaml::Node::sequence "const Sequence&
gaia2::yaml::Node::sequence() const ";

%feature("docstring")  gaia2::yaml::Node::sequence "Sequence&
gaia2::yaml::Node::sequence() ";

%feature("docstring")  gaia2::yaml::Node::mapping "const Mapping&
gaia2::yaml::Node::mapping() const ";

%feature("docstring")  gaia2::yaml::Node::mapping "Mapping&
gaia2::yaml::Node::mapping() ";


// File: classgaia2_1_1Normalize.xml
%feature("docstring") gaia2::Normalize "

The Normalize analyzer normalizes real descriptors.

There are various ways of doing normalization, the most common being
to just normalize the values so that they fall within the range [0;
1].

NB: This algorithm does not yet work with multi-segments points.

Parameters:
-----------

collections:  the list of collections from which to gather the stats.
the transformation will still apply to all of them, though. (default:
[], meaning all of them)

descriptorNames:  the names of the descriptors to be included in the
normalization. Only accepts real descriptors, can use wildcards.
(default: \"*\").

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

type:  the type of normalization. Valid types are:  \"range\" which
normalizes using the minimum and maximum values.

\"variance\" which normalizes using the variance of the values.
(default: \"range\").

independent:  indicates whether to normalize each dimension from a
single descriptor independently, or if the vector should be normalized
as a whole (default: false).

outliers:  the ratio (between 0 and 1) of outliers when computing the
statistics used for normalization. Outliers are the values which are
not taken into account because they have too extreme values. A value
of -1 means do not use outliers (default: -1).

applyClipping:  whether to apply clipping, eg: fitting the values into
the [0;1] range (default: false).

C++ includes: normalize.h ";

%feature("docstring")  gaia2::Normalize::Normalize "gaia2::Normalize::Normalize(const ParameterMap &params) ";

%feature("docstring")  gaia2::Normalize::~Normalize "virtual
gaia2::Normalize::~Normalize() ";

%feature("docstring")  gaia2::Normalize::analyze "Transformation
gaia2::Normalize::analyze(const DataSet *data, const Region &region)
const ";


// File: classgaia2_1_1filter_1_1NotFilter.xml
%feature("docstring") gaia2::filter::NotFilter "";

%feature("docstring")  gaia2::filter::NotFilter::NotFilter "gaia2::filter::NotFilter::NotFilter(const Filter *pred) ";


// File: classgaia2_1_1NullDistance.xml
%feature("docstring") gaia2::NullDistance "

This class computes a distance which is always equal to 0.

C++ includes: nulldistance.h ";

%feature("docstring")  gaia2::NullDistance::NullDistance "gaia2::NullDistance::NullDistance(const PointLayout &layout, const
ParameterMap &params) ";


// File: classgaia2_1_1filter_1_1OrFilter.xml
%feature("docstring") gaia2::filter::OrFilter "";

%feature("docstring")  gaia2::filter::OrFilter::OrFilter "gaia2::filter::OrFilter::OrFilter(const QList< Filter *> &preds) ";

%feature("docstring")  gaia2::filter::OrFilter::OrFilter "gaia2::filter::OrFilter::OrFilter(const std::vector< Filter *> &preds)
";


// File: classgaia2_1_1ParamCheckingFactory.xml
%feature("docstring") gaia2::ParamCheckingFactory "";


// File: classgaia2_1_1Parameter.xml
%feature("docstring") gaia2::Parameter "C++ includes: parameter.h ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter() ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const T &val) ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(float x) ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const Array< Real > &val) ";

%feature("docstring")  gaia2::Parameter::toArrayReal "Array< Real >
gaia2::Parameter::toArrayReal() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const RealDescriptor &val) ";

%feature("docstring")  gaia2::Parameter::toRealDescriptor "RealDescriptor gaia2::Parameter::toRealDescriptor() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const StringDescriptor &val) ";

%feature("docstring")  gaia2::Parameter::toStringDescriptor "StringDescriptor gaia2::Parameter::toStringDescriptor() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const ParameterMap &parameterMap) ";

%feature("docstring")  gaia2::Parameter::toParameterMap "ParameterMap
gaia2::Parameter::toParameterMap() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const Eigen::MatrixXf &matrix) ";

%feature("docstring")  gaia2::Parameter::toMatrix "Eigen::MatrixXf
gaia2::Parameter::toMatrix() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const Transformation &transfo) ";

%feature("docstring")  gaia2::Parameter::toTransformation "Transformation gaia2::Parameter::toTransformation() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const TransfoChain &history) ";

%feature("docstring")  gaia2::Parameter::toHistory "TransfoChain
gaia2::Parameter::toHistory() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const QList< QPointF > &coords) ";

%feature("docstring")  gaia2::Parameter::toCoords "QList< QPointF >
gaia2::Parameter::toCoords() const ";

%feature("docstring")  gaia2::Parameter::Parameter "gaia2::Parameter::Parameter(const PointLayout &layout) ";

%feature("docstring")  gaia2::Parameter::toPointLayout "PointLayout
gaia2::Parameter::toPointLayout() const ";


// File: classgaia2std_1_1ParameterMap.xml
%feature("docstring") gaia2std::ParameterMap "";

%feature("docstring")  gaia2std::ParameterMap::ParameterMap "gaia2std::ParameterMap::ParameterMap() ";

%feature("docstring")  gaia2std::ParameterMap::insert "void
gaia2std::ParameterMap::insert(const std::string &key, const
std::string &value) ";

%feature("docstring")  gaia2std::ParameterMap::insert "void
gaia2std::ParameterMap::insert(const std::string &key, float value) ";

%feature("docstring")  gaia2std::ParameterMap::insert "void
gaia2std::ParameterMap::insert(const std::string &key, std::vector<
std::string > value) ";

%feature("docstring")  gaia2std::ParameterMap::insert "void
gaia2std::ParameterMap::insert(const std::string &key, std::vector<
float > value) ";


// File: classgaia2_1_1ParameterMap.xml
%feature("docstring") gaia2::ParameterMap "";

%feature("docstring")  gaia2::ParameterMap::ParameterMap "gaia2::ParameterMap::ParameterMap() ";


// File: classgaia2_1_1PCA.xml
%feature("docstring") gaia2::PCA "

The PCA Analyzer computes thePrincipal Components Analysis of the
given DataSet using the specified descriptors.

NB: the dimension and coveredVariance parameters are exclusive, so you
must specify only one of the two.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
PCA analysis. Only accepts real descriptors, can use wildcards.
(default: \"*\").

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

resultName:  the name of the resulting descriptor. (default: some name
constructed from 'pca', the number of dimensions, and the original
descriptor names)

dimension:  the target (final) dimension.

coveredVariance:  the percentage of the total variance to be covered
by the PCA. The target dimension will then be chosen as the smallest
one that accounts for at least that much variance.

C++ includes: pca.h ";

%feature("docstring")  gaia2::PCA::PCA "gaia2::PCA::PCA(const
ParameterMap &params) ";

%feature("docstring")  gaia2::PCA::~PCA "virtual gaia2::PCA::~PCA()
";

%feature("docstring")  gaia2::PCA::analyze "Transformation
gaia2::PCA::analyze(const DataSet *dataset, const Region &region)
const ";


// File: classgaia2_1_1Point.xml
%feature("docstring") gaia2::Point "C++ includes: point.h ";

%feature("docstring")  gaia2::Point::name "QString
gaia2::Point::name() const

Returns the point name, which also acts as its unique identifier. ";

%feature("docstring")  gaia2::Point::setName "void
gaia2::Point::setName(const QString &name)

Sets the point name, which also acts as its unique identifier. ";

%feature("docstring")  gaia2::Point::load "void Point::load(const
QString &filename, const QStringList &select=QStringList()<< \"*\",
const QStringList &exclude=QStringList())

Loads the given filename into this Point.

Can specify filters for specifying which data to load. ";

%feature("docstring")  gaia2::Point::loadFromString "void
Point::loadFromString(const std::string &str, const QStringList
&select=QStringList()<< \"*\", const QStringList
&exclude=QStringList())

Loads the given string as this Point's data.

Can specify filters for specifying which data to load. ";

%feature("docstring")  gaia2::Point::fromBase64 "void
Point::fromBase64(const std::string &data)

Loads a Point from its base64 representation. ";

%feature("docstring")  gaia2::Point::fromBase64 "void
Point::fromBase64(const QByteArray &data)

Loads a Point from its base64 representation. ";

%feature("docstring")  gaia2::Point::toBase64 "std::string
Point::toBase64() const

Returns a base64 representation for this Point. ";

%feature("docstring")  gaia2::Point::Point "gaia2::Point::Point() ";

%feature("docstring")  gaia2::Point::Point "gaia2::Point::Point(const
Point &rhs) ";

%feature("docstring")  gaia2::Point::Point "Point::Point(const
PointLayout &layout) ";

%feature("docstring")  gaia2::Point::Point "Point::Point(const
QString &name, const PointLayout &layout) ";

%feature("docstring")  gaia2::Point::Point "Point::Point(const
std::string &name, const PointLayout &layout) ";

%feature("docstring")  gaia2::Point::~Point "gaia2std::Point::~Point() ";

%feature("docstring")  gaia2::Point::setSegment "void
Point::setSegment(int nsegment, const Point *point, int origsegment=0)

Copy the segment from the given Point to a segment into this Point.

This method can be used to build a multi-segment point from its
constituent segments given as single points. WARNING: if you call this
method on a Point which has fewer segments than what you require, it
will create those segments, but the value of the data inside those
newly created segments will be undefined.

Parameters:
-----------

nsegment:  the number of the segment to fill

point:  the Point from which to extract the data

origsegment:  the segment inside the original Point to copy ";

%feature("docstring")  gaia2::Point::layout "const PointLayout&
gaia2::Point::layout() const

Returns the current layout of the point. ";

%feature("docstring")  gaia2::Point::layout "PointLayout&
gaia2::Point::layout()

Returns the current layout of the point. ";

%feature("docstring")  gaia2::Point::setLayout "void
Point::setLayout(const PointLayout &layout, int nsegments=0)

Uses the given layout for this point.

This method can optionally take the number of segments this point will
have. If unspecified, or 0, the number of segments will be left
untouched. WARNING: this invalidates all the data in the point. ";

%feature("docstring")  gaia2::Point::switchLayout "void
Point::switchLayout(const PointLayout &layout)

Switches the point to use the given layout.

This only works if the new layout is the same as the one the point had
before. This is used to update the enum values in the points. ";

%feature("docstring")  gaia2::Point::data "const Array<ScopedData>&
gaia2::Point::data() const ";

%feature("docstring")  gaia2::Point::data "Array<ScopedData>&
gaia2::Point::data() ";

%feature("docstring")  gaia2::Point::numberSegments "int
gaia2::Point::numberSegments() const ";

%feature("docstring")  gaia2::Point::value "RealDescriptor
Point::value(const QString &name) const ";

%feature("docstring")  gaia2::Point::value "RealDescriptor
Point::value(int nsegment, const QString &name) const ";

%feature("docstring")  gaia2::Point::label "StringDescriptor
Point::label(const QString &name) const ";

%feature("docstring")  gaia2::Point::label "StringDescriptor
Point::label(int nsegment, const QString &name) const ";

%feature("docstring")  gaia2::Point::setValue "void
Point::setValue(const QString &name, const RealDescriptor &value) ";

%feature("docstring")  gaia2::Point::setValue "void
Point::setValue(int nsegment, const QString &name, const
RealDescriptor &value) ";

%feature("docstring")  gaia2::Point::setLabel "void
Point::setLabel(const QString &name, const StringDescriptor &label) ";

%feature("docstring")  gaia2::Point::setLabel "void
Point::setLabel(int nsegment, const QString &name, const
StringDescriptor &label) ";

%feature("docstring")  gaia2::Point::vrealData "const
Array<RealDescriptor>& gaia2::Point::vrealData(int i=0) const ";

%feature("docstring")  gaia2::Point::vrealData "Array<RealDescriptor>& gaia2::Point::vrealData(int i=0) ";

%feature("docstring")  gaia2::Point::vstringData "const
Array<StringDescriptor>& gaia2::Point::vstringData(int i=0) const ";

%feature("docstring")  gaia2::Point::vstringData "Array<StringDescriptor>& gaia2::Point::vstringData(int i=0) ";

%feature("docstring")  gaia2::Point::venumData "const
Array<EnumDescriptor>& gaia2::Point::venumData(int i=0) const ";

%feature("docstring")  gaia2::Point::venumData "Array<EnumDescriptor>& gaia2::Point::venumData(int i=0) ";

%feature("docstring")  gaia2::Point::frealData "const RealDescriptor&
gaia2::Point::frealData(int i=0) const ";

%feature("docstring")  gaia2::Point::frealData "RealDescriptor&
gaia2::Point::frealData(int i=0) ";

%feature("docstring")  gaia2::Point::fstringData "const
StringDescriptor& gaia2::Point::fstringData(int i=0) const ";

%feature("docstring")  gaia2::Point::fstringData "StringDescriptor&
gaia2::Point::fstringData(int i=0) ";

%feature("docstring")  gaia2::Point::fenumData "const EnumDescriptor&
gaia2::Point::fenumData(int i=0) const ";

%feature("docstring")  gaia2::Point::fenumData "EnumDescriptor&
gaia2::Point::fenumData(int i=0) ";

%feature("docstring")  gaia2::Point::scope "const Scope&
gaia2::Point::scope(int i=0) const ";

%feature("docstring")  gaia2::Point::scope "Scope&
gaia2::Point::scope(int i=0) ";


// File: classgaia2std_1_1Point.xml
%feature("docstring") gaia2std::Point "";

%feature("docstring")  gaia2std::Point::Point "gaia2std::Point::Point() ";

%feature("docstring")  gaia2std::Point::name "string
gaia2std::Point::name() const ";

%feature("docstring")  gaia2std::Point::setName "void
gaia2std::Point::setName(const std::string &name) ";

%feature("docstring")  gaia2std::Point::load "void
gaia2std::Point::load(const std::string &name) ";

%feature("docstring")  gaia2std::Point::value "vector< float >
gaia2std::Point::value(const std::string &name) const ";

%feature("docstring")  gaia2std::Point::label "vector< string >
gaia2std::Point::label(const std::string &name) const ";

%feature("docstring")  gaia2std::Point::value "vector< float >
gaia2std::Point::value(int nsegment, const std::string &name) const ";

%feature("docstring")  gaia2std::Point::label "vector< string >
gaia2std::Point::label(int nsegment, const std::string &name) const ";

%feature("docstring")  gaia2std::Point::setValue "void
gaia2std::Point::setValue(const std::string &name, float value) ";

%feature("docstring")  gaia2std::Point::setValue "void
gaia2std::Point::setValue(const std::string &name, const std::vector<
float > &value) ";

%feature("docstring")  gaia2std::Point::setLabel "void
gaia2std::Point::setLabel(const std::string &name, const std::string
&label) ";

%feature("docstring")  gaia2std::Point::setLabel "void
gaia2std::Point::setLabel(const std::string &name, const std::vector<
std::string > &label) ";

%feature("docstring")  gaia2std::Point::setValue "void
gaia2std::Point::setValue(int nsegment, const std::string &name, float
value) ";

%feature("docstring")  gaia2std::Point::setValue "void
gaia2std::Point::setValue(int nsegment, const std::string &name, const
std::vector< float > &value) ";

%feature("docstring")  gaia2std::Point::setLabel "void
gaia2std::Point::setLabel(int nsegment, const std::string &name, const
std::string &label) ";

%feature("docstring")  gaia2std::Point::setLabel "void
gaia2std::Point::setLabel(int nsegment, const std::string &name, const
std::vector< std::string > &label) ";


// File: classgaia2_1_1PointArray.xml
%feature("docstring") gaia2::PointArray "

A PointArray is a simple subclass of a QVector<Point*> that owns the
memory to the points (hence will delete them when it is itself
deleted).

NB: the PointArray can be told explicitly that it doesn't own the
points, in which case it will behave the same as a simple
QVector<Point*>.

C++ includes: dataset.h ";

%feature("docstring")  gaia2::PointArray::PointArray "gaia2::PointArray::PointArray(int n=0, bool ownsMemory=true) ";

%feature("docstring")  gaia2::PointArray::PointArray "gaia2::PointArray::PointArray(bool ownsMemory) ";

%feature("docstring")  gaia2::PointArray::~PointArray "PointArray::~PointArray() ";

%feature("docstring")  gaia2::PointArray::clear "void
PointArray::clear()

Delete the points this array contains (if it owns them), then resize
the array to 0. ";

%feature("docstring")  gaia2::PointArray::totalSegments "int
PointArray::totalSegments() const

Returns the total number of segments in this PointArray (the sum of
the number of segments for each point in the array). ";

%feature("docstring")  gaia2::PointArray::samplePoint "const Point *
PointArray::samplePoint() const

Returns any single point from the PointArray. ";


// File: classgaia2_1_1PointLayout.xml
%feature("docstring") gaia2::PointLayout "

This class describes the layout of a point.

It maps identifier names (QString) to Region.

C++ includes: pointlayout.h ";

%feature("docstring")  gaia2::PointLayout::PointLayout "PointLayout::PointLayout() ";

%feature("docstring")  gaia2::PointLayout::PointLayout "PointLayout::PointLayout(const PointLayout &other) ";

%feature("docstring")  gaia2::PointLayout::~PointLayout "PointLayout::~PointLayout() ";

%feature("docstring")  gaia2::PointLayout::copy "PointLayout
PointLayout::copy() const ";

%feature("docstring")  gaia2::PointLayout::descriptorLocation "Region
PointLayout::descriptorLocation(const QString &name) const

Returns the region corresponding to the location of the specified
descriptor or group of descriptors. ";

%feature("docstring")  gaia2::PointLayout::descriptorLocation "Region
PointLayout::descriptorLocation(const QStringList &names) const

Returns the locations of all the specified descriptors merged into one
region. ";

%feature("docstring")  gaia2::PointLayout::descriptorName "QString
PointLayout::descriptorName(DescriptorType type, DescriptorLengthType
ltype, int index) const

Returns the name of the descriptor located at specified position. ";

%feature("docstring")  gaia2::PointLayout::descriptorNames "QStringList PointLayout::descriptorNames() const

Returns all descriptor names from this layout. ";

%feature("docstring")  gaia2::PointLayout::descriptorNames "QStringList PointLayout::descriptorNames(const Region &region) const

Returns the names of the descriptors which are contained in the given
region. ";

%feature("docstring")  gaia2::PointLayout::descriptorNames "QStringList PointLayout::descriptorNames(const QStringList &patterns)
const

Returns the descriptor names from this layout which match any of the
given patterns.

Patterns are matched using wildcard matching. ";

%feature("docstring")  gaia2::PointLayout::descriptorNames "QStringList PointLayout::descriptorNames(DescriptorType type, const
QString &pattern) const

Returns the descriptor names from this layout which match the given
pattern and which also are of the specified type.

Patterns are matched using wildcard matching. The value UndefinedType
for the type matches all types. ";

%feature("docstring")  gaia2::PointLayout::descriptorNames "QStringList PointLayout::descriptorNames(DescriptorType type, const
QStringList &patterns=QStringList(\"*\"), bool failOnUnmatched=true)
const

Returns the descriptor names from this layout which match any of the
given patterns and which also are of the specified type.

Patterns are matched using wildcard matching. The value UndefinedType
for the type matches all types.

Parameters:
-----------

failOnUnmatched:  if set to true, will throw an exception if a pattern
didn't match any descriptor. If false, it will ignore this pattern and
go on as normal. ";

%feature("docstring")  gaia2::PointLayout::enumToString "StringDescriptor PointLayout::enumToString(const QString &name, const
EnumDescriptor &value) const

Converts the given Enum descriptor into its String representation. ";

%feature("docstring")  gaia2::PointLayout::enumToString "QString
PointLayout::enumToString(const QString &name, const Enum &value)
const

Converts the given single Enum value into its String representation.
";

%feature("docstring")  gaia2::PointLayout::stringToEnum "EnumDescriptor PointLayout::stringToEnum(const QString &name, const
StringDescriptor &str) const

Converts the given String descriptor into an enum.

If there was no suitable value for it before, add one in the enumMap.
";

%feature("docstring")  gaia2::PointLayout::stringToEnum "Enum
PointLayout::stringToEnum(const QString &name, const QString &str)
const

Converts the given single String value into an enum.

If there was no suitable value for it before, add one in the enumMap.
";

%feature("docstring")  gaia2::PointLayout::isEmpty "bool
gaia2::PointLayout::isEmpty() const

Returns true when this layout is the empty layout (eg: just contains a
root node without children). ";

%feature("docstring")  gaia2::PointLayout::subsetOf "bool
PointLayout::subsetOf(const PointLayout &layout) const

Returns whether the given layout is a subset of this one. ";

%feature("docstring")  gaia2::PointLayout::add "void
PointLayout::add(const QString &nodeName, DescriptorType
type=UndefinedType, DescriptorLengthType ltype=VariableLength, int
size=1)

Adds the given node to this PointLayout.

If the name contains dots, this method will split the name on them and
add the corresponding intermediate nodes (if necessary). ";

%feature("docstring")  gaia2::PointLayout::add "void
PointLayout::add(const QString &parentName, const QString &childName,
DescriptorType type=UndefinedType, DescriptorLengthType
ltype=VariableLength, int size=1)

Adds the given node to this PointLayout, attaching it to the node
called parentName.

If the name contains dots, this method will split the name on them and
add the corresponding intermediate nodes (if necessary). ";

%feature("docstring")  gaia2::PointLayout::remove "void
PointLayout::remove(const QString &name)

Remove the given node from this PointLayout. ";

%feature("docstring")  gaia2::PointLayout::remove "void
PointLayout::remove(const QStringList &names)

Remove the given list of nodes from this PointLayout.

This is substantially faster than repeated calls to remove(const
QString&). ";

%feature("docstring")  gaia2::PointLayout::fixLength "void
PointLayout::fixLength(const QString &name, int size)

Fixes the length of the specified descriptor by changing its length
type from VariableLength to FixedLength.

WARNING: you cannot call this method if the layout is already been
used by a point or dataset. ";

%feature("docstring")  gaia2::PointLayout::fixLength "void
PointLayout::fixLength(const QList< QPair< QString, int > > &descs)

Fixes the length of the specified descriptors by changing their length
type from VariableLength to FixedLength using the associated size. ";

%feature("docstring")  gaia2::PointLayout::enumerate "void
PointLayout::enumerate(const QString &name)

Changes the type of the specified descriptor from StringType to
EnumType.

WARNING: you cannot call this method if the layout is already been
used by a point or dataset. ";

%feature("docstring")  gaia2::PointLayout::symmetricDifferenceWith "QStringList PointLayout::symmetricDifferenceWith(const PointLayout
&layout) const

Returns the list of descriptors which are different in these two
layouts.

In particular, returns all descriptors which are in one layout and not
in the other, or ones which are in both but with different types. ";

%feature("docstring")  gaia2::PointLayout::differenceWith "QStringList PointLayout::differenceWith(const PointLayout &layout)
const

Returns the list of descriptors which are in this layout, but not in
the other one.

Also returns the list of descriptors which are in both but with
different types. ";

%feature("docstring")  gaia2::PointLayout::canMorphInto "bool
PointLayout::canMorphInto(const PointLayout &targetLayout) const

Returns whether this layout can morph into the given target layout.

There are various ways one could achieve this, but at the moment the
conditions we check for this function to succeed are the following:
both layouts have the same descriptor names

all the ones which don't have the same type have a Real type in this
layout and a String type in the target layout.

This can happen when reading a string which can actually be
interpreted as a real value (the yaml parser usually does that if not
told otherwise). ";

%feature("docstring")  gaia2::PointLayout::morphPoint "Point *
PointLayout::morphPoint(const Point *point) const

Morphs the Point given as input so that the returned Point has the
same layout as this layout.

Exceptions:
-----------

GaiaException if point could not be morphed ";

%feature("docstring")  gaia2::PointLayout::filter "void
PointLayout::filter(const QStringList &select, const QStringList
&exclude)

Filters the elements contained in this layout so that the resulting
layout only contains those which name match one of the pattern in the
select argument and doesn't match any of the patterns in the exclude
argument.

You can use shell style globbing in the patterns. ";

%feature("docstring")  gaia2::PointLayout::debug "void
PointLayout::debug() const ";

%feature("docstring")  gaia2::PointLayout::toYaml "QString
PointLayout::toYaml() const

Returns a yaml string representing: the enumeration maps (\"enums\")
that are being used by this layout

the hierachy of descriptors (\"descriptors\") that this layout
contains. ";

%feature("docstring")  gaia2::PointLayout::fullName "QString
PointLayout::fullName(const QString &name) const

Returns the fully qualified name of the given descriptor name. ";

%feature("docstring")  gaia2::PointLayout::regionSize "int
PointLayout::regionSize(DescriptorType type, DescriptorLengthType
ltype) const

Returns the full size of the specified Region.

This is mostly useful for the Point class, so it knows how much space
it needs to allocate. ";

%feature("docstring")  gaia2::PointLayout::correspondingRegion "Region PointLayout::correspondingRegion() const ";

%feature("docstring")  gaia2::PointLayout::sameObjectAs "bool
PointLayout::sameObjectAs(const PointLayout &layout) const

Return whether both layout point to the same object in memory.

We might want to distinguish it from operator==() because the latter
doesn't check for enum2str and str2enum maps. ";

%feature("docstring")  gaia2::PointLayout::sameEnumMap "bool
PointLayout::sameEnumMap(const PointLayout &layout, const QString
&enumName) const ";

%feature("docstring")  gaia2::PointLayout::ref "int
gaia2::PointLayout::ref() const

Reserved for internal testing.

Should not be used. ";


// File: classgaia2_1_1PointLayoutData.xml
%feature("docstring") gaia2::PointLayoutData "";

%feature("docstring")  gaia2::PointLayoutData::PointLayoutData "PointLayoutData::PointLayoutData() ";

%feature("docstring")  gaia2::PointLayoutData::PointLayoutData "PointLayoutData::PointLayoutData(const PointLayoutData &rhs) ";

%feature("docstring")  gaia2::PointLayoutData::~PointLayoutData "PointLayoutData::~PointLayoutData() ";


// File: classgaia2_1_1parser_1_1Predicate.xml
%feature("docstring") gaia2::parser::Predicate "";

%feature("docstring")  gaia2::parser::Predicate::~Predicate "virtual
gaia2::parser::Predicate::~Predicate() ";

%feature("docstring")  gaia2::parser::Predicate::value "virtual bool
gaia2::parser::Predicate::value() const =0 ";

%feature("docstring")  gaia2::parser::Predicate::isTrue "bool
gaia2::parser::Predicate::isTrue() const ";

%feature("docstring")  gaia2::parser::Predicate::toString "virtual
QString gaia2::parser::Predicate::toString() const =0 ";

%feature("docstring")  gaia2::parser::Predicate::copy "virtual
Predicate* gaia2::parser::Predicate::copy() const =0 ";


// File: classgaia2_1_1parser_1_1PredLabelComparison.xml
%feature("docstring") gaia2::parser::PredLabelComparison "";

%feature("docstring")
gaia2::parser::PredLabelComparison::PredLabelComparison "gaia2::parser::PredLabelComparison::PredLabelComparison(Label *lhs,
int type, Label *rhs) ";

%feature("docstring")
gaia2::parser::PredLabelComparison::PredLabelComparison "gaia2::parser::PredLabelComparison::PredLabelComparison(const
PredLabelComparison &other) ";

%feature("docstring")
gaia2::parser::PredLabelComparison::~PredLabelComparison "gaia2::parser::PredLabelComparison::~PredLabelComparison() ";

%feature("docstring")  gaia2::parser::PredLabelComparison::copy "virtual Predicate* gaia2::parser::PredLabelComparison::copy() const ";

%feature("docstring")  gaia2::parser::PredLabelComparison::value "virtual bool gaia2::parser::PredLabelComparison::value() const ";

%feature("docstring")  gaia2::parser::PredLabelComparison::toString "virtual QString gaia2::parser::PredLabelComparison::toString() const
";


// File: classgaia2_1_1parser_1_1PredLabelIsIn.xml
%feature("docstring") gaia2::parser::PredLabelIsIn "";

%feature("docstring")  gaia2::parser::PredLabelIsIn::PredLabelIsIn "gaia2::parser::PredLabelIsIn::PredLabelIsIn(Label *var, const
QStringList &slist) ";

%feature("docstring")  gaia2::parser::PredLabelIsIn::PredLabelIsIn "gaia2::parser::PredLabelIsIn::PredLabelIsIn(const PredLabelIsIn
&other) ";

%feature("docstring")  gaia2::parser::PredLabelIsIn::~PredLabelIsIn "gaia2::parser::PredLabelIsIn::~PredLabelIsIn() ";

%feature("docstring")  gaia2::parser::PredLabelIsIn::copy "virtual
Predicate* gaia2::parser::PredLabelIsIn::copy() const ";

%feature("docstring")  gaia2::parser::PredLabelIsIn::value "virtual
bool gaia2::parser::PredLabelIsIn::value() const ";

%feature("docstring")  gaia2::parser::PredLabelIsIn::toString "virtual QString gaia2::parser::PredLabelIsIn::toString() const ";


// File: classgaia2_1_1parser_1_1PredLogicalOp.xml
%feature("docstring") gaia2::parser::PredLogicalOp "";

%feature("docstring")  gaia2::parser::PredLogicalOp::PredLogicalOp "gaia2::parser::PredLogicalOp::PredLogicalOp(Predicate *lhs, int op,
Predicate *rhs) ";

%feature("docstring")  gaia2::parser::PredLogicalOp::PredLogicalOp "gaia2::parser::PredLogicalOp::PredLogicalOp(const PredLogicalOp
&other) ";

%feature("docstring")  gaia2::parser::PredLogicalOp::~PredLogicalOp "gaia2::parser::PredLogicalOp::~PredLogicalOp() ";

%feature("docstring")  gaia2::parser::PredLogicalOp::copy "virtual
Predicate* gaia2::parser::PredLogicalOp::copy() const ";

%feature("docstring")  gaia2::parser::PredLogicalOp::value "virtual
bool gaia2::parser::PredLogicalOp::value() const ";

%feature("docstring")  gaia2::parser::PredLogicalOp::toString "virtual QString gaia2::parser::PredLogicalOp::toString() const ";


// File: classgaia2_1_1parser_1_1PredNegate.xml
%feature("docstring") gaia2::parser::PredNegate "";

%feature("docstring")  gaia2::parser::PredNegate::PredNegate "gaia2::parser::PredNegate::PredNegate(Predicate *pred) ";

%feature("docstring")  gaia2::parser::PredNegate::PredNegate "gaia2::parser::PredNegate::PredNegate(const PredNegate &other) ";

%feature("docstring")  gaia2::parser::PredNegate::~PredNegate "gaia2::parser::PredNegate::~PredNegate() ";

%feature("docstring")  gaia2::parser::PredNegate::copy "virtual
Predicate* gaia2::parser::PredNegate::copy() const ";

%feature("docstring")  gaia2::parser::PredNegate::value "virtual bool
gaia2::parser::PredNegate::value() const ";

%feature("docstring")  gaia2::parser::PredNegate::toString "virtual
QString gaia2::parser::PredNegate::toString() const ";


// File: classgaia2_1_1parser_1_1PredValueComparison.xml
%feature("docstring") gaia2::parser::PredValueComparison "";

%feature("docstring")
gaia2::parser::PredValueComparison::PredValueComparison "gaia2::parser::PredValueComparison::PredValueComparison(Value *lhs,
int type, Value *rhs) ";

%feature("docstring")
gaia2::parser::PredValueComparison::PredValueComparison "gaia2::parser::PredValueComparison::PredValueComparison(const
PredValueComparison &other) ";

%feature("docstring")
gaia2::parser::PredValueComparison::~PredValueComparison "gaia2::parser::PredValueComparison::~PredValueComparison() ";

%feature("docstring")  gaia2::parser::PredValueComparison::copy "virtual Predicate* gaia2::parser::PredValueComparison::copy() const ";

%feature("docstring")  gaia2::parser::PredValueComparison::value "virtual bool gaia2::parser::PredValueComparison::value() const ";

%feature("docstring")  gaia2::parser::PredValueComparison::toString "virtual QString gaia2::parser::PredValueComparison::toString() const
";


// File: classgaia2_1_1parser_1_1PredValueIsIn.xml
%feature("docstring") gaia2::parser::PredValueIsIn "";

%feature("docstring")  gaia2::parser::PredValueIsIn::PredValueIsIn "gaia2::parser::PredValueIsIn::PredValueIsIn(Value *var, const QList<
Real > &vlist) ";

%feature("docstring")  gaia2::parser::PredValueIsIn::PredValueIsIn "gaia2::parser::PredValueIsIn::PredValueIsIn(const PredValueIsIn
&other) ";

%feature("docstring")  gaia2::parser::PredValueIsIn::~PredValueIsIn "gaia2::parser::PredValueIsIn::~PredValueIsIn() ";

%feature("docstring")  gaia2::parser::PredValueIsIn::copy "virtual
Predicate* gaia2::parser::PredValueIsIn::copy() const ";

%feature("docstring")  gaia2::parser::PredValueIsIn::value "virtual
bool gaia2::parser::PredValueIsIn::value() const ";

%feature("docstring")  gaia2::parser::PredValueIsIn::toString "virtual QString gaia2::parser::PredValueIsIn::toString() const ";


// File: classgaia2_1_1parser_1_1PredValueRange.xml
%feature("docstring") gaia2::parser::PredValueRange "";

%feature("docstring")  gaia2::parser::PredValueRange::PredValueRange "gaia2::parser::PredValueRange::PredValueRange(Value *var, Real minv,
Real maxv) ";

%feature("docstring")  gaia2::parser::PredValueRange::PredValueRange "gaia2::parser::PredValueRange::PredValueRange(const PredValueRange
&other) ";

%feature("docstring")  gaia2::parser::PredValueRange::~PredValueRange
"gaia2::parser::PredValueRange::~PredValueRange() ";

%feature("docstring")  gaia2::parser::PredValueRange::copy "virtual
Predicate* gaia2::parser::PredValueRange::copy() const ";

%feature("docstring")  gaia2::parser::PredValueRange::value "virtual
bool gaia2::parser::PredValueRange::value() const ";

%feature("docstring")  gaia2::parser::PredValueRange::toString "virtual QString gaia2::parser::PredValueRange::toString() const ";


// File: classgaia2_1_1RandomDistance.xml
%feature("docstring") gaia2::RandomDistance "

This class computes a distance which returns a random number between 0
and the specified range.

It uses a uniform distribution.

Parameters:
-----------

range:  the max value this distance can return (minimum being 0).

C++ includes: randomdistance.h ";

%feature("docstring")  gaia2::RandomDistance::RandomDistance "RandomDistance::RandomDistance(const PointLayout &, const ParameterMap
&) ";


// File: classgaia2_1_1RCA.xml
%feature("docstring") gaia2::RCA "

The RCA Analyzer computes theRelevant Components Analysis of the given
DataSet using the specified descriptors.

Note: the use of classFile, classMap and className is exclusive, ie:
you need to specify one and only one of them.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be included in the
RCA analysis. Only accepts real descriptors, can use wildcards.
(default: \"*\").

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

resultName:  the name of the resulting descriptor. (default: some name
constructed from 'rca', the number of dimensions, and the original
descriptor names)

dimension:  the target (final) dimension.

classFile:  the name of the file containing the list of classes
(chunklets in RCA language).

classMap:  the groundtruth map, from point IDs to their class value.

className:  the name of the descriptor in the PointLayout that will be
used as class label.

C++ includes: rca.h ";

%feature("docstring")  gaia2::RCA::RCA "gaia2::RCA::RCA(const
ParameterMap &params) ";

%feature("docstring")  gaia2::RCA::~RCA "virtual gaia2::RCA::~RCA()
";

%feature("docstring")  gaia2::RCA::analyze "Transformation
gaia2::RCA::analyze(const DataSet *dataset, const Region &region)
const

Works exactly like the matlab algo, so it should be ok. However
there's something that smells fishy to me so better check it out
sometime... ";


// File: classgaia2_1_1Region.xml
%feature("docstring") gaia2::Region "

A region is a physical location in the point layout which consists in
a list of segments.

It does not require all the segments to be of the same type.

C++ includes: region.h ";

%feature("docstring")  gaia2::Region::merge "Region &
Region::merge(const Region &r)

Merges another Region with this one.

Surely there is an algorithmically \"smart\" way to do this... ";

%feature("docstring")  gaia2::Region::type "DescriptorType
Region::type() const

Returns the descriptor type if all segments within this region are of
the same type.

Exceptions:
-----------

GaiaException if region is empty or if there are at least 2 segments
of different types in this region. ";

%feature("docstring")  gaia2::Region::lengthType "DescriptorLengthType Region::lengthType() const

Returns the descriptor length type if all segments within this region
are of the same type.

Exceptions:
-----------

GaiaException if region is empty or if there are at least 2 segments
of different types in this region. ";

%feature("docstring")  gaia2::Region::index "int Region::index()
const

Returns the index corresponding to the descriptor contained in this
region if and only if this region is composed of a single segment of
size 1 (eg: it only contains one single descriptor).

Throws an error if the region is empty or contains more than one
descriptor. ";

%feature("docstring")  gaia2::Region::index "int
Region::index(DescriptorType type, DescriptorLengthType ltype) const

Does the same thing as the index() method without arguments, but also
checks if the region type is of the specified type and the length-type
is the same also, otherwise throws an error. ";

%feature("docstring")  gaia2::Region::index "int
Region::index(DescriptorLengthType ltype) const

Does the same thing as the index() method without arguments, but also
checks if the length type is the same. ";

%feature("docstring")  gaia2::Region::checkTypeOnlyFrom "void
Region::checkTypeOnlyFrom(DescriptorType type, const PointLayout
*layout=0) const

Checks that all segments in this region are of the same type.

Does nothing on success, and throws an exception if this condition is
not fulfilled.

You can optionally specify a PointLayout, which is only used for
giving full descriptor names in case of an error. ";

%feature("docstring")  gaia2::Region::isTypeOnlyFrom "bool
Region::isTypeOnlyFrom(DescriptorType type) const

Returns whether all segments in this region are of the same given
type. ";

%feature("docstring")  gaia2::Region::checkLengthTypeOnlyFrom "void
Region::checkLengthTypeOnlyFrom(DescriptorLengthType ltype, const
PointLayout *layout=0) const

Checks that all segments in this region are of the same length type.

Does nothing on success, and throws an exception if this condition is
not fulfilled.

You can optionally specify a PointLayout, which is only used for
giving full descriptor names in case of an error. ";

%feature("docstring")  gaia2::Region::isLengthTypeOnlyFrom "bool
Region::isLengthTypeOnlyFrom(DescriptorLengthType type) const

Returns whether all segments in this region are of the same given
length type. ";

%feature("docstring")  gaia2::Region::checkSingleDescriptor "void
Region::checkSingleDescriptor() const

Checks that this Region is composed of only 1 descriptor.

Does nothing on success and throws an exception if this condition is
not fulfilled. ";

%feature("docstring")  gaia2::Region::dimension "int
Region::dimension(DescriptorType type=UndefinedType, const Point *p=0)
const

Returns the total dimension of all the fields of the specified type in
the given point.

Note: you need to specify a point because of variable-length fields
which change from one point to another. ";

%feature("docstring")  gaia2::Region::segment "const Segment &
Region::segment() const

If this region corresponds to a single descriptor, returns its
corresponding Segment.

Throws an exception otherwise. ";

%feature("docstring")  gaia2::Region::segment "const Segment &
Region::segment(DescriptorType type) const

If this region corresponds to a single descriptor of the specified
type, returns its corresponding Segment.

Throws an exception otherwise. ";

%feature("docstring")  gaia2::Region::size "int
Region::size(DescriptorType type, DescriptorLengthType ltype) const

Returns the size of the part of the region which is of the specified
type and length type, eg: the sum of the sizes of its segments. ";

%feature("docstring")  gaia2::Region::listIndices "QVector< int >
Region::listIndices(DescriptorType type, DescriptorLengthType ltype)
const ";

%feature("docstring")  gaia2::Region::select "Region
Region::select(DescriptorType type) const

Returns a new region which is a subset of this one and contains all
the segments of the specified type. ";

%feature("docstring")  gaia2::Region::select "Region
Region::select(DescriptorType type, DescriptorLengthType ltype) const

Returns a new region which is a subset of this one and contains all
the segments of the specified type. ";

%feature("docstring")  gaia2::Region::toString "QString
Region::toString() const ";

%feature("docstring")  gaia2::Region::debug "void Region::debug() ";

%feature("docstring")  gaia2::Region::canonical "void
Region::canonical()

Tries to simplify segments if there are contiguous segments, ie:
[0,10[ U [10,20[ = [0,20[. ";


// File: classgaia2_1_1Factory_1_1Registrar.xml
%feature("docstring") gaia2::Factory::Registrar "";

%feature("docstring")  gaia2::Factory::Registrar::Registrar "gaia2::Factory< IdentifierType, AbstractProduct, ArgumentType
>::Registrar< ConcreteProduct >::Registrar(const IdentifierType &id)
";


// File: classgaia2_1_1LayoutAwareFactory_1_1Registrar.xml
%feature("docstring") gaia2::LayoutAwareFactory::Registrar "";

%feature("docstring")  gaia2::LayoutAwareFactory::Registrar::Registrar
"gaia2::LayoutAwareFactory< IdentifierType, AbstractProduct,
LayoutType, ArgumentType >::Registrar< ConcreteProduct
>::Registrar(const IdentifierType &id) ";


// File: classgaia2_1_1Remove.xml
%feature("docstring") gaia2::Remove "

The Remove analyzer allows you to discard specific descriptors.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be discarded. Can
use wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

failOnUnmatched:  whether to fail if one of the given patterns didn't
match any descriptor in the layout.

C++ includes: remove.h ";

%feature("docstring")  gaia2::Remove::Remove "gaia2::Remove::Remove(const ParameterMap &params) ";

%feature("docstring")  gaia2::Remove::~Remove "virtual
gaia2::Remove::~Remove() ";

%feature("docstring")  gaia2::Remove::analyze "Transformation
gaia2::Remove::analyze(const DataSet *data) const ";


// File: classgaia2_1_1RemoveDesc.xml
%feature("docstring") gaia2::RemoveDesc "

RemoveDesc applier class.

Removes specified dimensions from given point.

C++ includes: removedesc.h ";

%feature("docstring")  gaia2::RemoveDesc::RemoveDesc "RemoveDesc::RemoveDesc(const Transformation &transfo) ";

%feature("docstring")  gaia2::RemoveDesc::~RemoveDesc "RemoveDesc::~RemoveDesc() ";

%feature("docstring")  gaia2::RemoveDesc::mapPoint "Point *
RemoveDesc::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classgaia2_1_1RemoveVL.xml
%feature("docstring") gaia2::RemoveVL "

The RemoveVL analyzer checks for descriptors which are variable-length
descriptors, such as the list of beats, ticks, chords, etc. and
removes them.

C++ includes: removevl.h ";

%feature("docstring")  gaia2::RemoveVL::RemoveVL "gaia2::RemoveVL::RemoveVL(const ParameterMap &params) ";

%feature("docstring")  gaia2::RemoveVL::~RemoveVL "virtual
gaia2::RemoveVL::~RemoveVL() ";

%feature("docstring")  gaia2::RemoveVL::analyze "Transformation
gaia2::RemoveVL::analyze(const DataSet *data) const ";


// File: classgaia2_1_1Rename.xml
%feature("docstring") gaia2::Rename "

The Rename analyzer allows you to rename an existing field.

Parameters:
-----------

descriptorName:  the name of the field you want to rename.

newName:  the new names to give to the field.

C++ includes: rename.h ";

%feature("docstring")  gaia2::Rename::Rename "gaia2::Rename::Rename(const ParameterMap &params) ";

%feature("docstring")  gaia2::Rename::~Rename "virtual
gaia2::Rename::~Rename() ";

%feature("docstring")  gaia2::Rename::analyze "Transformation
gaia2::Rename::analyze(const DataSet *data) const ";


// File: classgaia2_1_1RenameApplier.xml
%feature("docstring") gaia2::RenameApplier "

Rename applier class.

Renames a descriptor.

C++ includes: renameapplier.h ";

%feature("docstring")  gaia2::RenameApplier::RenameApplier "RenameApplier::RenameApplier(const Transformation &transfo) ";

%feature("docstring")  gaia2::RenameApplier::~RenameApplier "RenameApplier::~RenameApplier() ";

%feature("docstring")  gaia2::RenameApplier::mapPoint "Point *
RenameApplier::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classgaia2_1_1ResistorAverageDistance.xml
%feature("docstring") gaia2::ResistorAverageDistance "

This class computes the symmetric resistor-average distance of a given
Region.

This distance is based on the harmonic mean of the Kullback-Leibler
divergence. More details can be found in the \"Symmetrizing the
Kullback-Leibler Distance\" 2001 paper by Don H. Johnson and Sinan
Sinanović.

NB: compared to the Kullback-Leibler distance, this one also needs the
determinant of the covariance matrix to be precomputed.

Parameters:
-----------

descriptorName:  the name of the descriptor for which you want to
compute the KL- distance. This descriptor needs to have 4 sub-
descriptors: mean, cov, icov and covdet. (representing, resp., the
mean of the frames, the covariance matrix, the inverse of the
covariance matrix and the determinant of the covariance matrix).

C++ includes: resistoraveragedistance.h ";

%feature("docstring")
gaia2::ResistorAverageDistance::ResistorAverageDistance "ResistorAverageDistance::ResistorAverageDistance(const PointLayout
&layout, const ParameterMap &params) ";


// File: classgaia2_1_1RhythmDistance.xml
%feature("docstring") gaia2::RhythmDistance "

This class computes the rhythm distance of a given descriptor.

It is defined as $d_{BPM}(X,Y) =
\\\\min_{i=1,2,4,6,...}{\\\\alpha_{BPM}^{i-1}|\\\\frac{X_{BPM}}{Y_{BPM}}
- i|}, X_{BPM} > Y_{BPM}$ .

Parameters:
-----------

descriptorName:  the names of the descriptor to be used for the
computation of the distance. Only accepts real descriptors.

alpha:  the alpha coefficient (from the formula)

indexRange:  the list of indices  upon which to iterate.

C++ includes: rhythmdistance.h ";

%feature("docstring")  gaia2::RhythmDistance::RhythmDistance "RhythmDistance::RhythmDistance(const PointLayout &layout, const
ParameterMap &params) ";


// File: classgaia2_1_1Scope.xml
%feature("docstring") gaia2::Scope "";

%feature("docstring")  gaia2::Scope::Scope "gaia2::Scope::Scope() ";


// File: classgaia2_1_1ScopedData.xml
%feature("docstring") gaia2::ScopedData "";


// File: classgaia2_1_1SearchPoint.xml
%feature("docstring") gaia2::SearchPoint "";

%feature("docstring")  gaia2::SearchPoint::SearchPoint "gaia2::SearchPoint::SearchPoint() ";

%feature("docstring")  gaia2::SearchPoint::SearchPoint "gaia2::SearchPoint::SearchPoint(const Point *pptr, const Point *rref)
";


// File: classgaia2_1_1SearchSpaceWrapper.xml
%feature("docstring") gaia2::SearchSpaceWrapper "";

%feature("docstring")  gaia2::SearchSpaceWrapper::SearchSpaceWrapper "gaia2::SearchSpaceWrapper< SearchPointType, DataSetType
>::SearchSpaceWrapper(BaseSearchSpace< SearchPointType, DataSetType >
*s) ";

%feature("docstring")  gaia2::SearchSpaceWrapper::~SearchSpaceWrapper
"SearchSpaceWrapper::~SearchSpaceWrapper() ";


// File: classgaia2_1_1Segment.xml
%feature("docstring") gaia2::Segment "

A segment is a physical contiguous location of descriptors of the same
type in the point layout.

It has an additional property which is whether the segment is to be
taken in the fixed-length or variable-length part of the data (for
memory optimization reasons).

C++ includes: region.h ";

%feature("docstring")  gaia2::Segment::Segment "gaia2::Segment::Segment() ";

%feature("docstring")  gaia2::Segment::Segment "gaia2::Segment::Segment(QString name, DescriptorType type) ";

%feature("docstring")  gaia2::Segment::Segment "gaia2::Segment::Segment(QString name, DescriptorType type,
DescriptorLengthType ltype, int begin, int end) ";

%feature("docstring")  gaia2::Segment::size "int
gaia2::Segment::size() const ";

%feature("docstring")  gaia2::Segment::toString "QString
Segment::toString() const ";


// File: classgaia2_1_1Select.xml
%feature("docstring") gaia2::Select "

The Select analyzer allows you to keep only specific descriptors.

Parameters:
-----------

descriptorNames:  the names of the descriptors to be kept. Can use
wildcards.

except:  the names of the descriptors to be removed from the selected
list specified before. Can use wildcards. (default: \"\").

C++ includes: select.h ";

%feature("docstring")  gaia2::Select::Select "gaia2::Select::Select(const ParameterMap &params) ";

%feature("docstring")  gaia2::Select::~Select "virtual
gaia2::Select::~Select() ";

%feature("docstring")  gaia2::Select::analyze "Transformation
gaia2::Select::analyze(const DataSet *data) const ";


// File: classgaia2_1_1SelectDesc.xml
%feature("docstring") gaia2::SelectDesc "

SelectDesc applier class.

Selects specified dimensions from given point.

C++ includes: selectdesc.h ";

%feature("docstring")  gaia2::SelectDesc::SelectDesc "SelectDesc::SelectDesc(const Transformation &transfo) ";

%feature("docstring")  gaia2::SelectDesc::~SelectDesc "SelectDesc::~SelectDesc() ";

%feature("docstring")  gaia2::SelectDesc::mapPoint "Point *
SelectDesc::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";


// File: classgaia2_1_1SemanticClassifierDistance.xml
%feature("docstring") gaia2::SemanticClassifierDistance "

: desc is wrong, still the wpearson one...

This class computes the semantic classifier distance between 2 points.
It is defined as $ d (X,Y) = \\\\sum{w_i\\\\frac{e^{(x_i-
y_i)^{\\\\alpha}}}{\\\\beta+e^{x_i+y_i}}} $

the alpha coefficient in the above formula  the beta coefficient in
the above formula

Parameters:
-----------

weights:  a mapping from descriptor name to its weight.

C++ includes: semanticclassifierdistance.h ";

%feature("docstring")
gaia2::SemanticClassifierDistance::SemanticClassifierDistance "SemanticClassifierDistance::SemanticClassifierDistance(const
PointLayout &layout, const ParameterMap &params) ";


// File: classgaia2_1_1SingleValueMorphable.xml
%feature("docstring") gaia2::SingleValueMorphable "";

%feature("docstring")
gaia2::SingleValueMorphable::SingleValueMorphable "gaia2::SingleValueMorphable< T >::SingleValueMorphable() ";

%feature("docstring")
gaia2::SingleValueMorphable::SingleValueMorphable "gaia2::SingleValueMorphable< T >::SingleValueMorphable(int size, const
T &value) ";

%feature("docstring")
gaia2::SingleValueMorphable::SingleValueMorphable "gaia2::SingleValueMorphable< T >::SingleValueMorphable(const T &value)
";

%feature("docstring")
gaia2::SingleValueMorphable::SingleValueMorphable "gaia2::SingleValueMorphable< T >::SingleValueMorphable(const
SingleValueMorphable &value) ";

%feature("docstring")
gaia2::SingleValueMorphable::SingleValueMorphable "gaia2::SingleValueMorphable< T >::SingleValueMorphable(const T *arr,
int size) ";

%feature("docstring")
gaia2::SingleValueMorphable::SingleValueMorphable "gaia2::SingleValueMorphable< T >::SingleValueMorphable(const
std::vector< T > &v) ";

%feature("docstring")  gaia2::SingleValueMorphable::toSingleValue "const T& gaia2::SingleValueMorphable< T >::toSingleValue() const ";

%feature("docstring")  gaia2::SingleValueMorphable::checkSize "void
gaia2::SingleValueMorphable< T >::checkSize(const SingleValueMorphable
&rhs) const ";

%feature("docstring")
gaia2::SingleValueMorphable::transferAssignmentOperator "gaia2::SingleValueMorphable< T >::transferAssignmentOperator(+=) ";

%feature("docstring")
gaia2::SingleValueMorphable::transferAssignmentOperator "gaia2::SingleValueMorphable< T >::transferAssignmentOperator(-=) ";

%feature("docstring")
gaia2::SingleValueMorphable::transferAssignmentOperator "transferAssignmentOperator* gaia2::SingleValueMorphable< T
>::transferAssignmentOperator(/=);#define transferBinaryOperator(OP)
transferBinaryOperator(+ ";

%feature("docstring")
gaia2::SingleValueMorphable::transferBinaryOperator "gaia2::SingleValueMorphable< T >::transferBinaryOperator(-) ";

%feature("docstring")
gaia2::SingleValueMorphable::transferBinaryOperator "transferBinaryOperator* gaia2::SingleValueMorphable< T
>::transferBinaryOperator(/);#define transferUnaryOperator(OP)
transferUnaryOperator(- ";

%feature("docstring")
gaia2::SingleValueMorphable::transferAssignmentOperatorFromSingleValue
"gaia2::SingleValueMorphable< T
>::transferAssignmentOperatorFromSingleValue(+=) ";

%feature("docstring")
gaia2::SingleValueMorphable::transferAssignmentOperatorFromSingleValue
"gaia2::SingleValueMorphable< T
>::transferAssignmentOperatorFromSingleValue(-=) ";

%feature("docstring")
gaia2::SingleValueMorphable::transferAssignmentOperatorFromSingleValue
"transferAssignmentOperatorFromSingleValue*
gaia2::SingleValueMorphable< T
>::transferAssignmentOperatorFromSingleValue(/=);#define
transferBinaryOperatorFromSingleValue(OP)
transferBinaryOperatorFromSingleValue(+ ";

%feature("docstring")
gaia2::SingleValueMorphable::transferBinaryOperatorFromSingleValue "gaia2::SingleValueMorphable< T
>::transferBinaryOperatorFromSingleValue(-) ";

%feature("docstring")
gaia2::SingleValueMorphable::transferBinaryOperatorFromSingleValue "transferBinaryOperatorFromSingleValue* gaia2::SingleValueMorphable< T
>::transferBinaryOperatorFromSingleValue(/);#define
transferComparison(OP) transferComparison(< ";

%feature("docstring")  gaia2::SingleValueMorphable::transferComparison
"gaia2::SingleValueMorphable< T >::transferComparison(<=) ";

%feature("docstring")  gaia2::SingleValueMorphable::transferComparison
"gaia2::SingleValueMorphable< T >::transferComparison(>) ";

%feature("docstring")  gaia2::SingleValueMorphable::transferComparison
"gaia2::SingleValueMorphable< T >::transferComparison(>=) ";

%feature("docstring")  gaia2::SingleValueMorphable::minimum "SingleValueMorphable& gaia2::SingleValueMorphable< T >::minimum(const
SingleValueMorphable &rhs) ";

%feature("docstring")  gaia2::SingleValueMorphable::maximum "SingleValueMorphable& gaia2::SingleValueMorphable< T >::maximum(const
SingleValueMorphable &rhs) ";

%feature("docstring")  gaia2::SingleValueMorphable::minimum "SingleValueMorphable& gaia2::SingleValueMorphable< T >::minimum(const
T &minv) ";

%feature("docstring")  gaia2::SingleValueMorphable::maximum "SingleValueMorphable& gaia2::SingleValueMorphable< T >::maximum(const
T &maxv) ";

%feature("docstring")  gaia2::SingleValueMorphable::isnan "bool
gaia2::SingleValueMorphable< T >::isnan() const ";

%feature("docstring")  gaia2::SingleValueMorphable::isinf "bool
gaia2::SingleValueMorphable< T >::isinf() const ";

%feature("docstring")  gaia2::SingleValueMorphable::inverse "SingleValueMorphable& gaia2::SingleValueMorphable< T >::inverse() ";

%feature("docstring")  gaia2::SingleValueMorphable::opposite "SingleValueMorphable& gaia2::SingleValueMorphable< T >::opposite() ";

%feature("docstring")  gaia2::SingleValueMorphable::sqrt "SingleValueMorphable& gaia2::SingleValueMorphable< T >::sqrt() ";

%feature("docstring")  gaia2::SingleValueMorphable::abs "SingleValueMorphable& gaia2::SingleValueMorphable< T >::abs() ";

%feature("docstring")  gaia2::SingleValueMorphable::norm "T
gaia2::SingleValueMorphable< T >::norm() const ";

%feature("docstring")  gaia2::SingleValueMorphable::toString "QString
gaia2::SingleValueMorphable< T >::toString() const ";


// File: classgaia2_1_1SortNode.xml
%feature("docstring") gaia2::SortNode "";

%feature("docstring")  gaia2::SortNode::SortNode "gaia2::SortNode< T
>::SortNode(float d=0.0, T *p=0) ";


// File: classSortOn.xml
%feature("docstring") SortOn "";

%feature("docstring")  SortOn::SortOn "SortOn< DataSetType, DescType
>::SortOn(const DataSetType *dataset, const QString &descriptorName)
";


// File: classgaia2_1_1SortOn.xml
%feature("docstring") gaia2::SortOn "";

%feature("docstring")  gaia2::SortOn::SortOn "gaia2::SortOn<
DataSetType, DescType >::SortOn(const DataSetType *dataset, const
QString &descriptorName) ";


// File: classgaia2_1_1Stringifier.xml
%feature("docstring") gaia2::Stringifier "";

%feature("docstring")  gaia2::Stringifier::Stringifier "gaia2::Stringifier::Stringifier() ";

%feature("docstring")  gaia2::Stringifier::str "const QString&
gaia2::Stringifier::str() const ";


// File: classgaia2_1_1SummedSegmentDistance.xml
%feature("docstring") gaia2::SummedSegmentDistance "

This class calculates a distance between segments from two points and
sums the results.

The cross param defines whether only segments at the same position
should be compared, or each with each. The function used to calculate
the distance between two segments is given by the distance param.

WARNING: this metric is not final, and it might change its behavior in
the future.

Parameters:
-----------

distance:  the name of the distance to be compressed.

params:  the ParameterMap to be used when creating the specified
distance.

cross:  compare segments at the same position or each with each

C++ includes: summedsegmentdistance.h ";

%feature("docstring")
gaia2::SummedSegmentDistance::SummedSegmentDistance "SummedSegmentDistance::SummedSegmentDistance(const PointLayout
&layout, const ParameterMap &params) ";


// File: classgaia2_1_1SVMPredict.xml
%feature("docstring") gaia2::SVMPredict "

SVMPredict applier class.

Predicts the class of a point using a given SVM model.

C++ includes: svmpredict.h ";

%feature("docstring")  gaia2::SVMPredict::SVMPredict "SVMPredict::SVMPredict(const Transformation &transfo) ";

%feature("docstring")  gaia2::SVMPredict::~SVMPredict "SVMPredict::~SVMPredict() ";

%feature("docstring")  gaia2::SVMPredict::mapPoint "Point *
SVMPredict::mapPoint(const Point *p) const

Ownership of resulting point is handed to caller of this function. ";

%feature("docstring")  gaia2::SVMPredict::mapDataSet "DataSet *
SVMPredict::mapDataSet(const DataSet *dataset) const

Provides a default function that maps a whole dataset but which can be
overriden in derived classes to provide a more efficient
implementation. ";


// File: classgaia2_1_1SVMTrain.xml
%feature("docstring") gaia2::SVMTrain "

The SVMTrain analyzer creates a SVM model for the given dataset using
libsvm.

Chih-Chung Chang and Chih-Jen Lin, LIBSVM : a library for support
vector machines, 2001. Software available
athttp://www.csie.ntu.edu.tw/~cjlin/libsvm.

Parameters:
-----------

className:  the name of the descriptor which is to be used as the
class

type:  the SVM type. Available types are: C-SVC

nu-SVC

one-class

epsilon-SVR

nu-SVR  (default: C-SVC)

kernel:  the kernel type. Available types are: linear

polynomial (poly)

radial (basis function) (also called RBF)

sigmoid  (default: RBF)

probability:  whether to train the model for probability estimates
(default: false)

degree:  the degree in the kernel function (default 3)

c:  the cost parameter C used in C-SVC, epsilon-SVR, and nu-SVR
(default 1)

nu:  the nu parameter of nu-SVC, one-class SVM, and nu-SVR (default
0.5)

gamma:  the gamma coeff in the kernel function (default 1/dim)

C++ includes: svmtrain.h ";

%feature("docstring")  gaia2::SVMTrain::SVMTrain "gaia2::SVMTrain::SVMTrain(const ParameterMap &params) ";

%feature("docstring")  gaia2::SVMTrain::~SVMTrain "virtual
gaia2::SVMTrain::~SVMTrain() ";

%feature("docstring")  gaia2::SVMTrain::analyze "Transformation
gaia2::SVMTrain::analyze(const DataSet *data) const ";


// File: classgaia2_1_1TextProgress.xml
%feature("docstring") gaia2::TextProgress "

This class is a very simple class which allows you to print a progress
percentage on the standard output.

It is initialized with the value to be reached (which represents 100%)
and then your only task is to feed it with the progress value and it
will display itself correctly.

C++ includes: textprogress.h ";

%feature("docstring")  gaia2::TextProgress::TextProgress "gaia2::TextProgress::TextProgress(int total, QString format=\"[%1/%2]
(%3% done)...\", UpdateType t=UpdateOnPercent) ";

%feature("docstring")  gaia2::TextProgress::total "int
gaia2::TextProgress::total() const ";

%feature("docstring")  gaia2::TextProgress::setTotal "void
gaia2::TextProgress::setTotal(int total) ";

%feature("docstring")  gaia2::TextProgress::update "void
gaia2::TextProgress::update()

Updates the counter, ie: displays its state on the standard output. ";

%feature("docstring")  gaia2::TextProgress::update "void
gaia2::TextProgress::update(int value)

Updates the counter to given value, then display its state on the
standard output.

Parameters:
-----------

value:  the value of the counter. ";

%feature("docstring")  gaia2::TextProgress::setSilent "void
gaia2::TextProgress::setSilent(bool silent) ";

%feature("docstring")  gaia2::TextProgress::isSilent "bool
gaia2::TextProgress::isSilent() ";


// File: classgaia2_1_1Timer.xml
%feature("docstring") gaia2::Timer "

This class allows you to measure a certain amount of time, for
instance if you want to know how long the execution of a given
function takes.

Its use is very easy: you create the object, you start it, you stop
it, and then you ask it how much time there was between the start and
stop calls. You can start it and stop it multiple times; in this case,
the result will be the sum of each lapse of time.

C++ includes: timer.h ";

%feature("docstring")  gaia2::Timer::Timer "Timer::Timer()

The constructor of the class. ";

%feature("docstring")  gaia2::Timer::start "void Timer::start()

Starts the timer.

Total is not reset to 0. ";

%feature("docstring")  gaia2::Timer::restart "void
gaia2::Timer::restart()

Restarts the timer.

Has same effect as  reset() followed by  start() Total is reset to 0.
";

%feature("docstring")  gaia2::Timer::stop "void Timer::stop()

Stops the timer. ";

%feature("docstring")  gaia2::Timer::reset "void Timer::reset()

Reset the total to 0. ";

%feature("docstring")  gaia2::Timer::elapsed "double Timer::elapsed()
const

Returns the cumulative number of seconds elapsed between each call to
the  start()/stop() pair.

If you didn't call one of these two functions at least once or if the
timer is still running, this functions returns 0. the number of
elapsed seconds between  start() and  stop(), 0 if an error has
occured. ";


// File: classToken.xml
%feature("docstring") Token "";


// File: classgaia2_1_1TransfoChain.xml
%feature("docstring") gaia2::TransfoChain "";

%feature("docstring")  gaia2::TransfoChain::mapPoint "Point *
gaia2::TransfoChain::mapPoint(const Point *point, bool
takeOwnership=false) const ";

%feature("docstring")  gaia2::TransfoChain::mapDataSet "DataSet *
gaia2::TransfoChain::mapDataSet(const DataSet *dataset, bool
takeOwnership=false) const ";

%feature("docstring")  gaia2::TransfoChain::partialMapDataSet "DataSet * gaia2::TransfoChain::partialMapDataSet(const DataSet
*dataset, int start, int end=0) const

Only maps a part of the transformation chain, you need to specify the
indices of first and after-last transformation.

If end == 0, then all the transformations starting at index start will
be applied. ";

%feature("docstring")  gaia2::TransfoChain::retransformDataSet "DataSet * gaia2::TransfoChain::retransformDataSet(const DataSet
*dataset, int start, int end=0) const

Retransforms the dataset using the given partial history.

See:   partialMapDataSet ";

%feature("docstring")  gaia2::TransfoChain::load "void
gaia2::TransfoChain::load(const QString &filename) ";

%feature("docstring")  gaia2::TransfoChain::save "void
gaia2::TransfoChain::save(const QString &filename) const ";

%feature("docstring")  gaia2::TransfoChain::contains "bool
gaia2::TransfoChain::contains(const QString &analyzerName) const ";


// File: classgaia2_1_1Transformation.xml
%feature("docstring") gaia2::Transformation "

Class containing all info pertaining to a specific transformation, ie:
the name of the analyzer class, the input parameters, the results of
this analysis (= the applier parameters), and the name of the applier
class.

This class may also contain pre-instantiated analyzer and applier
objects.

C++ includes: transformation.h ";

%feature("docstring")  gaia2::Transformation::Transformation "gaia2::Transformation::Transformation() ";

%feature("docstring")  gaia2::Transformation::Transformation "gaia2::Transformation::Transformation(const PointLayout &l) ";

%feature("docstring")  gaia2::Transformation::Transformation "gaia2::Transformation::Transformation(const Transformation &rhs) ";

%feature("docstring")  gaia2::Transformation::~Transformation "gaia2::Transformation::~Transformation() ";

%feature("docstring")  gaia2::Transformation::analyzer "const
Analyzer * gaia2::Transformation::analyzer() const ";

%feature("docstring")  gaia2::Transformation::applier "const Applier
* gaia2::Transformation::applier() const ";

%feature("docstring")  gaia2::Transformation::applyToDataSet "DataSet
* gaia2::Transformation::applyToDataSet(const DataSet *dataset) const

Apply the current transformation with its parameters to the DataSet.
";

%feature("docstring")  gaia2::Transformation::retransformDataSet "DataSet * gaia2::Transformation::retransformDataSet(const DataSet
*dataset) const

Redo the full analysis+apply to the given dataset.

The resulting transformation that is applied will have the same
analyzer parameters, but different applier ones. ";

%feature("docstring")  gaia2::Transformation::applyToPoint "Point *
gaia2::Transformation::applyToPoint(const Point *point) const ";

%feature("docstring")  gaia2::Transformation::load "void
gaia2::Transformation::load(const QString &filename) ";

%feature("docstring")  gaia2::Transformation::save "void
gaia2::Transformation::save(const QString &filename) ";

%feature("docstring")  gaia2::Transformation::updateOldTransformation
"void gaia2::Transformation::updateOldTransformation(int version)

Updates (if necessary) the parameters for this transformation from the
specified original version to the current one. ";


// File: classgaia2std_1_1Transformation.xml
%feature("docstring") gaia2std::Transformation "";

%feature("docstring")  gaia2std::Transformation::applyToDataSet "proxy_ptr< DataSet > gaia2std::Transformation::applyToDataSet(const
DataSet *dataset) const ";

%feature("docstring")  gaia2std::Transformation::applyToPoint "proxy_ptr< Point > gaia2std::Transformation::applyToPoint(const Point
*point) const ";


// File: classgaia2_1_1UnscopedData.xml
%feature("docstring") gaia2::UnscopedData "";

%feature("docstring")  gaia2::UnscopedData::UnscopedData "gaia2::UnscopedData::UnscopedData() ";


// File: classgaia2_1_1parser_1_1Value.xml
%feature("docstring") gaia2::parser::Value "";

%feature("docstring")  gaia2::parser::Value::~Value "virtual
gaia2::parser::Value::~Value() ";

%feature("docstring")  gaia2::parser::Value::value "virtual Real
gaia2::parser::Value::value() const =0 ";

%feature("docstring")  gaia2::parser::Value::toString "virtual
QString gaia2::parser::Value::toString() const =0 ";

%feature("docstring")  gaia2::parser::Value::copy "virtual Value*
gaia2::parser::Value::copy() const =0 ";


// File: classgaia2_1_1parser_1_1ValueConstant.xml
%feature("docstring") gaia2::parser::ValueConstant "";

%feature("docstring")  gaia2::parser::ValueConstant::ValueConstant "gaia2::parser::ValueConstant::ValueConstant(Real value) ";

%feature("docstring")  gaia2::parser::ValueConstant::copy "virtual
Value* gaia2::parser::ValueConstant::copy() const ";

%feature("docstring")  gaia2::parser::ValueConstant::value "virtual
Real gaia2::parser::ValueConstant::value() const ";

%feature("docstring")  gaia2::parser::ValueConstant::toString "virtual QString gaia2::parser::ValueConstant::toString() const ";


// File: classgaia2_1_1BaseSearchSpacePool_1_1ValueIndex.xml


// File: classgaia2_1_1parser_1_1ValueVariable.xml
%feature("docstring") gaia2::parser::ValueVariable "";

%feature("docstring")  gaia2::parser::ValueVariable::ValueVariable "gaia2::parser::ValueVariable::ValueVariable(const QString &name) ";

%feature("docstring")  gaia2::parser::ValueVariable::name "QString
gaia2::parser::ValueVariable::name() const ";

%feature("docstring")  gaia2::parser::ValueVariable::copy "virtual
Value* gaia2::parser::ValueVariable::copy() const ";

%feature("docstring")  gaia2::parser::ValueVariable::value "virtual
Real gaia2::parser::ValueVariable::value() const ";

%feature("docstring")  gaia2::parser::ValueVariable::bind "void
gaia2::parser::ValueVariable::bind(const PointLayout &layout, const
Point **pointLocation) ";

%feature("docstring")  gaia2::parser::ValueVariable::toString "virtual QString gaia2::parser::ValueVariable::toString() const ";


// File: classgaia2std_1_1View.xml
%feature("docstring") gaia2std::View "";

%feature("docstring")  gaia2std::View::View "gaia2std::View::View(const DataSet *dataset) ";

%feature("docstring")  gaia2std::View::nnSearch "SearchResults
gaia2std::View::nnSearch(const Point &p, const DistanceFunction
&dfunc, int numNeighbors, const std::string &filter=\"\") ";

%feature("docstring")  gaia2std::View::nnSearch "SearchResults
gaia2std::View::nnSearch(const std::string &id, const DistanceFunction
&dfunc, int numNeighbors, const std::string &filter=\"\") ";


// File: classgaia2_1_1WeightedEuclideanDistance.xml
%feature("docstring") gaia2::WeightedEuclideanDistance "

This class computes the euclidean distance of a given Region, applying
a weight on each descriptor.

WARNING: this metric is not final, and it might change its behavior in
the future.

Parameters:
-----------

weights:  a mapping from descriptor name to its weight.

C++ includes: weightedeuclideandistance.h ";

%feature("docstring")
gaia2::WeightedEuclideanDistance::WeightedEuclideanDistance "WeightedEuclideanDistance::WeightedEuclideanDistance(const PointLayout
&layout, const ParameterMap &params) ";


// File: classgaia2_1_1WeightedPearsonDistance.xml
%feature("docstring") gaia2::WeightedPearsonDistance "

This class computes the weighted Pearson correlation between 2 points.

It is defined as $ d (X,Y) =
1-\\\\frac{\\\\sum{w_i(x_i-\\\\bar{x_w})(y_i-\\\\bar{y_w})}}{\\\\sqrt{\\\\sum{w_i(x_i-\\\\bar{x_w})^2}}\\\\sqrt{\\\\sum{w_i(y_i-\\\\bar{y_w})^2}}}
$ with weighted means $ \\\\bar{x_w} =
\\\\frac{\\\\sum{w_ix_i}}{\\\\sum{w_i}}; \\\\bar{y_w} =
\\\\frac{\\\\sum{w_iy_i}}{\\\\sum{w_i}} $ .

Parameters:
-----------

weights:  a mapping from descriptor name to its weight.

C++ includes: weightedpearsondistance.h ";

%feature("docstring")
gaia2::WeightedPearsonDistance::WeightedPearsonDistance "WeightedPearsonDistance::WeightedPearsonDistance(const PointLayout
&layout, const ParameterMap &params) ";


// File: structyy__buffer__state.xml
%feature("docstring") yy_buffer_state "";


// File: structyy__trans__info.xml
%feature("docstring") yy_trans_info "";


// File: unionYYMINORTYPE.xml


// File: structyyParser.xml
%feature("docstring") yyParser "";


// File: structyyStackEntry.xml
%feature("docstring") yyStackEntry "";


// File: namespacedoxy2swig.xml
%feature("docstring")  doxy2swig::my_open_read "def
doxy2swig.my_open_read(source) ";

%feature("docstring")  doxy2swig::my_open_write "def
doxy2swig.my_open_write(dest) ";

%feature("docstring")  doxy2swig::convert "def
doxy2swig.convert(input, output, include_function_definition=True,
quiet=False) ";

%feature("docstring")  doxy2swig::main "def doxy2swig.main() ";


// File: namespaceEigen.xml


// File: namespacegaia2.xml
%feature("docstring")  gaia2::convert::getRegionMeanVar "QPair<
Point, Point > gaia2::getRegionMeanVar(const DataSet &dataset, const
Region &region)

Gets the mean and variance of the descriptors in the given region.

As those concepts only make sense for real numbers, the string
descriptors contained in this region will be silently ignored. This
function requires all descriptors in the region to be fixed-length,
otherwise it throws an exception. ";

%feature("docstring")  gaia2::convert::getRegionMinMax "QPair< Point,
Point > gaia2::getRegionMinMax(const DataSet &dataset, const Region
&region)

Gets the mean and variance of the descriptors in the given region.

As those concepts only make sense for real numbers, the string
descriptors contained in this region will be silently ignored. This
function requires all descriptors in the region to be fixed-length,
otherwise it throws an exception. ";

%feature("docstring")  gaia2::convert::findVariableLengthDescriptors "QStringList gaia2::findVariableLengthDescriptors(const DataSet
*dataset)

For all variable-length descriptors in this DataSet, find all the ones
which actually are of different lengths, ie: those such that there
exists 2 points p1 and p2 such as len(p1.desc) != len(p2.desc).

If there is a descriptor which has len == 0 for all points, then this
descriptor is considered variable-length as well. The rationale is the
following: if they're all empty, it only makes sense if they're
actually lists of values which (by chance) happen to be all empty, but
it can in no case be a fixed-length descriptor, because that would be
completely stupid to have such an empty descriptor. ";

%feature("docstring")  gaia2::convert::registerAnalyzers "void
gaia2::registerAnalyzers()

Registers all the analyzers available inside the AnalyzerFactory.

NB: For the python docstrings to work correctly, the name with which
the analyzer is registered needs to be the exact same name as the
class name (case-sensitive). ";

%feature("docstring")  gaia2::convert::registerAppliers "void
gaia2::registerAppliers() ";

%feature("docstring")  gaia2::convert::totalVariancePercentage "Real
gaia2::totalVariancePercentage(const VectorXd &eigenValues, int
targetDimension) ";

%feature("docstring")  gaia2::convert::debugModuleDescription "const
char * gaia2::debugModuleDescription(DebuggingModule module) ";

%feature("docstring")  gaia2::convert::setDebugLevel "void
gaia2::setDebugLevel(int levels) ";

%feature("docstring")  gaia2::convert::unsetDebugLevel "void
gaia2::unsetDebugLevel(int levels) ";

%feature("docstring")  gaia2::convert::sqrt "RealDescriptor
gaia2::sqrt(const RealDescriptor &x) ";

%feature("docstring")  gaia2::convert::qtMessageHandler "static void
gaia2::qtMessageHandler(QtMsgType type, const char *msg) ";

%feature("docstring")  gaia2::convert::init "void gaia2::init()

Init function that makes sure that all the factories have correctly
registered all the types they can create.

You need to call this function once in your program, as soon as you
can. ";

%feature("docstring")  gaia2::convert::shutdown "void
gaia2::shutdown()

Shutdown function that mostly frees up resources (statically)
allocated for having better performance in gaia. ";

%feature("docstring")  gaia2::convert::GAIA_DEFINE_EXCEPTION "gaia2::GAIA_DEFINE_EXCEPTION(FixedLengthException) ";

%feature("docstring")  gaia2::convert::erfinv "double
gaia2::erfinv(double P)

Computes the inverse error function
(http://en.wikipedia.org/wiki/Error_function). ";

%feature("docstring")  gaia2::convert::norminv "double
gaia2::norminv(double P, double mu=0, double sigma=1)

Computes the inverse of the normal cdf with parameter mu and sigma. ";

%feature("docstring")  gaia2::convert::chi2inv "double
gaia2::chi2inv(double P, int v)

Computes the inverse of the chi-square cdf with v degrees of freedom.
";

%feature("docstring")  gaia2::convert::isnan "bool gaia2::isnan(T x)
";

%feature("docstring")  gaia2::convert::isinf "bool gaia2::isinf(T x)
";

%feature("docstring")  gaia2::convert::mean "T gaia2::mean(const T
*array, uint n)

Computes the mean of an array. ";

%feature("docstring")  gaia2::convert::mean "T gaia2::mean(const
std::vector< T > &v) ";

%feature("docstring")  gaia2::convert::variance "T
gaia2::variance(const T *array, uint n)

Computes the variance of an array. ";

%feature("docstring")  gaia2::convert::variance "T
gaia2::variance(const std::vector< T > &v) ";

%feature("docstring")  gaia2::convert::skewness "T
gaia2::skewness(const T *array, uint n)

Computes the skewness of an array.

It is mathematically defined as: skewness = mu_3 / mu_2**{3/2}, where
mu_i is the i_th central moment. ";

%feature("docstring")  gaia2::convert::skewness "T
gaia2::skewness(const std::vector< T > &v) ";

%feature("docstring")  gaia2::convert::kurtosis "T
gaia2::kurtosis(const T *array, uint n)

Computes the kurtosis of an array.

It is mathematically defined as: kurtosis = mu_4 / mu_2**{2}, where
mu_i is the i_th central moment. ";

%feature("docstring")  gaia2::convert::kurtosis "T
gaia2::kurtosis(const std::vector< T > &v) ";

%feature("docstring")  gaia2::convert::sort "void
gaia2::sort(RandomAccessIterator first, RandomAccessIterator last) ";

%feature("docstring")  gaia2::convert::sort "void
gaia2::sort(RandomAccessIterator first, RandomAccessIterator last,
StrictWeakOrdering comp) ";

%feature("docstring")  gaia2::convert::sort "void
gaia2::sort(Container &container) ";

%feature("docstring")  gaia2::convert::hist "void gaia2::hist(const T
*array, uint n, int *n_array, T *x_array, uint n_bins)

Given a set of values, computes the associated histogram.

This method is designed to work the same way as in MatLab/Octave. It
is based on the algorithms used in Octave rather than Matlab. The
result structures (n_arr, x_arr) have to be allocated before calling
this function.

Parameters:
-----------

array:  the input array, containing the data values

n:  the number of elements of this array

n_array:  the array where the distribution will be written

x_array:  the array that will contain the centers of each bin

n_bins:  the number of desired bins for the distribution ";

%feature("docstring")  gaia2::convert::sgn "T gaia2::sgn(T x) ";

%feature("docstring")  gaia2::convert::intpow "T gaia2::intpow(T x,
int n)

Computes the integral power n of x. ";

%feature("docstring")  gaia2::convert::binarySearch "int
gaia2::binarySearch(const Container< T > &v, T value)

Iterative function for binary search (more efficient than recursive
one).

If n is the size of the vector, the returned value will be between 0
and n (inclusive) and correspond to the interval between numbers. Ex:
r = 0 means it is lower than the lowest value in v, r = n means it is
greater than the greatest value in v. ";

%feature("docstring")  gaia2::convert::sqrt "T gaia2::sqrt(const T
&x) ";

%feature("docstring")  gaia2::convert::clip "Real gaia2::clip(Real
value, Real minv, Real maxv)

Returns the given value clipped inside the specified interval. ";

%feature("docstring")  gaia2::convert::covarianceMatrix_tmpl "Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>
gaia2::covarianceMatrix_tmpl(const PointArray &v, const Region
&region) ";

%feature("docstring")  gaia2::convert::covarianceMatrixSinglePrecision
"Eigen::MatrixXf gaia2::covarianceMatrixSinglePrecision(const
PointArray &v, const Region &region) ";

%feature("docstring")  gaia2::convert::covarianceMatrix "Eigen::MatrixXd gaia2::covarianceMatrix(const PointArray &v, const
Region &region)

Computes the covariance matrix given a set of points.

Parameters:
-----------

v:  the set of points for which to compute the covariance matrix.

region:  the Region containing the descriptors to be included in the
covariance matrix computation. ";

%feature("docstring")  gaia2::convert::sortEigenVectors "void
gaia2::sortEigenVectors(Eigen::Matrix< T, Eigen::Dynamic, 1 > &values,
Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > &vectors, Compare
cmp)

Sort the eigen vectors and their corresponding values according to the
given compare function. ";

%feature("docstring")  gaia2::convert::sortEigenVectors "void
gaia2::sortEigenVectors(Eigen::Matrix< T, Eigen::Dynamic, 1 > &values,
Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > &vectors)

Sort the eigen vectors and their corresponding values in ascending
order. ";

%feature("docstring")  gaia2::convert::registerMetrics "void
gaia2::registerMetrics()

Registers all the metrics available inside the MetricFactory.

NB: For the python docstrings to work correctly, the name with which
the distance is registered needs to be the exact same name as the
class name, with the trailing \"Distance\" removed (case-sensitive).
";

%feature("docstring")  gaia2::convert::registerFrozenMetrics "void
gaia2::registerFrozenMetrics() ";

%feature("docstring")  gaia2::convert::traceMatrixProduct "Real
gaia2::traceMatrixProduct(const Real *m1, const Real *m2, int size)

Compute the trace of the matrix product of m1 and m2.

Both matrices need to be squared of dimension size. ";

%feature("docstring")  gaia2::convert::traceMatrixProductMean "Real
gaia2::traceMatrixProductMean(const Real *m1, const Real *m2, const
Real *v1, const Real *v2, int size)

Compute the trace of the product of 2 matrices and vectors using the
formula: trace((m1+m2)*(v1-v2)*(v1-v2)')

m1 and m2 are square matrices of dimension size, v1 and v2 are vectors
of dimension size. ";

%feature("docstring")  gaia2::convert::toParameterMap "ParameterMap
gaia2::toParameterMap(const yaml::Node &node) ";

%feature("docstring")  gaia2::convert::isAlwaysTrue "bool
gaia2::isAlwaysTrue(parser::Predicate *pred)

Returns true if the given predicate is always true (ie: it is a
boolean constant which value is True). ";

%feature("docstring")  gaia2::convert::isAlwaysFalse "bool
gaia2::isAlwaysFalse(parser::Predicate *pred)

Returns true if the given predicate is always false (ie: it is a
boolean constant which value is False). ";

%feature("docstring")  gaia2::convert::typeToString "QString
gaia2::typeToString(DescriptorType type) ";

%feature("docstring")  gaia2::convert::lengthTypeToString "QString
gaia2::lengthTypeToString(DescriptorLengthType ltype) ";

%feature("docstring")  gaia2::convert::toYaml "yaml::Node
gaia2::toYaml(const SearchResults &results) ";

%feature("docstring")  gaia2::convert::pointerOrderCompare "bool
gaia2::pointerOrderCompare(const SearchPointType &p1, const
SearchPointType &p2) ";

%feature("docstring")  gaia2::convert::pointerOrderCompare "bool
gaia2::pointerOrderCompare(const SearchPoint &p1, const SearchPoint
&p2) ";

%feature("docstring")  gaia2::convert::pointerOrderCompare "bool
gaia2::pointerOrderCompare(const FrozenSearchPoint &p1, const
FrozenSearchPoint &p2) ";

%feature("docstring")  gaia2::convert::copySearchPoints "void
gaia2::copySearchPoints(BaseSearchSpace< SearchPointType, DataSetType
> *dest, const BaseSearchSpace< SearchPointType, DataSetType > *src,
int start, int end, int destStart) ";

%feature("docstring")  gaia2::convert::copySearchPoints "void
gaia2::copySearchPoints(BaseSearchSpace< SearchPointType, DataSetType
> *dest, const BaseSearchSpace< SearchPointType, DataSetType > *src,
int start, int end) ";

%feature("docstring")  gaia2::convert::copySearchPoints "void
gaia2::copySearchPoints(BaseSearchSpace< SearchPointType, DataSetType
> *dest, const BaseSearchSpace< SearchPointType, DataSetType > *src)
";

%feature("docstring")  gaia2::convert::refPoint "const Point*
gaia2::refPoint(const SearchPointType &p, const DataSet *refDataSet)
";

%feature("docstring")  gaia2::convert::refPoint "const Point*
gaia2::refPoint(const SearchPoint &p, const DataSet *refDataSet) ";

%feature("docstring")  gaia2::convert::refPoint "const Point*
gaia2::refPoint(const FrozenSearchPoint &p, const DataSet *refDataSet)
";

%feature("docstring")  gaia2::convert::refPoint "const Point*
gaia2::refPoint(const SearchPointType &p, const BaseSearchSpace<
SearchPointType, DataSetType > &sspace) ";

%feature("docstring")  gaia2::convert::refPoint "const Point*
gaia2::refPoint(const SearchPoint &p, const SearchSpace &sspace) ";

%feature("docstring")  gaia2::convert::refPoint "const Point*
gaia2::refPoint(const FrozenSearchPoint &p, const FrozenSearchSpace
&sspace) ";

%feature("docstring")  gaia2::convert::binarySearch "int
gaia2::binarySearch(const SearchSpaceType &sspace, int idx, Real
value, int start, int end) ";

%feature("docstring")  gaia2::convert::RTIME "QString
gaia2::RTIME(double t) ";

%feature("docstring")  gaia2::convert::normalSearch "void
gaia2::normalSearch(DataSet &dataset, const DistanceFunction *dist,
const Point &query, int N) ";

%feature("docstring")  gaia2::convert::frozenSearch "void
gaia2::frozenSearch(const DataSet &dataset, const float
*frozenDataSet, int dimension, const float *queryPoint, int N, const
DistanceFunction &dist) ";

%feature("docstring")  gaia2::convert::deepFreeze "void
gaia2::deepFreeze(FrozenDataSet &frozenDS, const FrozenDistance &dist,
const Eigen::RowVectorXf &queryPoint, int N) ";

%feature("docstring")  gaia2::convert::deepFoldedFreeze "void
gaia2::deepFoldedFreeze(FrozenDataSet &frozenDS, const FrozenDistance
&dist, const Eigen::RowVectorXf &queryPoint, int N) ";

%feature("docstring")  gaia2::convert::deepMultiFreeze "void
gaia2::deepMultiFreeze(FrozenDataSet &frozenDS, const QList<
FrozenDistance *> &dists, const QList< Eigen::RowVectorXf > queries,
int N) ";

%feature("docstring")  gaia2::convert::transform "DataSet *
gaia2::transform(DataSet *ds, const QString &name, const ParameterMap
&params) ";

%feature("docstring")  gaia2::convert::applyTransfoChain "DataSet *
gaia2::applyTransfoChain(DataSet *ds, const QString &yamlTransfoChain)
";

%feature("docstring")  gaia2::convert::applyTransfoChain "DataSet*
gaia2::applyTransfoChain(DataSet *ds, const std::string
&yamlTransfoChain) ";

%feature("docstring")  gaia2::convert::parseDimension "QPair<
QString, int > gaia2::parseDimension(const QString &dimension)

Parse a string representing a descriptor + optional dimension as a
QPair<QString, int>.

In case the dimension is not specified, 0 will be returned. ";

%feature("docstring")  gaia2::convert::checkIsomorphDataSets "void
gaia2::checkIsomorphDataSets(const DataSet *dataset1, const DataSet
*dataset2, bool checkOriginalLayout=true)

Returns normally if both dataset are isomorph, meaning that they can
be linked together, merged together, etc...

In particular, they need to have exactly the same points and the same
original layout (we don't care about the transformation history). In
the case they aren't, throw an exception with an error message. In
case any of the 2 datasets is the null pointer, this doesn't throw an
exception. ";

%feature("docstring")  gaia2::convert::selectDescriptors "QStringList
gaia2::selectDescriptors(const PointLayout &layout, DescriptorType
type=UndefinedType, const QStringList &select=QStringList(\"*\"),
const QStringList &exclude=QStringList(), bool failOnUnmatched=true)

This function returns the region of descriptors from a layout that
match the patterns given by the select argument but that do not match
the ones given by the exclude argument. ";

%feature("docstring")  gaia2::convert::createIndexMappings "void
gaia2::createIndexMappings(const PointLayout &oldLayout, PointLayout
&newLayout, const Region &region, IndexMap &realMap, IndexMap
&stringMap, IndexMap &enumMap)

Creates mappings from indices in the NEW layout to indices in the OLD
layout. ";

%feature("docstring")  gaia2::convert::transferPointData "void
gaia2::transferPointData(const Point *p, Point *result, const IndexMap
&realMap, const IndexMap &stringMap, const IndexMap &enumMap)

Given a set of mappings, transfers the data from a Point to a new one.
";

%feature("docstring")  gaia2::convert::mapPoint "Point *
gaia2::mapPoint(const Point *p, const PointLayout &newLayout, const
IndexMap &realMap, const IndexMap &stringMap, const IndexMap &enumMap)

Creates a new Point that is a mapping of the given one, copying only
those descriptors that are in newLayout.

The Index maps should be precalculated with the createIndexMappings
function. ";

%feature("docstring")  gaia2::convert::mergeDescriptors "RealDescriptor gaia2::mergeDescriptors(const Point *p, int nseg, const
Region &region)

Merges the descriptors contained into the given region into a single
one. ";

%feature("docstring")  gaia2::convert::mergeLayouts "PointLayout
gaia2::mergeLayouts(const PointLayout &layout1, const PointLayout
&layout2)

Merges two PointLayouts together, provided that they don't overlap. ";

%feature("docstring")  gaia2::convert::mergePoints "Point *
gaia2::mergePoints(const Point *ds1, const Point *ds2)

Merges two points together, provided that their layout don't overlap,
and return the resulting point. ";

%feature("docstring")  gaia2::convert::mergeDataSets "DataSet *
gaia2::mergeDataSets(const DataSet *ds1, const DataSet *ds2)

Merges two datasets together, provided that their layout don't
overlap, and return the resulting dataset. ";

%feature("docstring")  gaia2::convert::mapRegionIndicesToNames "QMap<
int, QString > gaia2::mapRegionIndicesToNames(const Region &region,
const DataSet *dataset)

Given a DataSet (for reference) and a Region, returns the mapping
between the region indices and the names of the corresponding
descriptors.

This function will throw if the Region is not composed of a single
type of descriptors. ";

%feature("docstring")  gaia2::convert::dimensionParticipation "QList<
QPair< double, QString > > gaia2::dimensionParticipation(const
Eigen::VectorXd &values, const QMap< int, QString > &dnames)

Given a list of values and a map from their indices to their
respective names, returns a list of pairs containing the participation
of each original dimension in the target space, expressed as a
percentage of the total (meaning the sum of the participation is equal
to 100). ";

%feature("docstring")  gaia2::convert::dimensionBoost "QList< QPair<
double, QString > > gaia2::dimensionBoost(const Eigen::MatrixXf
&featureVector, const Region &region, const DataSet *dataset)

Dimension boost after applying the rotation/scaling defined by the
given base vectors.

Useful in particular in RCA (where the vectors are not an orthonormal
base). ";

%feature("docstring")  gaia2::convert::formatDimensionParticipation "QString gaia2::formatDimensionParticipation(const QList< QPair<
double, QString > > &participation, bool ratio=false)

Formats the results of the dimensionParticipation function into a
nicely printable string.

If ratio == true, prints \"x 1.23\" instead of \"1.23%\". ";

%feature("docstring")  gaia2::convert::dimensionListToMapping "QMap<
QString, QList< int > > gaia2::dimensionListToMapping(const
QStringList &dims)

Converts a list of dimensions names in the format descriptorName[dim]
to a map of descriptor name to list of integers representing the
dimensions selected for each one. ";

%feature("docstring")  gaia2::convert::gaiaVersion "int
gaia2::gaiaVersion(const QDataStream &stream)

Returns the Gaia version of this stream. ";

%feature("docstring")  gaia2::convert::setDataStreamVersion "void
gaia2::setDataStreamVersion(QDataStream &out)

Sets the version of the QDataStream to the current version of Gaia.

This function must be called before serializing anything in the
stream. ";

%feature("docstring")  gaia2::convert::checkValidDataStream "void
gaia2::checkValidDataStream(QDataStream &in)

Checks that the input QDataStream is a valid one (ie: supported by our
version of Gaia) and sets its corresponding type so that it's ready to
be read. ";

%feature("docstring")  gaia2::convert::memoryUsage "int
gaia2::memoryUsage(QString mtype=\"VmSize\")

Returns the memory currently used by this process, in KBs.

mtype can be either one of: VmSize: total virtual memory

VmRSS: resident segment size

NB: Only works on linux at the moment, will return 0 on other
platforms. ";

%feature("docstring")  gaia2::convert::mergeName "QString
gaia2::mergeName(const QStringList &name)

Utility function that merges a sequence of node names into a single
fully-qualified name. ";

%feature("docstring")  gaia2::convert::splitName "QStringList
gaia2::splitName(const QString &name, bool includeRootNode=false)

Splits a fully-qualified name into its consecutive node names. ";


// File: namespacegaia2_1_1convert.xml
%feature("docstring")  gaia2::convert::toString "std::string
gaia2::convert::toString(const QString &qstr)

Converts a QString into a std::string. ";

%feature("docstring")  gaia2::convert::QStringList_to_VectorString "std::vector< std::string >
gaia2::convert::QStringList_to_VectorString(const QList< QString >
&qlist)

Converts a QStringList into a std::vector<std::string>. ";

%feature("docstring")  gaia2::convert::VectorString_to_QStringList "QStringList gaia2::convert::VectorString_to_QStringList(const
std::vector< std::string > &svector)

Converts a std::vector<std::string> into a QStringList. ";

%feature("docstring")  gaia2::convert::RealDescriptor_to_VectorFloat "std::vector< float >
gaia2::convert::RealDescriptor_to_VectorFloat(const RealDescriptor
&rdesc)

Converts a RealDescriptor into a std::vector<float>. ";

%feature("docstring")  gaia2::convert::VectorFloat_to_RealDescriptor "RealDescriptor gaia2::convert::VectorFloat_to_RealDescriptor(const
std::vector< float > &fvector)

Converts a std::vector<float> into a RealDescriptor. ";

%feature("docstring")
gaia2::convert::StringDescriptor_to_VectorString "std::vector<
std::string > gaia2::convert::StringDescriptor_to_VectorString(const
StringDescriptor &sdesc)

Converts a StringDescriptor into a std::vector<std::string>. ";

%feature("docstring")
gaia2::convert::VectorString_to_StringDescriptor "StringDescriptor
gaia2::convert::VectorString_to_StringDescriptor(const std::vector<
std::string > &svector)

Converts a std::vector<std::string> into a StringDescriptor. ";

%feature("docstring")  gaia2::convert::SearchResults_to_StlTypes "std::vector< std::pair< std::string, float > >
gaia2::convert::SearchResults_to_StlTypes(const SearchResults
&results)

Converts a gaia2::SearchResults to its STL equivalent
(std::vector<std::pair<std::string, float> > >). ";

%feature("docstring")  gaia2::convert::VectorPair_to_QPointList "QList< QPointF > gaia2::convert::VectorPair_to_QPointList(const
std::vector< std::pair< float, float > > &pvector)

Converts a std::vector<std::pair<float, float> > to a QList<PointF>.
";

%feature("docstring")  gaia2::convert::QPointList_to_VectorPair "std::vector< std::pair< float, float > >
gaia2::convert::QPointList_to_VectorPair(const QList< QPointF >
&plist)

Converts a QList<PointF> to a std::vector<std::pair<float, float> >.
";

%feature("docstring")  gaia2::convert::Vector_to_QList "QList<T>
gaia2::convert::Vector_to_QList(const std::vector< T > &v)

Converts a std::vector of something into a QList of the same objects.
";


// File: namespacegaia2_1_1filter.xml


// File: namespacegaia2_1_1parser.xml
%feature("docstring")  gaia2::parser::yy_reduce "void
gaia2::parser::yy_reduce(yyParser *, int) ";


// File: namespacegaia2_1_1yaml.xml
%feature("docstring")  gaia2::yaml::errorMessage "QString
gaia2::yaml::errorMessage(const yaml_parser_t &parser) ";

%feature("docstring")  gaia2::yaml::loadFromString "Node
gaia2::yaml::loadFromString(const std::string &str, bool
markQuotedScalars=false)

Global function for the namespace that reads a string and returns a
Node instance that represents the data.

You can pass it a parameter, markQuotedScalars, which if it is true,
will add a char '\\\\0' in front of all the scalars that have been
quoted. ";

%feature("docstring")  gaia2::yaml::loadFromFile "Node
gaia2::yaml::loadFromFile(const QString &filename, bool
markQuotedScalars=false)

This function loads a yaml file and returns the corresponding yaml
node.

You can pass it a parameter, markQuotedScalars, which if it is true,
will add a char '\\\\0' in front of all the scalars that have been
quoted. ";

%feature("docstring")  gaia2::yaml::load "Node
gaia2::yaml::load(const QString &str) ";

%feature("docstring")  gaia2::yaml::load "Node
gaia2::yaml::load(const char *str, uint size, bool markQuotedScalars)
";

%feature("docstring")  gaia2::yaml::write_handler "int
gaia2::yaml::write_handler(void *ext, yaml_char_t *buffer, size_t
size) ";

%feature("docstring")  gaia2::yaml::emitterErrorMessage "QString
gaia2::yaml::emitterErrorMessage(yaml_emitter_t *emitter, const char
*location) ";

%feature("docstring")  gaia2::yaml::dumpObject "void
gaia2::yaml::dumpObject(yaml_emitter_t *emitter, const yaml::Node
&node) ";

%feature("docstring")  gaia2::yaml::dump "QByteArray
gaia2::yaml::dump(const yaml::Node &node) ";

%feature("docstring")  gaia2::yaml::GAIA_DEFINE_EXCEPTION "gaia2::yaml::GAIA_DEFINE_EXCEPTION(YamlException)

YamlException that offers the the same functionality as GaiaException
but is a different class to allow specific filtering on it. ";


// File: namespacegaia2std.xml
%feature("docstring")  gaia2std::PROXY_DESTRUCTOR "gaia2std::PROXY_DESTRUCTOR(Analyzer) ";

%feature("docstring")  gaia2std::PROXY_DESTRUCTOR "gaia2std::PROXY_DESTRUCTOR(DataSet) ";

%feature("docstring")  gaia2std::PROXY_DESTRUCTOR "gaia2std::PROXY_DESTRUCTOR(DistanceFunction) ";

%feature("docstring")  gaia2std::init "void gaia2std::init()

Call this function before doing anything with gaia.

If you forget it, you will not be able to instantiate analyzers,
distance functions, etc... ";

%feature("docstring")  gaia2std::verbose "bool gaia2std::verbose()

Returns whether the global verbosity flag is set to true or false. ";

%feature("docstring")  gaia2std::setVerbose "void
gaia2std::setVerbose(bool verbosity)

Set the global verbosity flag. ";

%feature("docstring")  gaia2std::setDebugLevel "void
gaia2std::setDebugLevel(int levels) ";

%feature("docstring")  gaia2std::unsetDebugLevel "void
gaia2std::unsetDebugLevel(int levels) ";

%feature("docstring")  gaia2std::PROXY_DESTRUCTOR "gaia2std::PROXY_DESTRUCTOR(ParameterMap) ";

%feature("docstring")  gaia2std::PROXY_DESTRUCTOR "gaia2std::PROXY_DESTRUCTOR(Transformation) ";

%feature("docstring")  gaia2std::PROXY_DESTRUCTOR "gaia2std::PROXY_DESTRUCTOR(View) ";


// File: namespacegaiafusion.xml
%feature("docstring")  gaiafusion::createOptionParser "def
gaiafusion.createOptionParser() ";

%feature("docstring")  gaiafusion::usage "def gaiafusion.usage()

gaiafusion --yamllist=mylist.yaml --output=data/itunes.db gaiafusion
-d essentia_1.0.6/sigfiles -o amazon.db gaiafusion --chunksize=10000
... ";

%feature("docstring")  gaiafusion::fusion "def gaiafusion.fusion() ";


// File: namespaceregenerate__docstring.xml
%feature("docstring")  regenerate_docstring::currentPath "def
regenerate_docstring.currentPath()

Returns the path in which the calling file is located. ";

%feature("docstring")  regenerate_docstring::getDoc "def
regenerate_docstring.getDoc(className) ";


// File: namespacestd.xml


// File: addfield_8cpp.xml


// File: addfield_8h.xml


// File: addfieldapplier_8cpp.xml
%feature("docstring")  checkNSegments "void checkNSegments(const
Point *p) ";


// File: addfieldapplier_8h.xml


// File: algoutils_8cpp.xml


// File: algoutils_8h.xml


// File: algorithms_2analyzer_8cpp.xml


// File: stlfacade_2analyzer_8cpp.xml


// File: algorithms_2analyzer_8h.xml


// File: stlfacade_2analyzer_8h.xml


// File: applier_8cpp.xml


// File: applier_8h.xml


// File: center_8cpp.xml


// File: center_8h.xml


// File: cleaner_8cpp.xml


// File: cleaner_8h.xml


// File: distribute_8cpp.xml


// File: distribute_8h.xml


// File: dotproduct_8cpp.xml


// File: dotproduct_8h.xml


// File: enumerate_8cpp.xml


// File: enumerate_8h.xml


// File: enumerateapplier_8cpp.xml


// File: enumerateapplier_8h.xml


// File: extract_8cpp.xml


// File: extract_8h.xml


// File: extractapplier_8cpp.xml


// File: extractapplier_8h.xml


// File: fixlength_8cpp.xml


// File: fixlength_8h.xml


// File: fixlengthapplier_8cpp.xml


// File: fixlengthapplier_8h.xml


// File: gaussianize_8cpp.xml


// File: gaussianize_8h.xml


// File: inverseprojection_8cpp.xml


// File: inverseprojection_8h.xml


// File: matmult_8cpp.xml
%feature("docstring")  Eigen::regionFromNames "Region
regionFromNames(const PointLayout &layout, const QStringList
&descriptorNames) ";

%feature("docstring")  Eigen::matrixMultiply "void
matrixMultiply(const RealDescriptor &x, const MatrixXf &matrix,
RealDescriptor &result) ";


// File: matmult_8h.xml


// File: merge_8cpp.xml


// File: merge_8h.xml


// File: mergeregion_8cpp.xml


// File: mergeregion_8h.xml


// File: mergeregionapplier_8cpp.xml


// File: mergeregionapplier_8h.xml


// File: normalize_8cpp.xml


// File: normalize_8h.xml


// File: pca_8cpp.xml


// File: pca_8h.xml


// File: rca_8cpp.xml


// File: rca_8h.xml


// File: remove_8cpp.xml


// File: remove_8h.xml


// File: removedesc_8cpp.xml


// File: removedesc_8h.xml


// File: removevl_8cpp.xml


// File: removevl_8h.xml


// File: rename_8cpp.xml


// File: rename_8h.xml


// File: renameapplier_8cpp.xml


// File: renameapplier_8h.xml


// File: select_8cpp.xml


// File: select_8h.xml


// File: selectdesc_8cpp.xml


// File: selectdesc_8h.xml


// File: svmpredict_8cpp.xml


// File: svmpredict_8h.xml


// File: svmtrain_8cpp.xml


// File: svmtrain_8h.xml


// File: baseexception_8h.xml


// File: convert_8cpp.xml


// File: convert_8h.xml


// File: dataset_8cpp.xml
%feature("docstring")  nameLessThan "bool nameLessThan(const QString
&s1, const QString &s2) ";

%feature("docstring")  pidLessThan "bool pidLessThan(const Point *p1,
const Point *p2) ";


// File: stlfacade_2dataset_8cpp.xml


// File: dataset_8h.xml


// File: stlfacade_2dataset_8h.xml


// File: debugging_8cpp.xml


// File: debugging_8h.xml


// File: descriptor_8h.xml


// File: descriptortree_8cpp.xml
%feature("docstring")  unconstifyList "QList<T*> unconstifyList(const
QList< const T *> &clist) ";

%feature("docstring")  cmpNodes "bool cmpNodes(const DescriptorTree
*n1, const DescriptorTree *n2) ";


// File: descriptortree_8h.xml


// File: algorithms_8h.xml


// File: api__overview_8h.xml


// File: dealing__with__vld_8h.xml


// File: doxy2swig_8py.xml


// File: filtering_8h.xml


// File: fingerprint__tutorial_8h.xml


// File: mainpage_8h.xml


// File: metrics_8h.xml


// File: regenerate__docstring_8py.xml


// File: tutorial_8h.xml


// File: factory_8h.xml


// File: frozendataset_8cpp.xml


// File: frozendataset_8h.xml


// File: gaia_8cpp.xml


// File: gaia_8h.xml
%feature("docstring")  gaia2::ArraySizeHelper "char(&
ArraySizeHelper(T(&array)[N]))[N] ";


// File: gaiaexception_8h.xml


// File: gaiamath_8cpp.xml


// File: gaiamath_8h.xml


// File: gvarlengtharray_8h.xml


// File: linalg_8cpp.xml


// File: linalg_8h.xml


// File: bpfdistance_8cpp.xml
%feature("docstring")  compareByX "bool compareByX(const QPointF &p1,
const QPointF &p2) ";


// File: bpfdistance_8h.xml


// File: cosineangledistance_8cpp.xml


// File: cosineangledistance_8h.xml


// File: cosinesimilarity_8cpp.xml


// File: cosinesimilarity_8h.xml


// File: crosssegmentdistance_8cpp.xml


// File: crosssegmentdistance_8h.xml


// File: metrics_2distancefunction_8cpp.xml


// File: stlfacade_2distancefunction_8cpp.xml


// File: metrics_2distancefunction_8h.xml


// File: stlfacade_2distancefunction_8h.xml


// File: euclideandistance_8cpp.xml


// File: euclideandistance_8h.xml


// File: exponentialcompressdistance_8cpp.xml


// File: exponentialcompressdistance_8h.xml


// File: forceidentitydistance_8cpp.xml


// File: forceidentitydistance_8h.xml


// File: frozencosineangledistance_8h.xml


// File: frozendistance_8cpp.xml


// File: frozendistance_8h.xml


// File: frozeneuclideandistance_8h.xml


// File: frozenexponentialcompressdistance_8h.xml


// File: frozenlinearcombinationdistance_8h.xml


// File: keydistance_8cpp.xml


// File: keydistance_8h.xml


// File: kullbackleiblerdistance_8cpp.xml
%feature("docstring")  adjustDistance "Real adjustDistance(Real dist)

this function could be used to adjust the distance, using for instance
an exponential scaling (eg: dist = e^(a*dist+b)) ";


// File: kullbackleiblerdistance_8h.xml


// File: layoutawarefactory_8h.xml


// File: linearcombinationdistance_8cpp.xml


// File: linearcombinationdistance_8h.xml


// File: manhattandistance_8cpp.xml


// File: manhattandistance_8h.xml


// File: minkowskidistance_8cpp.xml


// File: minkowskidistance_8h.xml


// File: nulldistance_8h.xml


// File: randomdistance_8cpp.xml


// File: randomdistance_8h.xml


// File: resistoraveragedistance_8cpp.xml
%feature("docstring")  adjustDistance "Real adjustDistance(Real dist)

this function could be used to adjust the distance, using for instance
an exponential scaling (eg: dist = e^(a*dist+b)) ";

%feature("docstring")  kullbackLeiblerDivergence "Real
kullbackLeiblerDivergence(const Real *m1, const Real *m2, const Real
*cov1, const Real *cov2, const Real *icov1, const Real *icov2, Real
covdet1, Real covdet2, int size) ";


// File: resistoraveragedistance_8h.xml


// File: rhythmdistance_8cpp.xml


// File: rhythmdistance_8h.xml


// File: semanticclassifierdistance_8cpp.xml


// File: semanticclassifierdistance_8h.xml


// File: summedsegmentdistance_8cpp.xml


// File: summedsegmentdistance_8h.xml


// File: weightedeuclideandistance_8cpp.xml


// File: weightedeuclideandistance_8h.xml


// File: weightedpearsondistance_8cpp.xml


// File: weightedpearsondistance_8h.xml


// File: osdetect_8h.xml


// File: parameter_8cpp.xml


// File: stlfacade_2parameter_8cpp.xml


// File: parameter_8h.xml


// File: stlfacade_2parameter_8h.xml


// File: filter_8cpp.xml
%feature("docstring")  gaia2::yylex "int yylex(void) ";

%feature("docstring")  gaia2::yy_scan_string "YY_BUFFER_STATE
yy_scan_string(const char *) ";

%feature("docstring")  gaia2::yy_delete_buffer "void
yy_delete_buffer(YY_BUFFER_STATE)

Destroy the buffer.

Parameters:
-----------

b:  a buffer created with yy_create_buffer() ";


// File: filter_8h.xml


// File: filterwrapper_8cpp.xml


// File: filterwrapper_8h.xml


// File: grammar_8c.xml
%feature("docstring")  ParseTrace "void ParseTrace(FILE *TraceFILE,
char *zTracePrompt) ";

%feature("docstring")  ParseAlloc "void* ParseAlloc(void
*(*mallocProc)(size_t)) ";

%feature("docstring")  yy_destructor "static void
yy_destructor(yyParser *yypParser, YYCODETYPE yymajor, YYMINORTYPE
*yypminor) ";

%feature("docstring")  yy_pop_parser_stack "static int
yy_pop_parser_stack(yyParser *pParser) ";

%feature("docstring")  ParseFree "void ParseFree(void *p,
void(*freeProc)(void *)) ";

%feature("docstring")  yy_find_shift_action "static int
yy_find_shift_action(yyParser *pParser, YYCODETYPE iLookAhead) ";

%feature("docstring")  yy_find_reduce_action "static int
yy_find_reduce_action(int stateno, YYCODETYPE iLookAhead) ";

%feature("docstring")  yyStackOverflow "static void
yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor) ";

%feature("docstring")  yy_shift "static void yy_shift(yyParser
*yypParser, int yyNewState, int yyMajor, YYMINORTYPE *yypMinor) ";

%feature("docstring")  yy_accept "static void yy_accept(yyParser *)
";

%feature("docstring")  yy_reduce "static void yy_reduce(yyParser
*yypParser, int yyruleno) ";

%feature("docstring")  yy_parse_failed "static void
yy_parse_failed(yyParser *yypParser) ";

%feature("docstring")  yy_syntax_error "static void
yy_syntax_error(yyParser *yypParser, int yymajor, YYMINORTYPE yyminor)
";

%feature("docstring")  Parse "void Parse(void *yyp, int yymajor,
ParseTOKENTYPE yyminor ParseARG_PDECL) ";


// File: grammar_8h.xml


// File: lex_8yy_8cpp.xml
%feature("docstring")  yyrestart "void yyrestart(FILE *input_file)

Immediately switch to a different input stream.

Parameters:
-----------

input_file:  A readable stream.

This function does not reset the start condition to INITIAL . ";

%feature("docstring")  yy_switch_to_buffer "void
yy_switch_to_buffer(YY_BUFFER_STATE new_buffer)

Switch to a different input buffer.

Parameters:
-----------

new_buffer:  The new input buffer. ";

%feature("docstring")  yy_create_buffer "YY_BUFFER_STATE
yy_create_buffer(FILE *file, int size)

Allocate and initialize an input buffer state.

Parameters:
-----------

file:  A readable stream.

size:  The character buffer size in bytes. When in doubt, use
YY_BUF_SIZE.

the allocated buffer state. ";

%feature("docstring")  yy_delete_buffer "void
yy_delete_buffer(YY_BUFFER_STATE b)

Destroy the buffer.

Parameters:
-----------

b:  a buffer created with yy_create_buffer() ";

%feature("docstring")  yy_flush_buffer "void
yy_flush_buffer(YY_BUFFER_STATE b)

Discard all buffered characters.

On the next scan, YY_INPUT will be called.

Parameters:
-----------

b:  the buffer state to be flushed, usually YY_CURRENT_BUFFER. ";

%feature("docstring")  yypush_buffer_state "void
yypush_buffer_state(YY_BUFFER_STATE new_buffer)

Pushes the new state onto the stack.

The new state becomes the current state. This function will allocate
the stack if necessary.

Parameters:
-----------

new_buffer:  The new state. ";

%feature("docstring")  yypop_buffer_state "void
yypop_buffer_state(void)

Removes and deletes the top of the stack, if present.

The next element becomes the new top. ";

%feature("docstring")  yyensure_buffer_stack "static void
yyensure_buffer_stack(void) ";

%feature("docstring")  yy_load_buffer_state "static void
yy_load_buffer_state(void) ";

%feature("docstring")  yy_init_buffer "static void
yy_init_buffer(YY_BUFFER_STATE b, FILE *file) ";

%feature("docstring")  yy_scan_buffer "YY_BUFFER_STATE
yy_scan_buffer(char *base, yy_size_t size)

Setup the input buffer state to scan directly from a user-specified
character buffer.

Parameters:
-----------

base:  the character buffer

size:  the size in bytes of the character buffer

the newly allocated buffer state object. ";

%feature("docstring")  yy_scan_string "YY_BUFFER_STATE
yy_scan_string(yyconst char *yystr)

Setup the input buffer state to scan a string.

The next call to yylex() will scan from a copy of str.

Parameters:
-----------

yystr:  a NUL-terminated string to scan

the newly allocated buffer state object.

If you want to scan bytes that may contain NUL values, then use
yy_scan_bytes() instead. ";

%feature("docstring")  yy_scan_bytes "YY_BUFFER_STATE
yy_scan_bytes(yyconst char *yybytes, yy_size_t _yybytes_len)

Setup the input buffer state to scan the given bytes.

The next call to yylex() will scan from a copy of bytes.

Parameters:
-----------

bytes:  the byte buffer to scan

len:  the number of bytes in the buffer pointed to by bytes.

the newly allocated buffer state object. ";

%feature("docstring")  yyalloc "void * yyalloc(yy_size_t) ";

%feature("docstring")  yyrealloc "void * yyrealloc(void *, yy_size_t)
";

%feature("docstring")  yyfree "void yyfree(void *) ";

%feature("docstring")  yy_get_previous_state "static yy_state_type
yy_get_previous_state(void) ";

%feature("docstring")  yy_try_NUL_trans "static yy_state_type
yy_try_NUL_trans(yy_state_type current_state) ";

%feature("docstring")  yy_get_next_buffer "static int
yy_get_next_buffer(void) ";

%feature("docstring")  yy_fatal_error "static void
yy_fatal_error(yyconst char msg[]) ";

%feature("docstring")  yyunput "static void yyunput(int, char *) ";

%feature("docstring")  yywrap "int yywrap(void) ";

%feature("docstring")  yy_init_globals "static int
yy_init_globals(void) ";

%feature("docstring")  yylex_destroy "int yylex_destroy(void) ";

%feature("docstring")  yyget_debug "int yyget_debug(void) ";

%feature("docstring")  yyset_debug "void yyset_debug(int debug_flag)
";

%feature("docstring")  yyget_extra "YY_EXTRA_TYPE yyget_extra(void)
";

%feature("docstring")  yyset_extra "void yyset_extra(YY_EXTRA_TYPE
user_defined) ";

%feature("docstring")  yyget_in "FILE * yyget_in(void)

Get the input stream. ";

%feature("docstring")  yyset_in "void yyset_in(FILE *in_str)

Set the input stream.

This does not discard the current input buffer.

Parameters:
-----------

in_str:  A readable stream.

See:  yy_switch_to_buffer ";

%feature("docstring")  yyget_out "FILE * yyget_out(void)

Get the output stream. ";

%feature("docstring")  yyset_out "void yyset_out(FILE *out_str) ";

%feature("docstring")  yyget_leng "yy_size_t yyget_leng(void)

Get the length of the current token. ";

%feature("docstring")  yyget_text "char * yyget_text(void)

Get the current token. ";

%feature("docstring")  yyget_lineno "int yyget_lineno(void)

Get the current line number. ";

%feature("docstring")  yyset_lineno "void yyset_lineno(int
line_number)

Set the current line number.

Parameters:
-----------

line_number:  ";

%feature("docstring")  input "static int input(void) ";

%feature("docstring")  yylex "int yylex(void) ";

%feature("docstring")  if "if(!(yy_init)) ";

%feature("docstring")  while "while(1) ";

%feature("docstring")  yyunput "static void yyunput(int c, register
char *yy_bp) ";

%feature("docstring")  isatty "int isatty(int) ";

%feature("docstring")  yy_fatal_error "static void
yy_fatal_error(yyconst char *msg) ";


// File: parsertypes_8h.xml


// File: token_8h.xml


// File: point_8cpp.xml
%feature("docstring")  detectSequenceType "DescriptorType
detectSequenceType(const yaml::Sequence &seq) ";

%feature("docstring")  sequenceDimension "int sequenceDimension(const
yaml::Sequence &seq, int n=1) ";

%feature("docstring")  parseYamlSequenceReal "RealDescriptor
parseYamlSequenceReal(const yaml::Sequence &seq) ";

%feature("docstring")  parseYamlSequenceString "StringDescriptor
parseYamlSequenceString(const yaml::Sequence &seq) ";


// File: stlfacade_2point_8cpp.xml


// File: point_8h.xml


// File: stlfacade_2point_8h.xml


// File: pointlayout_8cpp.xml


// File: pointlayout_8h.xml


// File: queryoptimizer_8h.xml


// File: queryoptimizer__impl_8h.xml


// File: region_8cpp.xml
%feature("docstring")  compareSegments "bool compareSegments(const
Segment &s1, const Segment &s2) ";

%feature("docstring")  mergeContiguous "QList<Segment>
mergeContiguous(DescriptorType type, DescriptorLengthType ltype, const
QList< Segment > &segments) ";


// File: region_8h.xml


// File: searchspace_8cpp.xml


// File: searchspace_8h.xml


// File: searchspace__impl_8h.xml
%feature("docstring")  pointerOrderCompare "bool
pointerOrderCompare(const SearchPointType &p1, const SearchPointType
&p2) ";

%feature("docstring")  pointerOrderCompare "bool
pointerOrderCompare(const SearchPoint &p1, const SearchPoint &p2) ";

%feature("docstring")  pointerOrderCompare "bool
pointerOrderCompare(const FrozenSearchPoint &p1, const
FrozenSearchPoint &p2) ";


// File: searchspacepool_8cpp.xml


// File: searchspacepool_8h.xml


// File: searchspacepool__impl_8h.xml
%feature("docstring")  copySearchPoints "void
copySearchPoints(BaseSearchSpace< SearchPointType, DataSetType >
*dest, const BaseSearchSpace< SearchPointType, DataSetType > *src, int
start, int end, int destStart) ";

%feature("docstring")  copySearchPoints "void
copySearchPoints(BaseSearchSpace< SearchPointType, DataSetType >
*dest, const BaseSearchSpace< SearchPointType, DataSetType > *src, int
start, int end) ";

%feature("docstring")  copySearchPoints "void
copySearchPoints(BaseSearchSpace< SearchPointType, DataSetType >
*dest, const BaseSearchSpace< SearchPointType, DataSetType > *src) ";

%feature("docstring")  refPoint "const Point* refPoint(const
SearchPointType &p, const DataSet *refDataSet) ";

%feature("docstring")  refPoint "const Point* refPoint(const
SearchPoint &p, const DataSet *refDataSet) ";

%feature("docstring")  refPoint "const Point* refPoint(const
FrozenSearchPoint &p, const DataSet *refDataSet) ";

%feature("docstring")  refPoint "const Point* refPoint(const
SearchPointType &p, const BaseSearchSpace< SearchPointType,
DataSetType > &sspace) ";

%feature("docstring")  refPoint "const Point* refPoint(const
SearchPoint &p, const SearchSpace &sspace) ";

%feature("docstring")  refPoint "const Point* refPoint(const
FrozenSearchPoint &p, const FrozenSearchSpace &sspace) ";

%feature("docstring")  binarySearch "int binarySearch(const
SearchSpaceType &sspace, int idx, Real value, int start, int end) ";


// File: counted__ptr_8h.xml


// File: gaia2std_8cpp.xml


// File: gaia2std_8h.xml


// File: origtypes_8h.xml


// File: streamutil_8h.xml


// File: stlfacade_2transformation_8cpp.xml


// File: transformation_8cpp.xml


// File: stlfacade_2transformation_8h.xml


// File: transformation_8h.xml


// File: view_8cpp.xml


// File: stlfacade_2view_8h.xml


// File: view_8h.xml


// File: textprogress_8h.xml


// File: timer_8cpp.xml


// File: timer_8h.xml


// File: frozensearch_8cpp.xml


// File: frozensearch_8h.xml


// File: gaiabench_8cpp.xml
%feature("docstring")  parseOptions "ParameterMap parseOptions(int
argc, char *argv[]) ";

%feature("docstring")  multiQuery "void multiQuery(FrozenDataSet
&dataset, int n) ";

%feature("docstring")  main "int main(int argc, char *argv[]) ";


// File: gaiafreeze_8cpp.xml
%feature("docstring")  parseOptions "ParameterMap parseOptions(int
argc, char *argv[]) ";

%feature("docstring")  main "int main(int argc, char *argv[]) ";


// File: gaiafusion_8py.xml


// File: gaiainfo_8cpp.xml
%feature("docstring")  parseOptions "ParameterMap parseOptions(int
argc, char *argv[]) ";

%feature("docstring")  main "int main(int argc, char *argv[]) ";


// File: gaiamerge_8cpp.xml
%feature("docstring")  descs_select "QStringList descs_select(\"*\")
";

%feature("docstring")  usage "void usage() ";

%feature("docstring")  parseOptions "ParameterMap parseOptions(int
argc, char *argv[]) ";

%feature("docstring")  main "int main(int argc, char *argv[]) ";


// File: types_8h.xml


// File: utils_8cpp.xml


// File: utils_8h.xml


// File: view__impl_8h.xml


// File: yamlcpp_8cpp.xml


// File: yamlcpp_8h.xml


// File: group__algorithms.xml


// File: group__analyzers.xml


// File: group__datasetmanagementalgos.xml


// File: group__datatransformationalgos.xml


// File: group__appliers.xml


// File: group__metrics.xml


// File: group__standardmetrics.xml


// File: group__compositemetrics.xml


// File: group__refmetrics.xml


// File: api_overview.xml


// File: dealing_with_vld.xml


// File: filtering.xml


// File: fingerprint_tutorial.xml


// File: tutorial.xml


// File: dir_e6df591b0639d4c4807ef64d058833e2.xml


// File: dir_659facb5ea1bf65b3deecf95a0be0132.xml


// File: dir_ee0a2921d8a41481a2eb4b0c39976a70.xml


// File: dir_6cd8491d143eb218b70983dbdb3c58bc.xml


// File: dir_68267d1309a1af8e8297ef4c3efbcdba.xml


// File: dir_f3eaf2ccd92dcae96d1b9152e788f026.xml


// File: dir_1563a38af0d3a5e4a6330d6d45e9792a.xml


// File: indexpage.xml

