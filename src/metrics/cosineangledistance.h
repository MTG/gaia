#ifndef GAIA_COSINEANGLEDISTANCE_H
#define GAIA_COSINEANGLEDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

class CosineSimilarity;

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the angle in radians between 2 points and divides it
 * by Pi (so that it's normalized between 0 and 1).
 *
 * @param descriptorNames the names of the descriptors to be included in the
 *        computation of the distance. Only accepts real descriptors, can use
 *        wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards.
 * @param defaultValue when 1 of the 2 points is null, then the angle is not
 *        defined. If defaultValue is given, then it will be returned,
 *        otherwise an exception will be thrown.
 */
class CosineAngleDistance : public DistanceFunction {
 public:
  CosineAngleDistance(const PointLayout& layout, const ParameterMap& params);
  ~CosineAngleDistance();

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  CosineSimilarity* _cosDist;
};

} // namespace gaia2

#endif // GAIA_COSINEANGLEDISTANCE_H
