#ifndef GAIA_COSINESIMILARITY_H
#define GAIA_COSINESIMILARITY_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the cosine similarity of a given Region.
 *
 * @param descriptorNames the names of the descriptors to be included in the
 *        computation of the distance. Only accepts real descriptors, can use
 *        wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards.
 * @param defaultValue when 1 of the 2 points is null, then the similarity is not
 *        defined. If defaultValue is given, then it will be returned,
 *        otherwise an exception will be thrown.
 */
class CosineSimilarity : public DistanceFunction {
 public:
  CosineSimilarity(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  bool _useDefaultValue;
  Real _defaultValue;
};

} // namespace gaia2

#endif // GAIA_COSINESIMILARITY_H
