#ifndef GAIA_FIXLENGTH_H
#define GAIA_FIXLENGTH_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The FixLength analyzer allows you to specify a list of descriptors for which
 * you want to fix the length definitely. This is very useful for saving memory.
 * @param descriptorNames the names of the descriptors to be fixed. Can use wildcards.
 *        If you don't specify a value here, it will fix the length of all the descriptors
 *        which are of fixed size inside this dataset. In that case, the @c exclude
 *        parameter will be ignored.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class FixLength : public Analyzer {

 public:

  FixLength(const ParameterMap& params);
  virtual ~FixLength() {};

  Transformation analyze(const DataSet* data) const;


 protected:
  bool _default;

};

} // namespace gaia2

#endif // GAIA_FIXLENGTH_H
