#ifndef GAIA_SELECT_H
#define GAIA_SELECT_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Select analyzer allows you to keep only specific descriptors.
 * @param descriptorNames the names of the descriptors to be kept. Can use wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class Select : public Analyzer {

 public:

  Select(const ParameterMap& params);
  virtual ~Select() {};

  Transformation analyze(const DataSet* data) const;

};

} // namespace gaia2

#endif // GAIA_SELECT_H
