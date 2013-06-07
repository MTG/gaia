#ifndef GAIA_RENAME_H
#define GAIA_RENAME_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Rename analyzer allows you to rename an existing field.
 * @param descriptorName the name of the field you want to rename.
 * @param newName the new names to give to the field.
 */
class Rename : public Analyzer {

 public:

  Rename(const ParameterMap& params);
  virtual ~Rename() {};

  Transformation analyze(const DataSet* data) const;

};

} // namespace gaia2

#endif // GAIA_RENAME_H
