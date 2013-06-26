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

#ifndef GAIA_SEARCHSPACE_H
#define GAIA_SEARCHSPACE_H

#include <vector>
#include "dataset.h"
#include "frozendataset.h"
#include "parser/filter.h"


namespace gaia2 {


typedef QPair<QString, Real> Result;
typedef QList<Result> SearchResults;

yaml::Node toYaml(const SearchResults& results);

// fwd-declaration
class DistanceFunction;
class FrozenDistance;

#define Vector std::vector


class SearchPoint {
public:
  float dist;
  const Point* ptr;
  const Point* ref;

  // should never use the default constructor, it is defined because std::vector::resize() needs it
  SearchPoint() {}
  SearchPoint(const Point* pptr, const Point* rref) : ptr(pptr), ref(rref) {}
  bool operator<(const SearchPoint& x) const { return dist < x.dist; }
};


class FrozenSearchPoint {
public:
  float dist;
  int idx;

  // should never use the default constructor, it is defined because std::vector::resize() needs it
  FrozenSearchPoint() {}
  FrozenSearchPoint(int iidx) : idx(iidx) {}
  bool operator<(const FrozenSearchPoint& x) const { return dist < x.dist; }
};


/**
 * A SearchSpace is a structure dedicated to the task of storing pointers to
 * Point with an associated distance and is the structure on which the
 * nearest-neighbours queries are performed.
 *
 * It is also what is returned by the view as a search result, and allows to
 * get any number of points with a given offset if desired.
 *
 * Some pre-indexed SearchSpace instances will reside in a SearchSpace pool which
 * is owned by a specific View. It acts as an index, and the pool has a way to
 * either add more of those, or return a given subset that satisfies certain
 * conditions.
 * As such, when a View needs to be revalidated, it will also revalidate all
 * linked SearchSpace.
 *
 * @b WARNING: As we make copies of SearchSpace to perform queries, it is perfectly
 * safe to make multithreaded queries. However, it is NOT safe to add or remove
 * points while there is still a query being processed, or a SearchSpace returned
 * from a previous query still in use (ie: View instances revalidate themselves
 * automatically when a DataSet changes, but SearchSpace ones don't)
 */
template <typename SearchPointType, typename DataSetType>
class BaseSearchSpace : public Vector<SearchPointType> {

 public:
  BaseSearchSpace(const Filter* filter = 0, bool ownsFilter = false);

  /**
   * Copy constructor. WARNING: this does not copy the filter nor any other member
   * variables, only the points contained in the SearchSpace.
   */
  BaseSearchSpace(const BaseSearchSpace<SearchPointType, DataSetType>& other);

  ~BaseSearchSpace();

  /**
   * Copy all the points from the other SearchSpace into this one.
   * WARNING: this does not copy the filter nor any other member variables, only
   * the points contained in the SearchSpace.
   */
  void copyPointsFrom(const BaseSearchSpace<SearchPointType, DataSetType>& other);

  /**
   * Returns the total number of points contained in this SearchSpace.
   */
  int size() const;

  /**
   * Returns the size of this SearchSpace, before filtering the points.
   * WARNING: use with care, this value might not be consistent with what you expect.
   */
  int unfilteredSize() const;

  /**
   * Clears the whole SearchSpace.
   */
  void clear();

  /**
   * Returns the list of search results, which are pairs of (pointName, distance).
   * @param n the number of desired results
   * @param offset the start offset
   */
  SearchResults get(int n, int offset = 0);

  /**
   * This method limits the number of results contained in this SearchSpace.
   * If there were fewer points before than the given number, the SearchSpace is
   * left untouched, otherwise it will have a maximum of n points after the call.
   */
  void limit(int n);

  /**
   * This method limits the bymber of results contained in this SearchSpace.
   * All the results which have a distance higher than the specified maxDist will
   * be discarded from this SearchSpace.
   */
  void thresholdLimit(float maxDist);

  const Filter* filter() { return _filter; }
  void setFilter(const Filter* filter, bool ownsFilter = false);

  /**
   * Mark this SearchSpace as neither sorted nor filtered.
   */
  void invalidate() { _sortedUpTo = _filteredUpTo = 0; }

  /**
   * Filters all points so that the remaining points at the end all comply to
   * the Filter.
   */
  void filterAll();

  /**
   * Sorts the SearchPoints contained into this SearchSpace by order of their
   * pointer address.
   */
  void pointerSort();

  /**
   * Add the points from the dataset with the given IDs to this SearchSpace.
   */
  void addPoints(const DataSetType* dataset, const QList<QString>& ids);

  /**
   * Remove the points with the given IDs from this SearchSpace.
   */
  void removePoints(const QList<QString>& ids);

  /**
   * This computes the intersection of this SearchSpace with the other one.
   * NB: This function assumes that both SearchSpaces have their points ordered
   *     by pointer address.
   */
  void setIntersection(const BaseSearchSpace<SearchPointType, DataSetType>* other);

  /**
   * This computes the union of this SearchSpace with the other one.
   * NB: This function assumes that both SearchSpaces have their points ordered
   *     by pointer address.
   */
  void setUnion(const BaseSearchSpace<SearchPointType, DataSetType>* other);

  /**
   * This computes the difference of this SearchSpace and the other one (ie: this - other).
   * NB: This function assumes that both SearchSpaces have their points ordered
   *     by pointer address.
   */
  void setDifference(const BaseSearchSpace<SearchPointType, DataSetType>* other);

