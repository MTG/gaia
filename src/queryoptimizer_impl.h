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

#include "queryoptimizer.h"
#include "parser/parsertypes.h"

namespace gaia2 {

/* Note: here's the parser types hierarchy:

Predicate
 |
 |- PredValueComparison (uses 2 Values)
 |- PredValueRange      (uses 1 Value)
 |- PredLabelComparison (uses 2 Labels)
 |- PredLogicalOp       (uses 2 Predicates)
 |- PredNegate          (uses 1 Predicate)
 |- BooleanConstant
 |- PredValueIsIn       (uses 1 Value)
 \- PredLabelIsIn       (uses 1 Label)

Value
 |
 |- ValueConstant
 \- ValueVariable

Label
 |
 |- LabelConstant
 |- LabelVariable
 \- LabelPointID

*/


template <typename SearchPointType, typename DataSetType>
parser::PredLabelIsIn* BaseQueryOptimizer<SearchPointType, DataSetType>::findPointID(parser::Predicate* pred) {

  // if pred it is, return it
  parser::PredLabelIsIn* labelPred = dynamic_cast<parser::PredLabelIsIn*>(pred);
  if (labelPred && dynamic_cast<parser::LabelPointID*>(labelPred->_var)) {
    return labelPred;
  }

  // predicate is a negation, recurse down on inner predicate
  parser::PredNegate* negPred = dynamic_cast<parser::PredNegate*>(pred);
  if (negPred) return findPointID(negPred->_pred);

  // predicate is a logical comparison, recurse down on inner predicates
  parser::PredLogicalOp* logicPred = dynamic_cast<parser::PredLogicalOp*>(pred);
  if (logicPred) {
    parser::PredLabelIsIn* labelPred = findPointID(logicPred->_lhs);
    if (labelPred) return labelPred;
    return findPointID(logicPred->_rhs);
  }

  // didn't find anything, return 0
  return 0;
}

template <typename SearchPointType, typename DataSetType>
template <typename PredicateType>
PredicateType* BaseQueryOptimizer<SearchPointType, DataSetType>::findPredicate(parser::Predicate* pred) {
  // if we found it, return it
  PredicateType* result = dynamic_cast<PredicateType*>(pred);
  if (result) return result;

  // predicate is a negation, recurse down on inner predicate
  parser::PredNegate* negPred = dynamic_cast<parser::PredNegate*>(pred);
  if (negPred) return findPredicate<PredicateType>(negPred->_pred);

  // predicate is a logical comparison, recurse down on inner predicates
  parser::PredLogicalOp* logicPred = dynamic_cast<parser::PredLogicalOp*>(pred);
  if (logicPred) {
    result = findPredicate<PredicateType>(logicPred->_lhs);
    if (result) return result;
    return findPredicate<PredicateType>(logicPred->_rhs);
  }

  // didn't find anything, return 0
  return 0;
}


template <typename SearchPointType, typename DataSetType>
bool BaseQueryOptimizer<SearchPointType, DataSetType>::swapPredicate(Filter* filter,
                                                                     parser::Predicate* swapOut,
                                                                     parser::Predicate* swapIn) {
  if (filter->_pred == swapOut) {
    filter->_pred = swapIn;
    return true;
  }

  return swapPredicate(filter->_pred, swapOut, swapIn);
}

template <typename SearchPointType, typename DataSetType>
bool BaseQueryOptimizer<SearchPointType, DataSetType>::swapPredicate(parser::Predicate* parent,
                                                                     parser::Predicate* swapOut,
                                                                     parser::Predicate* swapIn) {
  // predicate is a negation, recurse down on inner predicate
  parser::PredNegate* negPred = dynamic_cast<parser::PredNegate*>(parent);
  if (negPred) {
    if (negPred->_pred == swapOut) {
      negPred->_pred = swapIn;
      return true;
    }

    return swapPredicate(negPred->_pred, swapOut, swapIn);
  }

  // predicate is a logical comparison, recurse down on inner predicates
  parser::PredLogicalOp* logicPred = dynamic_cast<parser::PredLogicalOp*>(parent);
  if (logicPred) {
    if (logicPred->_lhs == swapOut) { logicPred->_lhs = swapIn; return true; }
    if (logicPred->_rhs == swapOut) { logicPred->_rhs = swapIn; return true; }

    return swapPredicate(logicPred->_lhs, swapOut, swapIn) ||
           swapPredicate(logicPred->_rhs, swapOut, swapIn);
  }

  return false;
}

template <typename SearchPointType, typename DataSetType>
void BaseQueryOptimizer<SearchPointType, DataSetType>::deletePredicate(Filter* filter, parser::Predicate* pred) {
  if (swapPredicate(filter, pred, new parser::BooleanConstant(true))) {
    delete pred;
  }
}

template <typename SearchPointType, typename DataSetType>
void BaseQueryOptimizer<SearchPointType, DataSetType>::reduceFilter(Filter* filter) {
  parser::Predicate* reduced, *finalPred = filter->_pred;
  while ((reduced = reducePredicate(finalPred))) finalPred = reduced;

  filter->_pred = finalPred;

  G_DEBUG(GParser, "optimized filter: " << finalPred->toString());
}

template <typename SearchPointType, typename DataSetType>
parser::Predicate* BaseQueryOptimizer<SearchPointType, DataSetType>::reducePredicate(parser::Predicate* pred) {
  parser::PredNegate* negPred = dynamic_cast<parser::PredNegate*>(pred);

  // not not X -> X
  if (negPred && dynamic_cast<parser::PredNegate*>(negPred->_pred)) {
    parser::PredNegate* dblNeg = (parser::PredNegate*)(negPred->_pred);
    parser::Predicate* result = dblNeg->_pred;
    dblNeg->_pred = 0; // make sure we don't delete it twice!
    delete pred;
    return result;
  }

  // not true -> false, not false -> true
  if (negPred && dynamic_cast<parser::BooleanConstant*>(negPred->_pred)) {
    parser::Predicate* result = new parser::BooleanConstant(!((parser::BooleanConstant*)negPred->_pred)->value());
    delete pred;
    return result;
  }

  // not X -> not (reduce(X))
  if (negPred) {
    parser::Predicate* result = reducePredicate(negPred->_pred);
    if (!result) return 0;

    negPred->_pred = result;
    return pred;
  }


  parser::PredLogicalOp* logicPred = dynamic_cast<parser::PredLogicalOp*>(pred);

  // True AND X -> X,    False AND X -> False,
  // True OR  X -> True, False OR  X -> X
  if (logicPred && dynamic_cast<parser::BooleanConstant*>(logicPred->_lhs)) {
    parser::Predicate* result = 0;
    bool lvalue = ((parser::BooleanConstant*)logicPred->_lhs)->value();
    switch (logicPred->_op) {

    case AND:
      if (lvalue == true) { result = logicPred->_rhs; logicPred->_rhs = 0; }
      else                { result = new parser::BooleanConstant(false); }
      break;

    case OR:
      if (lvalue == true) { result = new parser::BooleanConstant(true); }
      else                { result = logicPred->_rhs; logicPred->_rhs = 0; }
      break;
    }

    if (result) delete logicPred;
    return result;
  }

  // X AND True -> X,    X AND False -> False,
  // X OR  True -> True, X OR  False -> X
  if (logicPred && dynamic_cast<parser::BooleanConstant*>(logicPred->_rhs)) {
    parser::Predicate* result = 0;
    bool rvalue = ((parser::BooleanConstant*)logicPred->_rhs)->value();
    switch (logicPred->_op) {

    case AND:
      if (rvalue == true) { result = logicPred->_lhs; logicPred->_lhs = 0; }
      else                { result = new parser::BooleanConstant(false); }
      break;

    case OR:
      if (rvalue == true) { result = new parser::BooleanConstant(true); }
      else                { result = logicPred->_lhs; logicPred->_lhs = 0; }
      break;
    }

    if (result) delete logicPred;
    return result;
  }

  // NOT (A OR NOT B) -> NOT A AND (NOT NOT) B (spare 1 negation)
  // and all variants of it... let's do it later if it's really worth it...

  // X AND Y -> reduce(X) AND reduce(Y), X OR Y -> reduce(X) OR reduce(Y)
  if (logicPred) {
    parser::Predicate* lhs = reducePredicate(logicPred->_lhs);
    parser::Predicate* rhs = reducePredicate(logicPred->_rhs);

    if (!lhs && !rhs) return 0;

    if (lhs) logicPred->_lhs = lhs;
    if (rhs) logicPred->_rhs = rhs;

    return pred;
  }

  return 0;
}


template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseQueryOptimizer<SearchPointType, DataSetType>::optimize(Filter* filter) {
  G_DEBUG(GParser, "optimizing filter: " << filter->_pred->toString());
  // first reduce the filter, to make sure there's no obvious optimization that
  // we might miss, or to avoid much more complexity than what's necessary in
  // the optimize function
  reduceFilter(filter);

  SearchSpaceType* result = optimize(filter->_pred);

  if (!result) {
    // we could not optimize the filter, just return the full search space with
    // the original filter
    result = _spool->getAllPoints();
    result->setFilter(filter, true);
  }
  else {
    // optimization worked, we have a new search space with associated filter
    // we can safely delete the old one;
    delete filter;
  }

  return result;
}

template <typename SearchPointType, typename DataSetType>
BaseSearchSpace<SearchPointType, DataSetType>* BaseQueryOptimizer<SearchPointType, DataSetType>::optimize(parser::Predicate* pred) {
  G_DEBUG(GParser, "optimizing predicate:" << pred->toString());

  if (dynamic_cast<parser::PredNegate*>(pred)) return 0;

  // Indexed subspaces: value <= x, < x, > x, >= x, ...
  //                    label == "s", != "s", ...
  if (dynamic_cast<parser::PredValueComparison*>(pred) ||
      dynamic_cast<parser::PredLabelComparison*>(pred) ||
      dynamic_cast<parser::PredValueRange*>(pred)) {
    return _spool->getSubSpace(pred);
  }

  // Logical operator: A OR B, A AND B
  parser::PredLogicalOp* logicPred = dynamic_cast<parser::PredLogicalOp*>(pred);
  if (logicPred) {
    SearchSpaceType* lhs = optimize(logicPred->_lhs);
    if (!lhs || !isAlwaysTrue(lhs->_filter->_pred)) return 0;
    SearchSpaceType* rhs = optimize(logicPred->_rhs);
    if (!rhs || !isAlwaysTrue(rhs->_filter->_pred)) return 0;

    switch (logicPred->_op) {
    case AND:
      lhs->setIntersection(rhs);
      break;
    case OR:
      lhs->setUnion(rhs);
      break;
    }

    delete rhs;
    return lhs;
  }

  // Boolean constant
  if (isAlwaysTrue(pred)) return _spool->getAllPoints();
  if (isAlwaysFalse(pred)) {
    SearchSpaceType* result = new SearchSpaceType();
    Filter* alwaysFalse = Filter::parse("where false");
    result->setFilter(alwaysFalse);
    return result;
  }

  // Point.id in (...)
  parser::PredLabelIsIn* labelPred = dynamic_cast<parser::PredLabelIsIn*>(pred);
  if (labelPred) {
    if (!dynamic_cast<parser::LabelPointID*>(labelPred->_var)) {
      return 0; // not implemented at the moment
    }

    return SearchSpaceType::createFromPoints(_spool->dataset(), labelPred->_slist);

    /*
    const DataSetType* dataset = _spool->dataset();
    int npoints = labelPred->_slist.size();
    SearchSpaceType* result = BaseSearchSpacePool<SearchPointType, DataSetType>::acquire();
    result->setFilter(Filter::parse("where true"), true);
    result->resize(npoints);

    for (int i=0; i<npoints; i++) {
      try {
        (*result)[i].ptr = dataset->point(labelPred->_slist[i]);
        (*result)[i].ref = refDataSet->point(labelPred->_slist[i]);
      }
      catch (GaiaException&) {
        npoints--; i--;
        result->resize(npoints);
      }
    }

    return result;
    */
  }

  if (dynamic_cast<parser::PredValueIsIn*>(pred)) {
    //G_WARNING("QueryOptimizer: 'Value is in' not implemented at the moment");
    return 0; // not implemented at the moment
  }

  return 0;
}

} // namespace gaia2
