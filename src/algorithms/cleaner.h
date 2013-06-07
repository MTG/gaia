#ifndef GAIA_CLEANER_H
#define GAIA_CLEANER_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Cleaner analyzer checks for values that may cause problems when doing
 * mathematical analysis (such as normalize, pca, ...), and removes them.
 * This analyzer checks for Real descriptors which have either Infinite or
 * NaN values, and the ones that are constant (< 1e-7, actually, hence they can
 * not be normalized, and do not add any information anyway), and returns a
 * transformation that removes them.
 * @param descriptorNames the names of the descriptors to be included in the
 *        checking. Only accepts real descriptors, can use wildcards.
 *        (default: "*").
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class Cleaner : public Analyzer {
 public:

  /**
   * Minimum range under which a dimension will be thrown away.
   */
  static const Real MIN_RANGE;

  Cleaner(const ParameterMap& params);
  virtual ~Cleaner() {};

  Transformation analyze(const DataSet* data, const Region& region) const;

};

} // namespace gaia2

#endif // GAIA_CLEANER_H
