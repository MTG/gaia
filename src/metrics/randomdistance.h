#ifndef GAIA_RANDOMDISTANCE_H
#define GAIA_RANDOMDISTANCE_H

#include "distancefunction.h"
#include "3rdparty/MersenneTwister.h"

namespace gaia2 {

/**
 * @ingroup metrics refmetrics
 *
 * This class computes a distance which returns a random number between
 * 0 and the specified range. It uses a uniform distribution.
 *
 * @param range the max value this distance can return (minimum being 0).
 */
class RandomDistance : public DistanceFunction {
 public:
  RandomDistance(const PointLayout&, const ParameterMap&);

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  mutable MTRand _mtrand;
  Real _range;
};

} // namespace gaia2

#endif // GAIA_RANDOMDISTANCE_H
