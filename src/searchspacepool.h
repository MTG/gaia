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
#ifndef GAIA_SEARCHSPACEPOOL_H
#define GAIA_SEARCHSPACEPOOL_H

#include <QMutex>
#include "searchspace.h"

namespace gaia2 {

namespace parser {
  class Predicate;
}


/**
 * The SearchSpacePool accomplishes 2 main functions:
 *  - it holds references to precomputed SearchSpaces which correspond to indexed
 *    descriptors
 *  - it acts as a pool of preallocated SearchSpaces, to avoid the cost of
 *    creating / deleting them each time.
 */
template <typename SearchPointType, typename DataSetType>
class BaseSearchSpacePool {

 public:
  typedef BaseSearchSpace<SearchPointType, DataSetType> SearchSpaceType;

  /**
   * By default, the SearchSpacePool constructor takes a DataSet which it will use
   * for creating indices. It is necessary to call the indexOn() method explicitly,
   * though.
   */
  BaseSearchSpacePool(const DataSetType* dataset);

  ~BaseSearchSpacePool();

  /**
   * Recreates the SearchSpacePool and all the indices (needed for instance when
   * the dataset it is pointing to has been changed).
   */
  void recreate();

  /**
   * Returns a SearchSpace containing all points.
   * FIXME: implementation needs to get a space from ::acquire() instead of creating a new one each time
   */
  SearchSpaceType* getAllPoints() const;

  /**
   * Indexes on the given descriptorName. Queries which then use this descriptor
   * should be faster.
   */
  void indexOn(const QString& descriptorName);

  /**
   * Returns whether an index exists for the given descriptorName.
   */
  bool hasIndex(const QString& descriptorName) const;

  SearchSpaceType* getSubSpace(const parser::Predicate* pred) const;

  const DataSetType* dataset() const { return _dataset; }

  /**
   * Clear the cache of preallocated SearchSpaces.
   */
  static void clear();

  static SearchSpaceType* acquire();
  static void release(SearchSpaceType* sspace);

 protected:
  static QList<SearchSpaceType*> _sspool;
  static QMutex* _sspoolMutex;

  friend class BaseQueryOptimizer<SearchPointType, DataSetType>;

  const DataSetType* _dataset;

  SearchSpaceType* _originalSpace;

  class ValueIndex {
   public:
    GaiaMap<QPair<int, int>, SearchSpaceType*> sorted;  // key is <start,end> for ranges
    SearchSpaceType* sspace;

    void clear() {
      foreach (const SearchSpaceType* sspace, sorted) delete sspace;
      sorted.clear();
      delete sspace; sspace = 0;
    }

    SearchSpaceType* getSubSpaceInto(SearchSpaceType* result,
                                     int start, int end) const;
  };

  GaiaMap<QString, ValueIndex> _valueIndex; // from descriptor name to ordered sspace

  class LabelIndex {
   public:
    GaiaMap<QString, QPair<int, int> > ranges; // from label value to (start, end) range inside sspace
    SearchSpaceType* sspace;

    void clear() { delete sspace; sspace = 0; ranges.clear(); }
  };

  GaiaMap<QString, LabelIndex> _labelIndex; // from descriptor name to label index


  void clearIndices();
  int getCuttingPos(const SearchSpaceType& sspace,
                    int idx, int type, Real value, int start, int end) const;

  /** assumes descriptorName is the fully qualified name */
  void indexOnValue(const QString& descriptorName);

  /** assumes descriptorName is the fully qualified name */
  void indexOnLabel(const QString& descriptorName);

  /** assumes descriptorName is the fully qualified name */
  void indexOnEnum(const QString& descriptorName);

};


// include implementation directly as our class is a templated one
#include "searchspacepool_impl.h"


typedef BaseSearchSpacePool<SearchPoint, DataSet> SearchSpacePool;
typedef BaseSearchSpacePool<FrozenSearchPoint, FrozenDataSet> FrozenSearchSpacePool;

} // namespace gaia2


#endif // GAIA_SEARCHSPACEPOOL_H
