#ifndef GAIA_MERGEREGION_H
#define GAIA_MERGEREGION_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 * The MergeRegion analyzer allows you to merge multiple descriptors into a single
 * one. You lose the ability to discern between them, but you gain a lot of memory
 * space by doing that.
 * @param descriptorNames the names of the descriptors to be merged. Can use wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param resultName the name of the resulting descriptor (default: "merged").
 */
class MergeRegion : public Analyzer {

 public:

  MergeRegion(const ParameterMap& params);
  virtual ~MergeRegion() {};

  Transformation analyze(const DataSet* data) const;


 protected:
  QString _resultName;

};

} // namespace gaia2

#endif // GAIA_MERGEREGION_H
