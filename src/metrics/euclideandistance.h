#ifndef GAIA_EUCLIDEANDISTANCE_H
#define GAIA_EUCLIDEANDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the euclidean distance of a given Region.
 *
 * @param descriptorNames the names of the descriptors to be included in the
 *        computation of the distance. Only accepts real descriptors, can use
 *        wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards.
 */
class EuclideanDistance : public DistanceFunction {
 public:
  EuclideanDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

};

} // namespace gaia2

#endif // GAIA_EUCLIDEANDISTANCE_H
