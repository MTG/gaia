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

#include <algorithm>

template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>::BaseSearchSpace(const Filter* filter, bool ownsFilter)
  :  _dataset(0), _filter(filter), _ownsFilter(ownsFilter), _sortedUpTo(0), _filteredUpTo(0) {
  // NB: we can probably optimize out filtering here: if (!_filter) isEveryoneFiltered = true;
}

template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>::BaseSearchSpace(const BaseSearchSpace<SearchPointType, DataSetType>& other)
  : _dataset(0), _filter(0), _ownsFilter(false), _sortedUpTo(0), _filteredUpTo(0) {

  copyPointsFrom(other);
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::copyPointsFrom(const BaseSearchSpace<SearchPointType, DataSetType>& other) {
  // WARNING: TODO: here we first filter all points from other (due to size() instead of Base::size())
  //                this might slow things down if it's not what we want...
  int size = other.size();
  this->resize(size);
  memcpy(&(*this)[0], &other[0], size*sizeof(SearchPointType));
  this->setDataSet(other.dataSet());
}

template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>::~BaseSearchSpace() {
  if (_ownsFilter) delete _filter;
}

template <typename SearchPointType, typename DataSetType>
int BaseSearchSpace<SearchPointType, DataSetType>::size() const {
  BaseSearchSpace<SearchPointType, DataSetType>* space = const_cast<BaseSearchSpace<SearchPointType, DataSetType>*>(this);
  space->filterAll();

  return unfilteredSize();
}

template <typename SearchPointType, typename DataSetType>
int BaseSearchSpace<SearchPointType, DataSetType>::unfilteredSize() const {
  return (int)std::vector<SearchPointType>::size();
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::cleanSearchSpace() {
  _dataset = 0;
  if (_ownsFilter) delete _filter;
  _filter = 0;
  _ownsFilter = false;
  invalidate();
}


template <typename SearchPointType, typename DataSetType>
SearchResults BaseSearchSpace<SearchPointType, DataSetType>::get(int n, int offset) {
  if (offset + n > validUpTo()) {
    // should be able to sort only part of it if necessary, like if offset is small
    filterAll();
    sortAll();
  }

  SearchResults result;
  offset = qMin(offset, (int)size());
  int end = qMin(offset+n, (int)size());
  for (int i=offset; i<end; i++) {
    result << qMakePair(this->pointName(this->at(i)), this->at(i).dist);
  }

  return result;
}

template <typename SearchPointType, typename DataSetType>
inline void BaseSearchSpace<SearchPointType, DataSetType>::setFilter(const Filter* filter, bool ownsFilter) {
  if (_ownsFilter) delete _filter;

  _filter = filter;
  _ownsFilter = ownsFilter;

  _filteredUpTo = 0;
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::filterAll() {
  int size = unfilteredSize();

  // for trivial filters, we can consider all the points already filtered
  if (!_filter || _filter->isAlwaysTrue()) _filteredUpTo = size;
  if (_filteredUpTo == size) return;

  Q_ASSERT(_filteredUpTo <= size);
  Q_ASSERT(_sortedUpTo <= size);

  int lastAccepted = _filteredUpTo;

  for (int i=_filteredUpTo; i<size; i++) {
    if (this->validPoint(this->at(i))) {
      this->at(lastAccepted) = this->at(i);
      lastAccepted++;
    }
    // if we "remove" one point which had already been sorted, we also need to decrement the
    // number of sorted points
    else if (_sortedUpTo > i) {
      _sortedUpTo--;
    }
  }

  this->resize(lastAccepted);
  _filteredUpTo = lastAccepted;
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::sortAll() {
  // make sure you only call this function with all your points previously filtered
  Q_ASSERT(_filteredUpTo == unfilteredSize());
  gaia2::sort(this->begin() + _sortedUpTo, this->end());
  _sortedUpTo = (int)std::vector<SearchPointType>::size();
}



template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::limit(int n) {
  Q_ASSERT(_filteredUpTo <= unfilteredSize());
  Q_ASSERT(_sortedUpTo <= unfilteredSize());
  filterAndSort(n);
  std::vector<SearchPointType>::resize(qMin(n, unfilteredSize()));
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::clear() {
  invalidate();
  std::vector<SearchPointType>::clear();
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::thresholdLimit(float maxDist) {
  Q_ASSERT(_filteredUpTo <= unfilteredSize());
  Q_ASSERT(_sortedUpTo <= unfilteredSize());

  float maxValidDist = -1;
  int validIdx = validUpTo();
  int start = 0, end = 0;

  if (validIdx > 0) maxValidDist = this->at(validIdx-1).dist;
  if (maxValidDist < maxDist) {
    filterAll();
    sortAll();
    end = unfilteredSize();
  }
  else {
    end = validIdx;
  }

  if (end == 0) return; // space is empty, don't do anything stupid

  // find the cutoff point using a binary search
  if (maxDist < this->at(start).dist) { clear(); return; }
  if (maxDist > this->at(end-1).dist) { return; }

  while ((end - start) > 1) {
    int pivotIdx = (start + end) / 2;
    float distm = this->at(pivotIdx).dist;
    if (distm > maxDist) end = pivotIdx;
    else                 start = pivotIdx;
  }

  // we have start + 1 == end
  int finalSize = start;
  //if (at(start).dist <= maxDist) finalSize = end-1;
  finalSize++;

  this->resize(finalSize);
  _filteredUpTo = qMin(_filteredUpTo, finalSize);
  _sortedUpTo = qMin(_sortedUpTo, finalSize);
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::filterAndSort(int n) {
  int size = unfilteredSize();

  Q_ASSERT(_filteredUpTo <= size);
  Q_ASSERT(_sortedUpTo <= size);

  if (n < 0 || n > size) n = size;

  if (_filteredUpTo >= n && _sortedUpTo >= n) return;

  filterAll();
  n = qMin(n, unfilteredSize());

  std::partial_sort(this->begin(), this->begin() + n, this->end());
  _sortedUpTo = n;
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::addPoints(const DataSetType* dataset, const QList<QString>& ids) {
  foreach (const QString& id, ids) {
    this->push_back(SearchPointType(dataset->point(id),
                              dataset->referenceDataSet()->point(id)));
  }
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::removePoints(const QList<QString>& ids) {
  QList<int> positionsToRemove;
  QSet<QString> idSet = QSet<QString>::fromList(ids);

  // first find positions of all the elements to remove
  for (typename BaseSearchSpace<SearchPointType, DataSetType>::const_iterator it = this->begin(); it != this->end(); ++it) {
    if (idSet.contains(it->ptr->name())) {
      positionsToRemove << (it - this->begin());
    }
  }

  // then remove them all in 1 go
  sort(positionsToRemove.begin(), positionsToRemove.end());

  // TODO: this is not the most efficient way to do it...
  for (int i=(int)positionsToRemove.size()-1; i>=0; i--) {
    this->erase(this->begin() + positionsToRemove[i]);
  }

  // update values for _sortedUpto and _filteredUpTo, as they might now be bigger
  // than the size of our BaseSearchSpace
  int size = unfilteredSize();
  _sortedUpTo = qMin(_sortedUpTo, size);
  _filteredUpTo = qMin(_filteredUpTo, size);
}


template <typename SearchPointType>
inline bool pointerOrderCompare(const SearchPointType& p1, const SearchPointType& p2);


template <>
inline bool pointerOrderCompare(const SearchPoint& p1, const SearchPoint& p2) {
  return p1.ptr < p2.ptr;
}

template <>
inline bool pointerOrderCompare(const FrozenSearchPoint& p1, const FrozenSearchPoint& p2) {
  return p1.idx < p2.idx;
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::pointerSort() {
  gaia2::sort(range(*this), pointerOrderCompare<SearchPointType>);
  invalidate();
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::setIntersection(const BaseSearchSpace* other) {
  Q_ASSERT(_filteredUpTo == 0);
  Q_ASSERT(_sortedUpTo == 0);

  typename std::vector<SearchPointType>::iterator end = std::set_intersection(range(*this),
                                                                         range(*other),
                                                                         this->begin(), pointerOrderCompare<SearchPointType>);
  this->resize(end - this->begin());
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::setDifference(const BaseSearchSpace* other) {
  Q_ASSERT(_filteredUpTo == 0);
  Q_ASSERT(_sortedUpTo == 0);

  typename std::vector<SearchPointType>::iterator end = std::set_difference(range(*this),
                                                                       range(*other),
                                                                       this->begin(), pointerOrderCompare<SearchPointType>);
  this->resize(end - this->begin());
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpace<SearchPointType, DataSetType>::setUnion(const BaseSearchSpace* other) {
  Q_ASSERT(_filteredUpTo == 0);
  Q_ASSERT(_sortedUpTo == 0);

  std::vector<SearchPointType> tmp;
  int size = unfilteredSize();

  tmp.resize(size);
  memcpy(&tmp[0], &(*this)[0], size * sizeof(SearchPointType));

  this->resize(size + static_cast<const std::vector<SearchPointType>*>(other)->size());
  typename std::vector<SearchPointType>::iterator end = std::set_union(range(tmp),
                                                                  range(*other),
                                                                  this->begin(), pointerOrderCompare<SearchPointType>);
  this->resize(end - this->begin());

  Q_ASSERT(_filteredUpTo == 0);
  Q_ASSERT(_sortedUpTo == 0);
}



template <typename SearchPointType, typename DataSetType>
BaseResultSet<SearchPointType, DataSetType>::BaseResultSet() {
  _d = new SearchSpaceWrapper<SearchPointType, DataSetType>(new BaseSearchSpace<SearchPointType, DataSetType>());
}

template <typename SearchPointType, typename DataSetType>
BaseResultSet<SearchPointType, DataSetType>::BaseResultSet(BaseSearchSpace<SearchPointType, DataSetType>* sspace) {
  _d = new SearchSpaceWrapper<SearchPointType, DataSetType>(sspace);
}

template <typename SearchPointType, typename DataSetType>
SearchResults BaseResultSet<SearchPointType, DataSetType>::get(int n, int offset) {
  return _d->sspace->get(n, offset);
}

template <typename SearchPointType, typename DataSetType>
void BaseResultSet<SearchPointType, DataSetType>::addPoints(const DataSetType* dataset, const QList<QString>& ids) {
  _d->sspace->addPoints(dataset, ids);
}

template <typename SearchPointType, typename DataSetType>
void BaseResultSet<SearchPointType, DataSetType>::removePoints(const QList<QString>& ids) {
  _d->sspace->removePoints(ids);
}
