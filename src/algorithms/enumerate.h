#ifndef GAIA_ENUMERATE_H
#define GAIA_ENUMERATE_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Enumerate analyzer allows you to specify a list of string descriptors
 * which you want to be stored as enumerations. Typically, for string descriptors
 * that only take a subset of values (eg: key and mode), this will reduce the
 * memory used for storage and it will also be much faster when filtering.
 * @param descriptorNames the names of the descriptors to be enumerated. Can use wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class Enumerate : public Analyzer {

 public:

  Enumerate(const ParameterMap& params);
  virtual ~Enumerate() {};

  Transformation analyze(const DataSet* data) const;

};

} // namespace gaia2

#endif // GAIA_ENUMERATE_H
