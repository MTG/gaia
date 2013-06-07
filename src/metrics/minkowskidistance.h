#ifndef GAIA_MINKOWSKIDISTANCE_H
#define GAIA_MINKOWSKIDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {
/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the Minkowski distance of a Region. It is defined as:
 * \f$ \left( \sum_{i} (a_i - b_i)^x \right) ^ {1/x}\f$ , for a given x.
 *
 * @param exponent the exponent for this distance (the 'x' in the formula)
 * <b>NB:</b> This class is not implemented at the moment.
 * @todo implement this with LUT to have efficient distances
 */
class MinkowskiDistance : public DistanceFunction {
 public:
  MinkowskiDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

};

} // namespace gaia2

#endif // GAIA_MINKOWSKIDISTANCE_H
