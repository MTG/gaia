#ifndef GAIA_KULLBACKLEIBLERDISTANCE_H
#define GAIA_KULLBACKLEIBLERDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics standardmetrics
 *
 * This class computes the symmetric Kullback-Leibler distance of a given Region.
 *
 * @param descriptorName the name of the descriptor for which you want to compute
 *        the KL-distance. This descriptor needs to have 3 sub-descriptors:
 *        mean, cov, and icov. (representing, resp., the mean of the frames, the
 *        covariance matrix, and the inverse of the covariance matrix.
 */
class KullbackLeiblerDistance : public DistanceFunction {
 public:
  KullbackLeiblerDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  int _meanIdx, _covIdx, _icovIdx;
  int _meanIdxEnd, _covIdxEnd, _icovIdxEnd;
};


/**
 * Compute the trace of the matrix product of m1 and m2.
 * Both matrices need to be squared of dimension @c size.
 */
inline Real traceMatrixProduct(const Real* m1, const Real* m2, int size) {
  Real result = 0.0;
  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      result += m1[i*size + j] * m2[j*size + i];
    }
  }
  return result;
}


/**
 * Compute the trace of the product of 2 matrices and vectors using the formula:
 * trace((m1+m2)*(v1-v2)*(v1-v2)')
 *
 * m1 and m2 are square matrices of dimension size, v1 and v2 are vectors of
 * dimension size.
 */
inline Real traceMatrixProductMean(const Real* m1, const Real* m2,
                                   const Real* v1, const Real* v2,
                                   int size) {
  Real result = 0.0;

  // we want this to be on the stack to avoid mem allocation, this function
  // needs to be real fast
  Q_ASSERT(size < 64);
  Real v[64];

  for (int i=0; i<size; i++) v[i] = v1[i] - v2[i];

  for (int i=0; i<size; i++) {
    for (int j=0; j<size; j++) {
      result += (m1[i*size + j] + m2[i*size + j]) * v[j] * v[i];
    }
  }

  return result;
}



} // namespace gaia2

#endif // GAIA_KULLBACKLEIBLERDISTANCE_H
