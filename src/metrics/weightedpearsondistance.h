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

#ifndef GAIA_WEIGHTEDPEARSONCORRELATION_H
#define GAIA_WEIGHTEDPEARSONCORRELATION_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the weighted Pearson correlation between 2 points. It is defined
 * as \f$ d (X,Y) = 1-\frac{\sum{w_i(x_i-\bar{x_w})(y_i-\bar{y_w})}}{\sqrt{\sum{w_i(x_i-\bar{x_w})^2}}\sqrt{\sum{w_i(y_i-\bar{y_w})^2}}} \f$
 * with weighted means \f$ \bar{x_w} = \frac{\sum{w_ix_i}}{\sum{w_i}}; \bar{y_w} = \frac{\sum{w_iy_i}}{\sum{w_i}} \f$ .
 *
 * @param weights a mapping from descriptor name to its weight.
 */
class WeightedPearsonDistance : public DistanceFunction {
 public:
  WeightedPearsonDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  Array<DimWeight> _fixedl;
  Real _weightSum;
};

} // namespace gaia2

#endif // GAIA_WEIGHTEDPEARSONCORRELATION_H
