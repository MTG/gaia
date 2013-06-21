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
#ifndef GAIA_QUERYOPTIMIZER_H
#define GAIA_QUERYOPTIMIZER_H

#include "dataset.h"
#include "searchspacepool.h"
#include "parser/filter.h"
#include "parser/parsertypes.h"

namespace gaia2 {

/**
 * The QueryOptimizer class tries to optimize a query by reducing the SearchSpace
 * on which it is supposed to operate, mostly by the means of the associated
 * filter. It should tap into a SearchSpace pool and return the final SearchSpace,
 * as well as the possibly modified Filter (contained inside the SearchSpace), on
 * which to perform the query.
 */
template <typename SearchPointType, typename DataSetType>
class BaseQueryOptimizer {

 public:

  typedef BaseSearchSpacePool<SearchPointType, DataSetType> SearchSpacePoolType;
  typedef BaseSearchSpace<SearchPointType, DataSetType> SearchSpaceType;

  BaseQueryOptimizer(SearchSpacePoolType* spool) : _spool(spool) {}

  /**
   * Ownership of the SearchSpace is yielded to the caller of the function.
   * Ownership of the filter is given to this function (either it will be destroyed
   * and replaced by an optimized filter inside the SearchSpace, or it will put
   * directly into the SearchSpace. In any case, you shouldn't try to delete it after
   * having called this function).
   */
  SearchSpaceType* optimize(Filter* filter);


 protected:
  SearchSpacePoolType* _spool;

  SearchSpaceType* optimize(parser::Predicate* pred);


  static parser::PredLabelIsIn* findPointID(parser::Predicate* pred);

  template <typename PredicateType>
  static PredicateType* findPredicate(parser::Predicate* pred);

  /**
   * Swaps predicate swapOut inside of filter for predicate swapIn.
   * @return true if it succeeded, false if we couldn't find swapOut inside of filter.
   */
  static bool swapPredicate(Filter* filter, parser::Predicate* swapOut, parser::Predicate* swapIn);

  /**
   * Swaps predicate swapOut (starting with given parent predicate) for predicate swapIn.
   * @return true if it succeeded, false if we couldn't find swapOut.
   */
  static bool swapPredicate(parser::Predicate* parent, parser::Predicate* swapOut, parser::Predicate* swapIn);

  /**
   * Deletes given predicate inside filter (replaces it with the always true predicate).
   */
  static void deletePredicate(Filter* filter, parser::Predicate* pred);

  /**
   * Tries to apply simplification on the filter, such as replacing
   * "X and True" by "X", etc... without changing the return value of it.
   * The filter is modified in-place.
   */
  static void reduceFilter(Filter* filter);

  /**
   * Tries to apply simplification on the predicate, such as replacing
   * "X and True" by "X", etc... without changing the return value of it.
   * @return the modified predicate if any simplification could be made, null pointer otherwise.
   */
  static parser::Predicate* reducePredicate(parser::Predicate* pred);


};

typedef BaseQueryOptimizer<SearchPoint, DataSet> QueryOptimizer;

} // namespace gaia2

// we need to include our implementation here as we have a templated class
#include "queryoptimizer_impl.h"

#endif // GAIA_QUERYOPTIMIZER_H
