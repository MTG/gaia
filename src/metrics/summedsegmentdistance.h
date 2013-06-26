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

#ifndef GAIA_SUMMEDSEGMENTDISTANCE_H
#define GAIA_SUMMEDSEGMENTDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This class calculates a distance between segments from two points and
 * sums the results.
 * The cross param defines whether only segments at the same position should
 * be compared, or each with each.
 * The function used to calculate the distance between two segments is given
 * by the distance param.
 *
 * WARNING: this metric is not final, and it might change its behavior in the future.
 *
 * @param distance the name of the distance to be compressed.
 * @param params the ParameterMap to be used when creating the specified distance.
 * @param cross compare segments at the same position or each with each
 */

class SummedSegmentDistance : public DistanceFunction {
 public:
  SummedSegmentDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  DistanceFunction* _dist;
  bool _cross;
};

} // namespace gaia2

#endif // GAIA_SUMMEDSEGMENTDISTANCE_H
