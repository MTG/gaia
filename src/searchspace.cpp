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

#include <queue>
#include "searchspace.h"
#include "searchspacepool.h"
#include "timer.h"
#include "metrics/distancefunction.h"
#include "metrics/frozendistance.h"
#include "parser/filter.h"

namespace gaia2 {


yaml::Node toYaml(const SearchResults& results) {
  yaml::Sequence result;
  for (int i=0; i<results.size(); i++) {
    yaml::Sequence r;
    r << results[i].first << QString::number(results[i].second, 'g', 12);
    result << yaml::Node(r);
  }
  return result;
}



#ifdef HAVE_TBB

#undef range

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

class ComputeDistanceFunctor {
  SearchSpace& _sspace;
  const Point* _query;
  const DistanceFunction* _dist;

 public:
  ComputeDistanceFunctor(SearchSpace* sspace, const Point* query,
                         const DistanceFunction* dist) :
    _sspace(*sspace), _query(query), _dist(dist) {}

  void operator()(const tbb::blocked_range<int>& r) const {
    SearchSpace& sspace = _sspace;
    const Point* query = _query;
    const DistanceFunction* dist = _dist;

    for (int i=r.begin(); i!=r.end(); ++i) {
      SearchPoint& cur = sspace[i];
      cur.dist = (*dist)(*query, *cur.ptr);
    }
  }

};


void SearchSpace::computeDistance(const Point* query, const DistanceFunction* dist) {
  _sortedUpTo = 0;
  tbb::parallel_for(tbb::blocked_range<int>(0, (int)Base::size(), 1000),
                    ComputeDistanceFunctor(this, query, dist));
}

#else // HAVE_TBB


template <>
SearchSpace* SearchSpace::createFromDataSet(const DataSet* dataset) {
  SearchSpace* result = new SearchSpace(); // TODO: should get it from the pool here
  result->setDataSet(dataset);
  const DataSet* refDataSet = dataset->referenceDataSet();

  const int size = dataset->size();
  result->resize(size);
  for (int i=0; i<size; i++) {
    (*result)[i].ptr = dataset->at(i);
    (*result)[i].ref = refDataSet->at(i);
  }

  result->filterAll();

  return result;
}

template <>
SearchSpace* SearchSpace::createFromPoints(const DataSet* dataset,
                                           const QList<QString>& pointNames) {
  int npoints = pointNames.size();
  const DataSet* refDataSet = dataset->referenceDataSet();

  SearchSpace* result = SearchSpacePool::acquire();
  result->setDataSet(dataset);
  result->setFilter(Filter::parse("where true"), true);
  result->resize(npoints);

  for (int i=0; i<npoints; i++) {
    try {
      // exception would happen in case one of the point names isn't a valid one
      (*result)[i].ptr = dataset->point(pointNames[i]);
      (*result)[i].ref = refDataSet->point(pointNames[i]);
    }
    catch (GaiaException&) {
      npoints--; i--;
      result->resize(npoints);
    }
  }

  return result;
}

template <> template <>
void SearchSpace::computeDistance(const Point& query, const DistanceFunction* dist) {
  invalidate();
  const int size = unfilteredSize();
  for (int i=0; i<size; i++) {
    SearchPoint& cur = (*this)[i];
    cur.dist = (*dist)(query, *cur.ptr);
  }
}






template <>
FrozenSearchSpace* FrozenSearchSpace::createFromDataSet(const FrozenDataSet* dataset) {
  FrozenSearchSpace* result = new FrozenSearchSpace(); // TODO: should get it from the pool here
  result->setDataSet(dataset);

  const int size = dataset->size();
  result->resize(size);
  for (int i=0; i<size; i++) {
    (*result)[i].idx = i;
  }

  return result;
}

template <>
FrozenSearchSpace* FrozenSearchSpace::createFromPoints(const FrozenDataSet* dataset,
                                                       const QList<QString>& pointNames) {
  int npoints = pointNames.size();

  FrozenSearchSpace* result = FrozenSearchSpacePool::acquire();
  result->setDataSet(dataset);
  result->setFilter(Filter::parse("where true"), true);
  result->resize(npoints);

  for (int i=0; i<npoints; i++) {
    try {
      // exception would happen in case one of the point names isn't a valid one
      (*result)[i].idx = dataset->pointIndex(pointNames[i]);
    }
    catch (GaiaException&) {
      npoints--; i--;
      result->resize(npoints);
    }
  }

  return result;
}

template <> template <>
void FrozenSearchSpace::computeDistance(const FrozenPoint& query, const FrozenDistance* dist) {
  invalidate();
  const int npoints = unfilteredSize();

  const_cast<FrozenDistance*>(dist)->prepare(query);

  for (int i=0; i<npoints; i++) {
    FrozenSearchPoint& cur = (*this)[i];
    cur.dist = (*dist)(cur.idx, query);
  }
}


#endif // HAVE_TBB


} // namespace gaia2
