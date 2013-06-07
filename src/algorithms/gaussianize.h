#ifndef GAIA_GAUSSIANIZE_H
#define GAIA_GAUSSIANIZE_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The Gaussianize analyzer fits each descriptor into a gaussian distribution.
 * It does it by sorting all the values, and giving them new values as if they
 * were forming a perfect gaussian distribution.
 * In doing so it completely discards the value it had before (only keeps the
 * rank), and as such may be not as correct or precise as a BoxCox
 * transformation, but it is much faster to compute.
 *
 * NB: This algorithm does not yet work with multi-segments points.
 *
 * @param distributionSize when analyzing the distribution of a lot of values,
 *        it is not necessary to keep all points as reference, but only a subset
 *        of them. This value represent the maximum number of reference values
 *        used to model the distribution. In most cases, 10'000 points should
 *        give a precise enough distribution while still being efficient to
 *        compute. (default: 10000).
 */
class Gaussianize : public Analyzer {

 public:
  Gaussianize(const ParameterMap& params);
  virtual ~Gaussianize() {};

  Transformation analyze(const DataSet* data, const Region& region) const;

 protected:
  int _maxDistSize;

};

} // namespace gaia2

#endif // GAIA_GAUSSIANIZE_H
