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

#include "cosineangledistance.h"
#include "cosinesimilarity.h"
#include "../utils.h"
#include <cmath>
using namespace gaia2;

CosineAngleDistance::CosineAngleDistance(const PointLayout& layout,
                                         const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams << "defaultValue";

  ParameterMap cosDistParams(params);
  if (cosDistParams.contains("defaultValue")) {
    cosDistParams.insert("defaultValue", cos(params.value("defaultValue").toDouble() * M_PI));
  }

  _cosDist = new CosineSimilarity(layout, cosDistParams);
}


CosineAngleDistance::~CosineAngleDistance() {
  delete _cosDist;
}


Real CosineAngleDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  return acos((*_cosDist)(p1, p2, seg1, seg2)) / M_PI;
}
