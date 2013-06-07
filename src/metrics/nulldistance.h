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
