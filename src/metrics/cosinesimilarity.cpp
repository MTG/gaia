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

#include "cosinesimilarity.h"
#include "../utils.h"
using namespace gaia2;

CosineSimilarity::CosineSimilarity(const PointLayout& layout,
                                   const ParameterMap& params) : DistanceFunction(layout, params) {

  validParams << "defaultValue";

  _region = checkFixedLength(_region.select(RealType));

  if (params.contains("defaultValue")) {
    _useDefaultValue = true;
    _defaultValue = params.value("defaultValue").toDouble();
  }
  else {
    _useDefaultValue = false;
  }
}

Real CosineSimilarity::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real dist = 0.0;
  Real norm1 = 0.0, norm2 = 0.0;

  const RealDescriptor& fldescs1 = p1.frealData(seg1);
  const RealDescriptor& fldescs2 = p2.frealData(seg2);

  for (int i=0; i<_region.segments.size(); i++) {
    const Segment& segment = _region.segments[i];
    for (int j=segment.begin; j<segment.end; j++) {
      Real v1 = fldescs1[j];
      Real v2 = fldescs2[j];
      dist += v1*v2;
      norm1 += v1*v1;
      norm2 += v2*v2;
    }
  }

  static const Real epsilon = 1e-12; // arbitrary value
  if (norm1 * norm2 < epsilon) {
    if (_useDefaultValue) {
      return _defaultValue;
    }
    QStringList msg;
    msg << "Could not compute cosine distance between '" << p1.name()
        << "' and '" << p2.name() << "' because: "
        << "one of the two points is null, result is undefined";
    throw GaiaException(msg);
  }

  dist /= sqrt(norm1 * norm2);

  // make sure we don't overflow stupidly because of precision issues...
  dist = clip(dist, -1.0, 1.0);

  return dist;
}
