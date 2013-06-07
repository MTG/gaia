#ifndef GAIA_RESISTORAVERAGEDISTANCE_H
#define GAIA_RESISTORAVERAGEDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the symmetric resistor-average distance of a given Region.
 * This distance is based on the harmonic mean of the Kullback-Leibler divergence.
 * More details can be found in the "Symmetrizing the Kullback-Leibler Distance"
 * 2001 paper by Don H. Johnson and Sinan Sinanović.
 *
 * @b NB: compared to the Kullback-Leibler distance, this one also needs the
 *        determinant of the covariance matrix to be precomputed.
 *
 * @param descriptorName the name of the descriptor for which you want to compute
 *        the KL-distance. This descriptor needs to have 4 sub-descriptors:
 *        mean, cov, icov and covdet. (representing, resp., the mean of the frames, the
 *        covariance matrix, the inverse of the covariance matrix and the determinant
 *        of the covariance matrix).
 */
class ResistorAverageDistance : public DistanceFunction {
 public:
  ResistorAverageDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  int _meanIdx, _covIdx, _icovIdx, _covdetIdx;
  int _meanIdxEnd, _covIdxEnd, _icovIdxEnd;
};

} // namespace gaia2

#endif // GAIA_RESISTORAVERAGEDISTANCE_H
