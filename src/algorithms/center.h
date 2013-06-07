#ifndef GAIA_CENTER_H
#define GAIA_CENTER_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The Center analyzer centers the points around the mean of each descriptor.
 * (it substracts the mean of a each dimension to all instances).
 *
 * @param descriptorNames the names of the descriptors to be included in the
 *        normalization. Only accepts real descriptors, can use wildcards.
 *        (default: "*").
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class Center : public Analyzer {

 public:
  Center(const ParameterMap& params);
  virtual ~Center() {};

  Transformation analyze(const DataSet* data, const Region& region) const;

};

} // namespace gaia2

#endif // GAIA_CENTER_H
