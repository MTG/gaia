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
#ifndef GAIA_MINKOWSKIDISTANCE_H
#define GAIA_MINKOWSKIDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {
/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the Minkowski distance of a Region. It is defined as:
 * \f$ \left( \sum_{i} (a_i - b_i)^x \right) ^ {1/x}\f$ , for a given x.
 *
 * @param exponent the exponent for this distance (the 'x' in the formula)
 * <b>NB:</b> This class is not implemented at the moment.
 * @todo implement this with LUT to have efficient distances
 */
class MinkowskiDistance : public DistanceFunction {
 public:
  MinkowskiDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

};

} // namespace gaia2

#endif // GAIA_MINKOWSKIDISTANCE_H
