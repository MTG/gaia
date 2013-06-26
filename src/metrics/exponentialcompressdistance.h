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

#ifndef GAIA_EXPONENTIALCOMPRESS_H
#define GAIA_EXPONENTIALCOMPRESS_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This class compresses a distance from [0; inf) into [0; 1) using the following
 * formula: dist = 1 - exp(-alpha*dist), alpha being a compression factor.
 *
 * @param distance the name of the distance to be compressed.
 * @param params the parameters for creating the distance to be compressed.
 * @param alpha the compression factor.
 */
class ExponentialCompressDistance : public DistanceFunction {
 public:
  ExponentialCompressDistance(const PointLayout& layout, const ParameterMap& params);
  ~ExponentialCompressDistance();

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  DistanceFunction* _dist;
  Real _alpha;
};

} // namespace gaia2

#endif // GAIA_EXPONENTIALCOMPRESS_H
