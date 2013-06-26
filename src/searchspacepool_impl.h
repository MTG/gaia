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



// this function assumes that the memory has already been allocated
template <typename SearchPointType, typename DataSetType>
void copySearchPoints(BaseSearchSpace<SearchPointType, DataSetType>* dest,
                      const BaseSearchSpace<SearchPointType, DataSetType>* src,
                      int start, int end, int destStart) {
  dest->setDataSet(src->dataSet());
  int howmany = end - start;
  if (howmany > 0) { // cause memcpy doesn't necessarily like copies of size 0
    SearchPointType* destp = &((*dest)[destStart]);
    const SearchPointType* srcp = &((*src)[start]);
    memcpy(destp, srcp,  howmany * sizeof(SearchPointType));
  }
}

// this function allocates the memory for dest
template <typename SearchPointType, typename DataSetType>
void copySearchPoints(BaseSearchSpace<SearchPointType, DataSetType>* dest,
                      const BaseSearchSpace<SearchPointType, DataSetType>* src,
                      int start, int end) {
  dest->resize(end - start);
  copySearchPoints(dest, src, start, end, 0);
}

template <typename SearchPointType, typename DataSetType>
void copySearchPoints(BaseSearchSpace<SearchPointType, DataSetType>* dest,
                      const BaseSearchSpace<SearchPointType, DataSetType>* src) {
  copySearchPoints(dest, src, 0, src->size());
}


template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseSearchSpacePool<SearchPointType, DataSetType>::ValueIndex::getSubSpaceInto(SearchSpaceType* result, int start, int end) const {
  // get the maximum pre-sorted subspace we can get, then use set_union to merge it
  // with the remaining part. this should be much sorter than just getting the whole
  // stuff and then sorting it.

  result->setDataSet(sspace->dataSet());
  result->resize(end - start);
  QSet<int> stepsSet;
  QPair<int, int> key;
  foreach (key, sorted.keys()) {
    stepsSet << key.first << key.second;
  }
  QList<int> steps = QList<int>::fromSet(stepsSet);
  sort(steps);

  Q_ASSERT(steps.first() == 0 && steps.last() == sspace->size());

  int i;

  if (start == 0) {
    for (i=0; i<steps.size(); i++) {
      if (end < steps[i]) break;
    }
    i--;

    if (i <= 0) { // no need to merge
      copySearchPoints(result, sspace, start, end);
      result->pointerSort();
    }
    else { // merge with presorted chunk
      SearchSpaceType* presorted = sorted[qMakePair(0, steps[i])];
      SearchSpaceType tmp; // need a temp copy because we need to sort it before we merge it
      copySearchPoints(&tmp, sspace, steps[i], end);
      tmp.pointerSort();
      typename Vector<SearchPointType>::iterator final = std::set_union(presorted->begin(), presorted->end(),
                                                                        tmp.begin(), tmp.end(),
                                                                        result->begin(), pointerOrderCompare<SearchPointType>);
      Q_ASSERT(final == result->end());
      Q_UNUSED(final); // needed in case we don't compile the assert
    }
  }
  else if (end == sspace->size()) {
    for (i=steps.size()-1; i>=0; i--) {
      if (start > steps[i]) break;
    }
    i++;

    if (i >= (steps.size()-1)) { // no need to merge
      copySearchPoints(result, sspace, start, end);
      result->pointerSort();
    }
    else { // merge with presorted chunk
      SearchSpaceType* presorted = sorted[qMakePair(steps[i], end)];
      SearchSpaceType tmp;
      copySearchPoints(&tmp, sspace, start, steps[i]);
      tmp.pointerSort();
      typename Vector<SearchPointType>::iterator final = std::set_union(presorted->begin(), presorted->end(),
                                                                        tmp.begin(), tmp.end(),
                                                                        result->begin(), pointerOrderCompare<SearchPointType>);
      Q_ASSERT(final == result->end());
      Q_UNUSED(final); // needed in case we don't compile the assert
    }
  }
  else {
    // Here we could both do the contained presorted interval, then merge the remaining ends,
    // or the containing presorted interval, then remove the points in excess. We choose the
    // latter, because then we can do the set_difference in place.
    // So:
    // - get the presorted chunk from L(start) to U(end)
    // - remove points in excess (with set_difference) from start to U(Start) and from L(end) to end
    int is, ie;
    for (is=steps.size()-1; is>=0; is--) if (steps[is] <= start) break;  is = qMax(is, 0);
    for (ie=0; ie<steps.size(); ie++)    if (steps[ie] >= end) break;    ie = qMin(ie, steps.size()-1);

    copySearchPoints(result, sorted[qMakePair(steps[is], steps[ie])]);
    SearchSpaceType tmp;

    // remove left chunk
    copySearchPoints(&tmp, sspace, steps[is], start);
    tmp.pointerSort();
    result->setDifference(&tmp);

    // remove right chunk
    copySearchPoints(&tmp, sspace, end, steps[ie]);
    tmp.pointerSort();
    result->setDifference(&tmp);
  }

  return result;
}



