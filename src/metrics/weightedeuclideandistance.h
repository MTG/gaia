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
