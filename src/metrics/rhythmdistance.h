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

#ifndef GAIA_RHYTHMDISTANCE_H
#define GAIA_RHYTHMDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the rhythm distance of a given descriptor. It is defined
 * as \f$d_{BPM}(X,Y) = \min_{i=1,2,4,6,...}{\alpha_{BPM}^{i-1}|\frac{X_{BPM}}{Y_{BPM}} - i|}, X_{BPM} > Y_{BPM}\f$ .
 *
 * @param descriptorName the names of the descriptor to be used for the
 *        computation of the distance. Only accepts real descriptors.
 * @param alpha the @c alpha coefficient (from the formula)
 * @param indexRange the list of indices @i upon which to iterate.
 */
class RhythmDistance : public DistanceFunction {
 public:
  RhythmDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  bool _fixedLength;
  int _index;

  Real _alpha;
  QVarLengthArray<Real, 16> _indexRange, _alphaWeights;

  // assumes that x > y
  Real distance(const Real& x, const Real& yi, const QString& t1, const QString& t2) const;
};

} // namespace gaia2

#endif // GAIA_RHYTHMDISTANCE_H