template <typename SearchPointType, typename DataSetType>
BaseSearchSpacePool<SearchPointType, DataSetType>::BaseSearchSpacePool(const DataSetType* dataset) : _dataset(dataset), _originalSpace(0) {
  recreate();
}

template <typename SearchPointType, typename DataSetType>
BaseSearchSpacePool<SearchPointType, DataSetType>::~BaseSearchSpacePool() {
  clearIndices();
}


// helper function
/*
template <typename SearchPointType, typename DataSetType>
SearchSpaceType* createSearchSpaceFromDataSet(const DataSetType* dataset);

template<>
SearchSpace* createSearchSpaceFromDataSet(const DataSet* dataset) {
  SearchSpace* result = new SearchSpace();
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

template<>
FrozenSearchSpace* createSearchSpaceFromDataSet(const FrozenDataSet* dataset) {
  FrozenSearchSpace* result = new FrozenSearchSpace();
}
*/

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::recreate() {
  // save the names of the descriptors which have been indexed to be able to reindex them
  QStringList valueIndices = _valueIndex.keys();
  QStringList labelIndices = _labelIndex.keys();
  clearIndices();

  delete _originalSpace;
  _originalSpace = SearchSpaceType::createFromDataSet(_dataset);

  // TODO: need to recreate previously existing indices (?)
  foreach (const QString& name, valueIndices) indexOn(name);
  foreach (const QString& name, labelIndices) indexOn(name);
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::clearIndices() {
  delete _originalSpace;
  _originalSpace = 0;

  foreach (const QString& desc, _valueIndex.keys()) _valueIndex.value(desc).clear();
  _valueIndex.clear();

  foreach (const QString& desc, _labelIndex.keys()) _labelIndex.value(desc).clear();
  _labelIndex.clear();
}


template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseSearchSpacePool<SearchPointType, DataSetType>::getAllPoints() const {
  SearchSpaceType* result = BaseSearchSpacePool<SearchPointType, DataSetType>::acquire();
  copySearchPoints(result, _originalSpace);
  result->setFilter(Filter::parse("where true"), true);
  return result;
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::indexOn(const QString& descriptorName) {
  PointLayout layout = _dataset->referenceDataSet()->layout();
  QString fullName = layout.fullName(descriptorName);
  DescriptorType type = layout.descriptorLocation(fullName).type();

  switch (type) {

  case RealType:
    return indexOnValue(fullName);

  case StringType:
    return indexOnLabel(fullName);

  case EnumType:
    return indexOnEnum(fullName);

  default:
    throw GaiaException("SearchSpacePool::indexOn: internal error, invalid indexing type");
  }
}


template <typename SearchPointType>
const Point* refPoint(const SearchPointType& p, const DataSet* refDataSet);

template <>
inline const Point* refPoint(const SearchPoint& p, const DataSet* refDataSet) {
  Q_UNUSED(refDataSet);
  return p.ref;
}

template <>
inline const Point* refPoint(const FrozenSearchPoint& p, const DataSet* refDataSet) {
  return refDataSet->at(p.idx);
}

template <typename SearchPointType, typename DataSetType>
const Point* refPoint(const SearchPointType& p, const BaseSearchSpace<SearchPointType, DataSetType>& sspace);

template <>
inline const Point* refPoint(const SearchPoint& p, const SearchSpace& sspace) {
  Q_UNUSED(sspace);
  return p.ref;
}

template <>
inline const Point* refPoint(const FrozenSearchPoint& p, const FrozenSearchSpace& sspace) {
  return sspace.dataSet()->referenceDataSet()->at(p.idx);
}

template <typename DataSetType, DescriptorType DescType>
class SortOn {
 public:
  SortOn(const DataSetType* dataset, const QString& descriptorName) {
    _refDataSet = dataset->referenceDataSet();
    // we assume that the descriptor is a fixed-length one
    Region region = _refDataSet->layout().descriptorLocation(descriptorName);
    if (region.lengthType() != FixedLength) {
      throw GaiaException("SearchSpacePool: you can only index on fixed-length descriptors");
    }

    if (region.size(DescType, FixedLength) != 1) {
      throw GaiaException("SearchSpacePool: you can only index on descriptors of size 1");
    }

    _index = region.index();
  }

  template <typename SearchPointType>
  inline bool operator()(const SearchPointType& p1, const SearchPointType& p2);

 protected:
  template <typename SearchPointType>
  inline const Point* ref(const SearchPointType& p) { return refPoint(p, _refDataSet); }
  const DataSet* _refDataSet;
  int _index;
};


template <> template <>
inline bool SortOn<DataSet, RealType>::operator()(const SearchPoint& p1, const SearchPoint& p2) {
  return ref(p1)->frealData()[_index] < ref(p2)->frealData()[_index];
}

template <> template <>
inline bool SortOn<DataSet, StringType>::operator()(const SearchPoint& p1, const SearchPoint& p2) {
  return ref(p1)->fstringData()[_index] < ref(p2)->fstringData()[_index];
}

template <> template <>
inline bool SortOn<DataSet, EnumType>::operator()(const SearchPoint& p1, const SearchPoint& p2) {
  // we don't need to sort alphanumerically as we just want to have points with the same
  // value grouped together (because in the filters we only allow == and !=)
  return ref(p1)->fenumData()[_index] < ref(p2)->fenumData()[_index];
}

template <> template <>
inline bool SortOn<FrozenDataSet, RealType>::operator()(const FrozenSearchPoint& p1, const FrozenSearchPoint& p2) {
  return ref(p1)->frealData()[_index] < ref(p2)->frealData()[_index];
}

template <> template <>
inline bool SortOn<FrozenDataSet, StringType>::operator()(const FrozenSearchPoint& p1, const FrozenSearchPoint& p2) {
  return ref(p1)->fstringData()[_index] < ref(p2)->fstringData()[_index];
}

template <> template <>
inline bool SortOn<FrozenDataSet, EnumType>::operator()(const FrozenSearchPoint& p1, const FrozenSearchPoint& p2) {
  // we don't need to sort alphanumerically as we just want to have points with the same
  // value grouped together (because in the filters we only allow == and !=)
  return ref(p1)->fenumData()[_index] < ref(p2)->fenumData()[_index];
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::indexOnValue(const QString& descriptorName) {
  SearchSpaceType* index = getAllPoints();
  gaia2::sort(range(*index), SortOn<DataSetType, RealType>(_dataset, descriptorName));

  ValueIndex completeIndex;
  completeIndex.sspace = index;

  // make a cumulatively-sorted array of subspaces so we don't have to re-sort by pointer
  // each time we return a subspace
  static const int factor = 10;
  const int chunkSize = (index->size() / factor) + 1;
  int split = 0;

  // presort full search space
  SearchSpaceType* full = new SearchSpaceType();
  copySearchPoints(full, index);
  full->pointerSort();
  completeIndex.sorted.insert(qMakePair(0, full->size()), full);

  // presort rays (ie: either start is 0, or end is 0)
  for (int i=0; i<factor-1; i++) {
    split += chunkSize;

    SearchSpaceType* half = new SearchSpaceType();
    copySearchPoints(half, index, 0, split);
    half->pointerSort();
    completeIndex.sorted.insert(qMakePair(0, split), half);

    half = new SearchSpaceType();
    copySearchPoints(half, index, split, index->size());
    half->pointerSort();
    completeIndex.sorted.insert(qMakePair(split, (int)index->size()), half);
  }

  // also presort segments (ie: neither start nor end is 0)
  // memory usage should be about 3 times than without it (for factor=10)
  // Note: we could make this entire function fit in this nested loop, but
  //       having it separate makes it easier to not compute the segments if
  //       we realize later that they suck up too much memory
  for (int end=2*chunkSize; end < (int)index->size(); end += chunkSize) {
    for (int start=chunkSize; start < end; start += chunkSize) {
      SearchSpaceType* segment = new SearchSpaceType();
      copySearchPoints(segment, index, start, end);
      segment->pointerSort();
      completeIndex.sorted.insert(qMakePair(start, end), segment);
    }
  }

  _valueIndex.insert(descriptorName, completeIndex);
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::indexOnLabel(const QString& descriptorName) {
  SearchSpaceType* index = getAllPoints();
  gaia2::sort(range(*index), SortOn<DataSetType, StringType>(_dataset, descriptorName));

  LabelIndex completeIndex;
  completeIndex.sspace = index;

  // sort again each subspace, but by pointer order this time, and also fill the
  // hashmap with the ranges
  const DataSet* rds = _dataset->referenceDataSet();
  int idx = rds->layout().descriptorLocation(descriptorName).index();
  typename Vector<SearchPointType>::iterator start = index->begin(), end = index->begin();
  while (start != index->end()) {
    const QString& value = refPoint(*start, rds)->fstringData()[idx];
    while ((end != index->end()) && (refPoint(*end, rds)->fstringData()[idx] == value)) {
      end++;
    }
    gaia2::sort(start, end, pointerOrderCompare<SearchPointType>);
    completeIndex.ranges.insert(value, qMakePair((int)(start - index->begin()), (int)(end - index->begin())));
    start = end;
  }

  _labelIndex.insert(descriptorName, completeIndex);
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::indexOnEnum(const QString& descriptorName) {
  SearchSpaceType* index = getAllPoints();
  gaia2::sort(range(*index), SortOn<DataSetType, EnumType>(_dataset, descriptorName));

  LabelIndex completeIndex;
  completeIndex.sspace = index;

  // sort again each subspace, but by pointer order this time, and also fill the
  // hashmap with the ranges
  const DataSet* rds = _dataset->referenceDataSet();
  int idx = rds->layout().descriptorLocation(descriptorName).index();
  typename Vector<SearchPointType>::iterator start = index->begin(), end = index->begin();
  while (start != index->end()) {
    Enum value = refPoint(*start, rds)->fenumData()[idx];
    while ((end != index->end()) && (refPoint(*end, rds)->fenumData()[idx] == value)) {
      end++;
    }
    gaia2::sort(start, end, pointerOrderCompare<SearchPointType>);
    QString svalue = refPoint(*start, rds)->layout().enumToString(descriptorName, value);
    completeIndex.ranges.insert(svalue, qMakePair((int)(start - index->begin()), (int)(end - index->begin())));
    start = end;
  }

  _labelIndex.insert(descriptorName, completeIndex);
}

// if value is not in array, returns the index of the first value which is > to given value
template <typename SearchSpaceType>
inline int binarySearch(const SearchSpaceType& sspace, int idx, Real value, int start, int end) {
  while (start < end) {
    int middle = (start + end) / 2;
    if (refPoint(sspace[middle], sspace)->frealData()[idx] < value) {
      start = middle + 1;
    }
    else {
      end = middle;
    }
  }
  return start;
}

template <typename SearchPointType, typename DataSetType>
bool BaseSearchSpacePool<SearchPointType, DataSetType>::hasIndex(const QString& descriptorName) const {
  return _valueIndex.contains(descriptorName) || _labelIndex.contains(descriptorName);
}


template <typename SearchPointType, typename DataSetType>
int BaseSearchSpacePool<SearchPointType, DataSetType>::getCuttingPos(const SearchSpaceType& sspace, int idx, int type,
                                                                     Real value, int start, int end) const {

  // estimated cutting point, will need to be adjusted
  int cuttingPos = binarySearch(sspace, idx, value, start, end);

  switch (type) {
  case LT:
    while (cuttingPos > start && refPoint(sspace[cuttingPos-1], sspace)->frealData()[idx] >= value) cuttingPos--;
    break;

  case LTE:
    while (cuttingPos < end && refPoint(sspace[cuttingPos], sspace)->frealData()[idx] <= value) cuttingPos++;
    while (cuttingPos > start && refPoint(sspace[cuttingPos-1], sspace)->frealData()[idx] > value) cuttingPos--;
    //cuttingPos++;
    break;

  case GT:
    while (cuttingPos < end && refPoint(sspace[cuttingPos], sspace)->frealData()[idx] <= value) cuttingPos++;
    break;

  case GTE:
    while (cuttingPos > start && refPoint(sspace[cuttingPos-1], sspace)->frealData()[idx] >= value) cuttingPos--;
    //cuttingPos++;
    while (cuttingPos < end && refPoint(sspace[cuttingPos], sspace)->frealData()[idx] < value) cuttingPos++;
    break;


  case EQ:
    //break;
  case NEQ:
    throw GaiaException("SearchSpacePool::getCuttingPos undefined for '==' and '!='");
  }

  return cuttingPos;
}

template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseSearchSpacePool<SearchPointType, DataSetType>::getSubSpace(const parser::Predicate* pred) const {
  // in case descriptor is not indexed, we return a null pointer

  const parser::PredValueComparison* comp = dynamic_cast<const parser::PredValueComparison*>(pred);
  if (comp) {
    // check for indexable comparisons: (var op constant) or (constant op var)
    // ie: (var op var) and (constant op constant) are forbidden here
    if (! ((dynamic_cast<const parser::ValueVariable*>(comp->_lhs) && dynamic_cast<const parser::ValueConstant*>(comp->_rhs)) ||
           (dynamic_cast<const parser::ValueConstant*>(comp->_lhs) && dynamic_cast<const parser::ValueVariable*>(comp->_rhs)))) {
      return 0;
    }

    const parser::ValueVariable* var = 0;
    const parser::ValueConstant* value = 0;
    if (dynamic_cast<const parser::ValueVariable*>(comp->_lhs)) {
      var = dynamic_cast<const parser::ValueVariable*>(comp->_lhs);
      value = dynamic_cast<const parser::ValueConstant*>(comp->_rhs);
    }
    else {
      var = dynamic_cast<const parser::ValueVariable*>(comp->_rhs);
      value = dynamic_cast<const parser::ValueConstant*>(comp->_lhs);
    }

    Q_ASSERT(var && value); // unnecessary, but just to make sure

    // look whether we have an index on that descriptor, and if yes:
    // - get the exact cutting point, as well as whether we should take the lower
    //   part or the upper one.
    // - returns this subspace
    QString fullName = _dataset->referenceDataSet()->layout().fullName(var->name());
    if (!hasIndex(fullName)) return 0;

    SearchSpaceType& fullSpace = *_valueIndex[fullName].sspace;
    int idx = _dataset->referenceDataSet()->layout().descriptorLocation(fullName).index();
    int start = 0, end = fullSpace.size(); // range to be extracted from the full searchspace

    switch (comp->_type) {
    case LT:
    case LTE:
      end = getCuttingPos(fullSpace, idx, comp->_type, (Real)value->value(), start, end);
      break;

    case GT:
    case GTE:
      start = getCuttingPos(fullSpace, idx, comp->_type, (Real)value->value(), start, end);
      break;

    case EQ:
    case NEQ:
      throw GaiaException("SearchSpacePool: cannot optimize with a != operator atm. Please fill a feature request.");
    }

    Filter* alwaysTrue = Filter::parse("where true");
    SearchSpaceType* result = BaseSearchSpacePool<SearchPointType, DataSetType>::acquire();
    result->setFilter(alwaysTrue, true);

    _valueIndex[fullName].getSubSpaceInto(result, start, end);

    return result;
  }

  const parser::PredValueRange* rangePred = dynamic_cast<const parser::PredValueRange*>(pred);
  if (rangePred) {
    if (!dynamic_cast<parser::ValueVariable*>(rangePred->_var)) return 0;

    QString fullName = _dataset->referenceDataSet()->layout().fullName(rangePred->_var->toString());
    if (!hasIndex(fullName)) return 0;

    SearchSpaceType& fullSpace = *_valueIndex[fullName].sspace;
    int idx = _dataset->referenceDataSet()->layout().descriptorLocation(fullName).index();
    int start = 0, end = fullSpace.size(); // range to be extracted from the full searchspace

    // find both cutting points, inclusive
    start = getCuttingPos(fullSpace, idx, GTE, rangePred->_min, start, end);
    end = getCuttingPos(fullSpace, idx, LTE, rangePred->_max, start, end);

    Filter* alwaysTrue = Filter::parse("where true");
    SearchSpaceType* result = BaseSearchSpacePool<SearchPointType, DataSetType>::acquire();
    result->setFilter(alwaysTrue, true);

    _valueIndex[fullName].getSubSpaceInto(result, start, end);

    return result;
  }

  const parser::PredLabelComparison* labelPred = dynamic_cast<const parser::PredLabelComparison*>(pred);
  if (labelPred) {
    // check for indexable comparisons: (var op constant) or (constant op var)
    // ie: (var op var) and (constant op constant) are forbidden here
    if (! ((dynamic_cast<const parser::LabelVariable*>(labelPred->_lhs) && dynamic_cast<const parser::LabelConstant*>(labelPred->_rhs)) ||
           (dynamic_cast<const parser::LabelConstant*>(labelPred->_lhs) && dynamic_cast<const parser::LabelVariable*>(labelPred->_rhs)))) {
      return 0;
    }

    const parser::LabelVariable* var = 0;
    const parser::LabelConstant* value = 0;
    if (dynamic_cast<const parser::LabelVariable*>(labelPred->_lhs)) {
      var = dynamic_cast<const parser::LabelVariable*>(labelPred->_lhs);
      value = dynamic_cast<const parser::LabelConstant*>(labelPred->_rhs);
    }
    else {
      var = dynamic_cast<const parser::LabelVariable*>(labelPred->_rhs);
      value = dynamic_cast<const parser::LabelConstant*>(labelPred->_lhs);
    }

    Q_ASSERT(var && value); // unnecessary, but just to make sure

    // look whether we have an index on that descriptor, and if yes:
    // - get either the corresponding subspace (type == EQ) or all of them except
    //   this one (type == NEQ)
    // - returns this subspace
    QString fullName = _dataset->referenceDataSet()->layout().fullName(var->name());
    if (!hasIndex(fullName)) return 0;

    const LabelIndex& index = _labelIndex.value(fullName);
    Filter* alwaysTrue = Filter::parse("where true");
    SearchSpaceType* result = BaseSearchSpacePool<SearchPointType, DataSetType>::acquire();
    result->setFilter(alwaysTrue, true);

    switch (labelPred->_type) {
    case EQ: {
      if (!index.ranges.contains(value->value())) return result;
      QPair<int, int> rng = index.ranges.value(value->value());
      copySearchPoints(result, index.sspace, rng.first, rng.second);
      return result;
    }

    case NEQ:
      // if the constant value is not in our list of possible values, it means all the points are valid
      if (!index.ranges.contains(value->value())) {
        BaseSearchSpacePool<SearchPointType, DataSetType>::release(result);
        return getAllPoints();
      }

      // otherwise, copy all the subspaces for which the value is different than the given one
      result->reserve(index.sspace->size());
      for (GaiaMap<QString, QPair<int, int> >::const_iterator it = index.ranges.constBegin();
           it != index.ranges.constEnd(); ++it) {
        if (value->value() != it.key()) {
          const QPair<int, int>& rng = it.value();
          int previous = result->size();
          result->resize(previous + (rng.second-rng.first));
          copySearchPoints(result, index.sspace, rng.first, rng.second, previous);
        }
      }
      // we need to sort by pointer order here, as we might reuse this subspace for another filter
      result->pointerSort();

      return result;
    }

    Q_ASSERT(false);
  }

  return 0;
}



template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseSearchSpacePool<SearchPointType, DataSetType>::acquire() {
  if (!_sspoolMutex) _sspoolMutex = new QMutex();
  QMutexLocker lock(_sspoolMutex);

  if (_sspool.isEmpty()) {
    G_DEBUG(GSearchSpace, "Creating new SearchSpace because pool is empty");
    return new SearchSpaceType();
  }

  G_DEBUG(GSearchSpace, "Pool has " << _sspool.size() << " ready SearchSpaces, getting one from there");
  SearchSpaceType* result = _sspool.takeLast();
  // need to revalidate here because we don't want to hand out a searchspace that the optimizer
  // or the view thinks it is already filtered & sorted
  result->invalidate();
  return result;
}


template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::release(SearchSpaceType* sspace) {
  if (!sspace) return;

  if (!_sspoolMutex) _sspoolMutex = new QMutex();
  QMutexLocker lock(_sspoolMutex);

  // optimization: only keep it if it has a certain number of points preallocated
  // using capacity() here doesn't cut it, because when we resize, it still needs to construct
  // all the new elements, even if they were previously allocated, and there's no performance
  // gain in doing that...
  if (sspace->unfilteredSize() > 50000) {
    sspace->cleanSearchSpace();
    _sspool << sspace;
    G_DEBUG(GSearchSpace, "Returning SearchSpace to pool, now has " << _sspool.size() << " available ones");
  }
  else {
    G_DEBUG(GSearchSpace, "SearchSpace is too small to return to pool, deleting it (pool has " << _sspool.size() << " available ones)");
    delete sspace;
    return;
  }

  // make sure our pool doesn't grow too big
  static const int MAX_SIZE = 10;

  while (_sspool.size() > MAX_SIZE) {
    delete _sspool.takeLast();
  }
}

template <typename SearchPointType, typename DataSetType>
void BaseSearchSpacePool<SearchPointType, DataSetType>::clear() {
  {
    QMutexLocker lock(_sspoolMutex);
    while (!_sspool.isEmpty()) delete _sspool.takeLast();
  }
  delete _sspoolMutex;
  _sspoolMutex = 0;
}


template <typename SearchPointType, typename DataSetType>
SearchSpaceWrapper<SearchPointType, DataSetType>::~SearchSpaceWrapper() {
  BaseSearchSpacePool<SearchPointType, DataSetType>::release(sspace);
}
