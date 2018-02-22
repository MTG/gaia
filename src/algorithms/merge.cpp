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

#include "merge.h"
#include "../utils.h"
using namespace gaia2;

Merge::~Merge() {}

Merge::Merge(const Transformation& transfo) : Applier(transfo) {
  _history1 = transfo.params.value("history1").toHistory();
  _history2 = transfo.params.value("history2").toHistory();
}


Point* Merge::mapPoint(const Point* p) const {
  Point* p1 = _history1.mapPoint(p);
  Point* p2 = _history2.mapPoint(p);

  Point* result = mergePoints(p1, p2);

  delete p1;
  delete p2;

  return result;
}



/**
 * @todo this could be optimized by copying the code from the mergeDataSets
 * utility function, but it is doubtful this will ever be useful (we shouldn't
 * be calling mapDataSet too often, but rather directly mergeDataSets).
 */
DataSet* Merge::mapDataSet(const DataSet* dataset) const {
  DataSet* ds1 = _history1.mapDataSet(dataset);
  DataSet* ds2 = _history2.mapDataSet(dataset);

  DataSet* result = mergeDataSets(ds1, ds2);

  delete ds1;
  delete ds2;

  return result;
}
