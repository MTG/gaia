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
