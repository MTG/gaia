#ifndef GAIA_FORCEIDENTITY_H
#define GAIA_FORCEIDENTITY_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This metric forces the identity condition to be valid on the composed distance
 * by checking first if the 2 points are equal and returning 0 if that is the case.
 * Otherwise, it just returns the value of the composed distance.
 *
 * @b NB: Equality is decided using the name of the points, not their contents.
 *
 * @param distance the name of the distance to be compressed.
 * @param params the parameters for creating the distance to be compressed
 */
class ForceIdentityDistance : public DistanceFunction {
 public:
  ForceIdentityDistance(const PointLayout& layout, const ParameterMap& params);
  ~ForceIdentityDistance();

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  DistanceFunction* _dist;
};

} // namespace gaia2

#endif // GAIA_FORCEIDENTITY_H
