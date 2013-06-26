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

#ifndef GAIA_CROSSSEGMENTDISTANCE_H
#define GAIA_CROSSSEGMENTDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

class DistAggr {
public:
  virtual Real initValue() const { return 0; }
  virtual Real operator()(Real x, Real y) const = 0;
  virtual Real postProcess(Real x, int nsegs) const { return x; }
  virtual ~DistAggr() {}
};

/**
 * @ingroup metrics compositemetrics
 *
 * This class calculates a distance between segments from two points.
 * The function used to calculate the distance between two segments is given
 * by the distance parameter.
 * What it does is the following:
 *  - for a given segment of p1, it computes its distance with all the segments
 *    of p2, then uses @c func1 to aggregate the results.
 *  - it does the previous for all segments of p1, then uses @c func2 to aggregate
 *    the results.
 *
 * WARNING: this metric is not final, and it might change its behavior in the future.
 *
 * @param distance the name of the distance to be composed with.
 * @param params the parameters for creating the distance.
 * @param func1 the first aggregation function (min, max or mean).
 * @param func2 the second aggregation function (min, max or mean).
 */
class CrossSegmentDistance : public DistanceFunction {
 public:
  CrossSegmentDistance(const PointLayout& layout, const ParameterMap& params);
  ~CrossSegmentDistance();
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  DistanceFunction* _dist;
  DistAggr *_aggr1, *_aggr2;
};

} // namespace gaia2

#endif // GAIA_CROSSSEGMENTDISTANCE_H
