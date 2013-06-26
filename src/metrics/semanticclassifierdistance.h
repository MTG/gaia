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

#ifndef GAIA_SEMANTICCLASSIFIERDISTANCE_H
#define GAIA_SEMANTICCLASSIFIERDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

typedef QPair<int, Real> DimWeight;

/**
 * @ingroup metrics standardmetrics
 *
 * @TODO: desc is wrong, still the wpearson one...
 * This class computes the semantic classifier distance between 2 points. It is defined
 * as \f$ d (X,Y) = \sum{w_i\frac{e^{(x_i-y_i)^{\alpha}}}{\beta+e^{x_i+y_i}}} \f$
 *
 * @alpha the alpha coefficient in the above formula
 * @beta the beta coefficient in the above formula
 * @param weights a mapping from descriptor name to its weight.
 */
class SemanticClassifierDistance : public DistanceFunction {
 public:
  SemanticClassifierDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  Array<DimWeight> _fixedl;
  Real _alpha, _beta;
};

} // namespace gaia2

#endif // GAIA_SEMANTICCLASSIFIERDISTANCE_H
