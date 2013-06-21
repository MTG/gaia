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
#ifndef GAIA_NULLDISTANCE_H
#define GAIA_NULLDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics refmetrics
 *
 * This class computes a distance which is always equal to 0.
 */
class NullDistance : public DistanceFunction {
 public:
  NullDistance(const PointLayout& layout, const ParameterMap& params) : DistanceFunction(layout, params) {}

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
    Q_UNUSED(p1); Q_UNUSED(p2);
    Q_UNUSED(seg1); Q_UNUSED(seg2);
    return 0.0;
  }
};

typedef NullDistance LabelDistance;

} // namespace gaia2

#endif // GAIA_NULLDISTANCE_H
