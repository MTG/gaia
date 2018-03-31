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


#include "view.h"
#include "timer.h"
#include "queryoptimizer.h"

namespace gaia2 {

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseView<DataSetType, PointType, SearchPointType, DistanceType>::BaseView(DataSetType* dataset) :
  _dataset(dataset), _isViewValid(false), _searchSpacePool(dataset) {

  init();
}


template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
void BaseView<DataSetType, PointType, SearchPointType, DistanceType>::init() {
  if (!_dataset) throw GaiaException("You need to specify a valid DataSet when creating a view");

  _dataset->addView(this);
  validate();
}


template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseView<DataSetType, PointType, SearchPointType, DistanceType>::~BaseView() {
  if (_dataset) {
    _dataset->removeView(this);
  }
}

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
void BaseView<DataSetType, PointType, SearchPointType, DistanceType>::indexOn(const QString& descriptorName) {
  validate(); // need to revalidate view cause we might have changed a ref dataset meanwhile
  _searchSpacePool.indexOn(descriptorName);
}


template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
void BaseView<DataSetType, PointType, SearchPointType, DistanceType>::invalidate() {
  G_DEBUG(GView, "invalidating view");
  _isViewValid = false;
}

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
void BaseView<DataSetType, PointType, SearchPointType, DistanceType>::validate() {
  if (_isViewValid) return;

  G_DEBUG(GView, "validating view");

  // check if the dataset still exists
  if (!_dataset) {
    throw GaiaException("Cannot revalidate view, as the underlying dataset has been deleted");
  }

  // make sure our indices and prepared search spaces are still up-to-date
  _searchSpacePool.recreate();
  G_DEBUG(GView, "Search space pool updated! View has been revalidated");

  _isViewValid = true;
}


template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
SearchResults BaseView<DataSetType, PointType, SearchPointType, DistanceType>::formatResults(std::priority_queue<SearchPointType>& resQueue) {
  SearchResults result;

  while (!resQueue.empty()) {
    result.append(Result(resQueue.top().ptr->name(),
                         resQueue.top().dist));
    resQueue.pop();
  }

  std::reverse(result.begin(), result.end());

  return result;
}



template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
void BaseView<DataSetType, PointType, SearchPointType, DistanceType>::bindFilter(const Filter* cfilter, const DataSetType* dataset) {
  Filter* filter = const_cast<Filter*>(cfilter);
  filter->updateVariables();

  // if there are unbound variables in the filter, we need to make sure a
  // reference dataset is available
  if (filter->needsBinding()) {
    const DataSet* refDataSet = dataset->referenceDataSet();
    if (!refDataSet) {
      throw GaiaException("Cannot use a filter on this query because no reference DataSet has been set.");
    }
    filter->bindToLayout(refDataSet->layout());
  }

  // this should be done anyway, because we don't need a specific layout
  filter->bindToPointAttributes();
}


template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
Filter* BaseView<DataSetType, PointType, SearchPointType, DistanceType>::createFilter(const QString& filterString) {

  QString filterStr = filterString;
  if (filterString == "") filterStr = "where true";

  Filter* filter = Filter::parse(filterStr);

  return filter;
}



template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseResultSet<SearchPointType, DataSetType> BaseView<DataSetType, PointType, SearchPointType, DistanceType>::nnSearch(const PointType& p,
                                                                                                                      const DistanceType* dist,
                                                                                                                      const Filter* filter) {
  Timer timer;
  timer.start();
  bool owns = (filter == 0); // if filter == 0, then we'll have to create it, so we own it
  ResultSetType result = nnSearch(p, getSearchSpace(0, filter, owns), dist);
  timer.stop();
  G_DEBUG(GPerf, "Total time for the full query (optimization + query): " << timer.elapsed() * 1000 << " ms");
  return result;
}

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseResultSet<SearchPointType, DataSetType> BaseView<DataSetType, PointType, SearchPointType, DistanceType>::nnSearch(const PointType& p,
                                                                                                                      const DistanceType* dist,
                                                                                                                      const QString& filter) {
  Timer timer;
  timer.start();
  ResultSetType result = nnSearch(p, getSearchSpace(0, filter), dist);
  timer.stop();
  G_DEBUG(GPerf, "Total time for the full query (optimization + query): " << timer.elapsed() * 1000 << " ms");
  return result;
}

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseResultSet<SearchPointType, DataSetType> BaseView<DataSetType, PointType, SearchPointType, DistanceType>::nnSearch(const PointType& p,
                                                                                                                      ResultSetType inputSpace,
                                                                                                                      const DistanceType* dist,
                                                                                                                      const Filter* filter) {
  Timer timer;
  timer.start();
  bool owns = (filter == 0); // if filter == 0, then we'll have to create it, so we own it
  ResultSetType result = nnSearch(p, getSearchSpace(inputSpace.searchSpace(), filter, owns), dist);
  timer.stop();
  G_DEBUG(GPerf, "Total time for the full query (optimization + query): " << timer.elapsed() * 1000 << " ms");
  return result;
}

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseResultSet<SearchPointType, DataSetType> BaseView<DataSetType, PointType, SearchPointType, DistanceType>::nnSearch(const PointType& p,
                                                                                                                      ResultSetType inputSpace,
                                                                                                                      const DistanceType* dist,
                                                                                                                      const QString& filter) {
  Timer timer;
  timer.start();
  ResultSetType result = nnSearch(p, getSearchSpace(inputSpace.searchSpace(), filter), dist);
  timer.stop();
  G_DEBUG(GPerf, "Total time for the full query (optimization + query): " << timer.elapsed() * 1000 << " ms");
  return result;
}




template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseView<DataSetType, PointType, SearchPointType, DistanceType>::getSearchSpace(SearchSpaceType* sspace,
                                                                                                                  const QString& filterString) {
  return getSearchSpace(sspace, createFilter(filterString), true);
}

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseView<DataSetType, PointType, SearchPointType, DistanceType>::getSearchSpace(SearchSpaceType* sspace,
                                                                                                                               const Filter* filter,
                                                                                                                               bool spaceOwnsFilter) {

  // need to (re)validate the view now, because otherwise we might get stale
  // search spaces...
  validate();

  // if no filter has been specified, get a default one
  if (!filter) {
    filter = Filter::parse("where true");
    spaceOwnsFilter = true;
  }

  // if no search space has been specified, get all points from dataset and
  // try to optimize that, otherwise just go with the given search space
  SearchSpaceType* result = 0;
  if (sspace) {
    G_DEBUG(GView, "Query on given SearchSpace, skipping optimization part");
    result = BaseSearchSpacePool<SearchPointType, DataSetType>::acquire();
    result->copyPointsFrom(*sspace);
    result->setFilter(filter, spaceOwnsFilter);
  }
  else {
    // searching in the whole dataset, trying to optimize the query
    G_DEBUG(GView, "Query on full dataset, getting optimized search space");
    Timer timer; timer.start();

    // if filter is not ours, make a copy, as the optimize function takes ownership
    // over the given filter
    if (!spaceOwnsFilter) filter = new Filter(*filter);

    BaseQueryOptimizer<SearchPointType, DataSetType> qo(&_searchSpacePool);
    result = qo.optimize(const_cast<Filter*>(filter));
    G_DEBUG(GPerf, "Time to do full optimization: " << timer.elapsed() * 1000 << " ms");
  }

  return result;
}


template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
BaseResultSet<SearchPointType, DataSetType> BaseView<DataSetType, PointType, SearchPointType, DistanceType>::nnSearch(const PointType& p,
                                                                                                                      SearchSpaceType* sspace,
                                                                                                                      const DistanceType* dist) {

  Timer fullTime, timer;
  fullTime.start();

  // 1- First make sure everything is in order, our layouts are good, etc...

  // make sure our filter variables are correctly bound
  bindFilter(sspace->filter(), _dataset);

  // make sure we have specified a DistanceFunction
  if (!dist) {
    throw GaiaException("View: you are trying to compute the nearest-neighbors of a point, "
                        "but you haven't specified any distance to use... Please do so.");
  }

  // revalidate the view, ie: make sure the underlying dataset is still valid
  timer.restart();
  validate();
  G_DEBUG(GPerf, "Time to (re)validate view: " << timer.elapsed() << " ms");

  // check that all layouts are matching
  timer.restart();
  checkPoint(p);
  checkDistance(dist);

  G_DEBUG(GPerf, "Time to compare layouts (dataset/query point/distance): "
          << timer.elapsed() << " ms");


  // 2- do the actual computation

  timer.restart();
  sspace->computeDistance(p, dist);
  G_DEBUG(GPerf, "Time to compute distances: " << timer.elapsed() * 1000 << " ms");

  timer.restart();
  sspace->filterAndSort();
  G_DEBUG(GPerf, "Time to filter and sort distances: " << timer.elapsed() * 1000 << " ms");


  G_DEBUG(GPerf, "Time to answer query: " << fullTime.elapsed() * 1000.0 << " ms");
  return ResultSetType(sspace);
}

} // namespace gaia2
