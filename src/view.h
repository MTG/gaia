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
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/


#ifndef GAIA_VIEW_H
#define GAIA_VIEW_H

#include <QPointer>
#include <vector>
#include <queue>
#include "dataset.h"
#include "searchspacepool.h"
#include "metrics/distancefunction.h"
#include "metrics/frozendistance.h"

namespace gaia2 {

class Filter;

/**
 * The View class contains a specific view on a dataset.
 * It has a specific distance function and can also work on subsets of the
 * whole dataset. A dataset can then be considered as a collection of different
 * datasets and can query any, some or all of them for similarity searches.
 */
template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
class BaseView {

 public:

  typedef BaseResultSet<SearchPointType, DataSetType> ResultSetType;
  typedef BaseSearchSpace<SearchPointType, DataSetType> SearchSpaceType;

  BaseView(DataSetType* dataset);
  ~BaseView();

  ResultSetType nnSearch(const PointType& p, const DistanceType* dist, const Filter* filter = 0);
  ResultSetType nnSearch(const PointType& p, const DistanceType* dist, const QString& filter);

  ResultSetType nnSearch(const PointType& p, ResultSetType inputSpace,
                         const DistanceType* dist, const Filter* filter = 0);
  ResultSetType nnSearch(const PointType& p, ResultSetType inputSpace,
                         const DistanceType* dist, const QString& filter);


  template <typename T>
  ResultSetType nnSearch(const QString& id, T x) {
    validate(); // to make sure that _dataset is not a dangling pointer
    return nnSearch(*_dataset->point(id), x);
  }

  template <typename T, typename U>
  ResultSetType nnSearch(const QString& id, T x, U y) {
    validate(); // to make sure that _dataset is not a dangling pointer
    return nnSearch(*_dataset->point(id), x, y);
  }

  template <typename T, typename U, typename V>
  ResultSetType nnSearch(const QString& id, T x, U y, V z) {
    validate(); // to make sure that _dataset is not a dangling pointer
    return nnSearch(*_dataset->point(id), x, y, z);
  }

  /**
   * This function is called whenever the DataSet it points to changes, so it
   * knows it needs to recompute SortingStruct, maybe its filters, etc...
   */
  void invalidate();

  /**
   * This function is called to make sure that the View is valid (ie: its sorting
   * structure and filters actually point to existing objects).
   */
  void validate();

  /**
   * This methods creates an index on the given descriptor name, in the hope that
   * future queries using this descriptor in a filter will be faster.
   */
  void indexOn(const QString& descriptorName);



 protected:
  QPointer<DataSetType>  _dataset;
  bool _isViewValid;
  // should have an associated mutex when using multi-threaded code
  BaseSearchSpacePool<SearchPointType, DataSetType> _searchSpacePool;


  void init();

  // a few helper functions
  static Filter* createFilter(const QString& filterString);
  static void bindFilter(const Filter* filter, const DataSetType* dataset);
  // WARNING: this function modifies the results variable.
  static SearchResults formatResults(std::priority_queue<SearchPointType>& results);


  // internal nnsearch methods & helpers

  void checkPoint(const PointType& p); // needs to be specialized
  void checkDistance(const DistanceType* dist); // needs to be specialized

  /**
   * Returns a valid SearchSpace with associated filter. In case some of the
   * input arguments are not given, it will try to fill them up using default
   * values: the full dataset if no search space is given, a "where true" filter
   * if not filter is given.
   *
   * @param spaceOwnsFilter whether you want to yield ownership of the filter
   *        to the resulting dataset or not. If true, the const qualifier on
   *        the given filter is ignored.
   */
  SearchSpaceType* getSearchSpace(SearchSpaceType* sspace = 0,
                                  const Filter* filter = 0,
                                  bool spaceOwnsFilter = true);

  /**
   * Same thing as the previous method, except the filter is specified as a
   * string and thus will be parsed before giving it to the search space.
   */
  SearchSpaceType* getSearchSpace(SearchSpaceType* sspace,
                                  const QString& filterString);

  /**
   * The nnSearch method which is actually doing all the work. All arguments should
   * already be valid (ie: no null pointer) (layouts will be checked, though).
   */
  ResultSetType nnSearch(const PointType& p,
                         SearchSpaceType* sspace,
                         const DistanceType* dist);


};

typedef BaseView<DataSet, Point, SearchPoint, DistanceFunction> View;
typedef BaseView<FrozenDataSet, FrozenPoint, FrozenSearchPoint, FrozenDistance> FrozenView;
typedef BaseResultSet<FrozenSearchPoint, FrozenDataSet> FrozenResultSet;

template <>
inline void View::checkPoint(const Point& p) {
  if (p.layout() != _dataset->layout()) {
    QStringList msg;
    msg << "Cannot query for point '" << p.name() << "', because it has a different "
        << "layout than the one from the dataset which the view points at.";
    throw GaiaException(msg);
  }
}

template <>
inline void View::checkDistance(const DistanceFunction* dist) {
  if (dist->layout() != _dataset->layout()) {
    QStringList msg;
    msg << "View: cannot use given distance because the DataSet and the DistanceFunction "
        << "don't share the same layout\n"
        << "Different descriptors: "
        << dist->layout().symmetricDifferenceWith(_dataset->layout()).join(", ");
    throw GaiaException(msg);
  }
}

template <>
inline void FrozenView::checkPoint(const FrozenPoint& p) {
  if (p.cols() != _dataset->cols()) {
    QStringList msg;
    msg << "Cannot query for point because it has a different dimension "
        << "than the one from the dataset which the view points at.";
    throw GaiaException(msg);
  }
}

template <>
inline void FrozenView::checkDistance(const FrozenDistance* dist) {
  if (&dist->dataset() != _dataset) {
    QStringList msg;
    msg << "View: you can only use a distance which has been created with "
        << "the same dataset as the one that was used to create this view.";
    throw GaiaException(msg);
  }
}


template <> template <typename T>
FrozenResultSet FrozenView::nnSearch(const QString& id, T x) {
  validate(); // to make sure that _dataset is not a dangling pointer
  return nnSearch(_dataset->row(_dataset->pointIndex(id)), x);
}

template <> template <typename T, typename U>
FrozenResultSet FrozenView::nnSearch(const QString& id, T x, U y) {
  validate(); // to make sure that _dataset is not a dangling pointer
  return nnSearch(_dataset->row(_dataset->pointIndex(id)), x, y);
}

template <> template <typename T, typename U, typename V>
FrozenResultSet FrozenView::nnSearch(const QString& id, T x, U y, V z) {
  validate(); // to make sure that _dataset is not a dangling pointer
  return nnSearch(_dataset->row(_dataset->pointIndex(id)), x, y, z);
}



} // namespace gaia2


// include implementation as View is a template class
#include "view_impl.h"


#endif // GAIA_VIEW_H
