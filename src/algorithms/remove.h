#ifndef GAIA_REMOVE_H
#define GAIA_REMOVE_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Remove analyzer allows you to discard specific descriptors.
 * @param descriptorNames the names of the descriptors to be discarded.
 *        Can use wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param failOnUnmatched whether to fail if one of the given patterns didn't
 *        match any descriptor in the layout.
 */
class Remove : public Analyzer {
 protected:
  bool _failOnUnmatched;

 public:

  Remove(const ParameterMap& params);
  virtual ~Remove() {};

  Transformation analyze(const DataSet* data) const;

};

} // namespace gaia2

#endif // GAIA_REMOVE_H
