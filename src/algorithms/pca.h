#ifndef GAIA_PCA_H
#define GAIA_PCA_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The PCA Analyzer computes the <a href="http://en.wikipedia.org/wiki/Principal_components_analysis">Principal Components Analysis</a>
 * of the given DataSet using the specified descriptors.
 * @b NB: the dimension and coveredVariance parameters are exclusive, so you must
 *        specify only one of the two.
 * @param descriptorNames the names of the descriptors to be included in the
 *        PCA analysis. Only accepts real descriptors, can use wildcards.
 *        (default: "*").
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param resultName the name of the resulting descriptor. (default: some
 *        name constructed from 'pca', the number of dimensions, and the original
 *        descriptor names)
 * @param dimension the target (final) dimension.
 * @param coveredVariance the percentage of the total variance to be covered by
 *        the PCA. The target dimension will then be chosen as the smallest one
 *        that accounts for at least that much variance.
 */
class PCA : public Analyzer {
 protected:
  QString _resultName;
  int _targetDimension;
  Real _coveredVariance;

 public:
  PCA(const ParameterMap& params);
  virtual ~PCA() {};

  Transformation analyze(const DataSet* dataset, const Region& region) const;

};

} // namespace gaia2

#endif // GAIA_PCA_H
