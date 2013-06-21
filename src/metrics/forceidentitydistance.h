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
#ifndef GAIA_FORCEIDENTITY_H
#define GAIA_FORCEIDENTITY_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This metric forces the identity condition to be valid on the composed distance
 * by checking first if the 2 points are equal and returning 0 if that is the case.
 * Otherwise, it just returns the value of the composed distance.
 *
 * @b NB: Equality is decided using the name of the points, not their contents.
 *
 * @param distance the name of the distance to be compressed.
 * @param params the parameters for creating the distance to be compressed
 */
class ForceIdentityDistance : public DistanceFunction {
 public:
  ForceIdentityDistance(const PointLayout& layout, const ParameterMap& params);
  ~ForceIdentityDistance();

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  DistanceFunction* _dist;
};

} // namespace gaia2

#endif // GAIA_FORCEIDENTITY_H
