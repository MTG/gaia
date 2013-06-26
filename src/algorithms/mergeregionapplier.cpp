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

#include "mergeregionapplier.h"
#include "../utils.h"
using namespace gaia2;

MergeRegionApplier::~MergeRegionApplier() {}

MergeRegionApplier::MergeRegionApplier(const Transformation& transfo) : Applier(transfo) {
  _select = transfo.params.value("descriptorNames").toStringList();
  _resultName = transfo.params.value("resultName").toString();
  _layout.add(_resultName, RealType);

  _regionCache = transfo.layout.descriptorLocation(_select);
}


Point* MergeRegionApplier::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  return mergePoint(p, _regionCache);
}

/**
 * @todo assert final merged point has the same size as the specified dimension.
 */
Point* MergeRegionApplier::mergePoint(const Point* p, const Region& region) const {
  Point* result = new Point();
  result->setName(p->name());
  result->setLayout(_layout, p->numberSegments());

  FORSEG(*result) {
    result->setValue(nseg, _resultName, mergeDescriptors(p, nseg, region));
  }

  return result;
}

