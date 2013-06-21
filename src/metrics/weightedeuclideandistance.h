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
#ifndef GAIA_WEIGHTEDEUCLIDEANDISTANCE_H
#define GAIA_WEIGHTEDEUCLIDEANDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the euclidean distance of a given Region, applying a weight on each descriptor.
 *
 * WARNING: this metric is not final, and it might change its behavior in the future.
 *
 * @param weights a mapping from descriptor name to its weight.
 */
class WeightedEuclideanDistance : public DistanceFunction {
 public:
  WeightedEuclideanDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  Array<DimWeight> _fixedl;
  Real _weightSum;
};

} // namespace gaia2

#endif // GAIA_WEIGHTEDEUCLIDEANDISTANCE_H