  //////////// need to be implemented by template specialization because virtual
  //////////// funcs don't work here for reasons too long to explain...
  QString pointName(const SearchPointType& p) const;
  const Point* refPoint(const SearchPointType& p) const;

  bool validPoint(const SearchPointType& p) const {
    return _filter->isTrue(refPoint(p));
  }

  static BaseSearchSpace<SearchPointType, DataSetType>* createFromDataSet(const DataSetType* dataset);

  static BaseSearchSpace<SearchPointType, DataSetType>* createFromPoints(const DataSetType* dataset,
                                                                         const QList<QString>& pointNames);

  /**
   * Compute the distance from the given query point to all those contained in
   * this SearchSpace, and presort a few points to have near instant access to
   * them.
   */
  template <typename PointType, typename DistanceType>
  void computeDistance(const PointType& query, const DistanceType* dist);

  //void computeDistance(const FrozenPoint& query, const FrozenDistance* dist);

  //////////// end of need to be implemented by template specialization

  void setDataSet(const DataSetType* dataset) { _dataset = dataset; }
  const DataSetType* dataSet() const { return _dataset; }

  /**
   * Filters the points in this SearchSpace using the given Filter and sort them
   * by increasing distance.
   * @param n only filter and sort the @c n points with the smallest distance,
   *          this has the advantage of being much faster than sorting all the
   *          points. If n <= 0, then all the points will be sorted.
   */
  void filterAndSort(int n = 1000);

  void cleanSearchSpace();

 protected:
  const DataSetType* _dataset;
  const Filter* _filter;
  bool _ownsFilter;
  int _sortedUpTo, _filteredUpTo;

    // FIXME: use the typenames from the search space, not any ones like here
  template <typename SearchPointType2, typename DataSetType2>
  friend class BaseQueryOptimizer;

  /**
   * Index in the search space up to which points are actually filtered and
   * sorted.
   */
  int validUpTo() const { return qMin(_sortedUpTo, _filteredUpTo); }

  void sortAll();

};



////////////// IMPLEMENTATION //////////////////
typedef BaseSearchSpace<SearchPoint, DataSet> SearchSpace;

template <>
inline const Point* SearchSpace::refPoint(const SearchPoint& p) const {
  return p.ref;
}

template <>
inline QString SearchSpace::pointName(const SearchPoint& p) const {
  return p.ptr->name();
}

///////
typedef BaseSearchSpace<FrozenSearchPoint, FrozenDataSet> FrozenSearchSpace;

template <>
inline const Point* FrozenSearchSpace::refPoint(const FrozenSearchPoint& p) const {
  return _dataset->referenceDataSet()->at(p.idx);
}

template <>
inline QString FrozenSearchSpace::pointName(const FrozenSearchPoint& p) const {
  return _dataset->pointName(p.idx);
}
////////////// END OF IMPLEMENTATION //////////////////



template <typename SearchPointType, typename DataSetType>
class SearchSpaceWrapper : public QSharedData {
 public:
  BaseSearchSpace<SearchPointType, DataSetType>* sspace;

  SearchSpaceWrapper(BaseSearchSpace<SearchPointType, DataSetType>* s) : sspace(s) {}
  ~SearchSpaceWrapper(); // defined after SearchSpacePool is defined, at the end of SearchSpacePool_impl.h
};


/**
 * This class serves as a ref-counted wrapper for a SearchSpace, which is much
 * more convenient to deal with than having to remember who owns the results
 * or who's responsible for deleting it.
 */
template <typename SearchPointType, typename DataSetType>
class BaseResultSet {

 public:
  BaseResultSet();
  BaseResultSet(BaseSearchSpace<SearchPointType, DataSetType>* sspace);

  /**
   * Returns the list of search results, which are pairs of (pointName, distance).
   * @param n the number of desired results
   * @param offset the start offset
   */
  SearchResults get(int n, int offset = 0);

  int size() const { return _d->sspace->size(); }

  /**
   * This function limits the number of results contained in this ResultSet.
   * If there were fewer points before than the given number, the ResultSet is
   * left untouched, otherwise it will have a maximum of n points after the call.
   * Returns this object so it is possible to chain this method call with a get() one.
   */
  BaseResultSet limit(int n) { _d->sspace->limit(n); return *this; }

  /**
   * This method limits the number of results contained in this ResultSet.
   * All the results which have a distance higher than the specified maxDist will
   * be discarded from this SearchSpace.
   * Returns this object so it is possible to chain this method call with a get() one.
   */
  BaseResultSet thresholdLimit(float maxDist) { _d->sspace->thresholdLimit(maxDist); return *this; }

#ifndef SWIG
  void addPoints(const DataSetType* dataset, const QList<QString>& ids);
  void removePoints(const QList<QString>& ids);
#endif

  BaseSearchSpace<SearchPointType, DataSetType>* searchSpace() { return _d->sspace; }

 protected:
  QExplicitlySharedDataPointer<SearchSpaceWrapper<SearchPointType, DataSetType> > _d;
};



// as SearchSpace* is a template, we have to import its definition here
#include "searchspace_impl.h"



typedef BaseResultSet<SearchPoint, DataSet> ResultSet;

// Useful typedefs, because sometimes we might want to create such a beast from
// scratch, in which case the name ResultSet is a bit misleading
typedef ResultSet InputSpace;

} // namespace gaia2


#endif // GAIA_SEARCHSPACE_H
