#ifndef GAIA_EXPONENTIALCOMPRESS_H
#define GAIA_EXPONENTIALCOMPRESS_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This class compresses a distance from [0; inf) into [0; 1) using the following
 * formula: dist = 1 - exp(-alpha*dist), alpha being a compression factor.
 *
 * @param distance the name of the distance to be compressed.
 * @param params the parameters for creating the distance to be compressed.
 * @param alpha the compression factor.
 */
class ExponentialCompressDistance : public DistanceFunction {
 public:
  ExponentialCompressDistance(const PointLayout& layout, const ParameterMap& params);
  ~ExponentialCompressDistance();

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  DistanceFunction* _dist;
  Real _alpha;
};

} // namespace gaia2

#endif // GAIA_EXPONENTIALCOMPRESS_H
