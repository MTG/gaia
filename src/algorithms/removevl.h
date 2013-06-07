#ifndef GAIA_REMOVEVL_H
#define GAIA_REMOVEVL_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The RemoveVL analyzer checks for descriptors which are variable-length
 * descriptors, such as the list of beats, ticks, chords, etc.\ and removes
 * them.
 */
class RemoveVL : public Analyzer {

 protected:
  QStringList _collections;

 public:

  RemoveVL(const ParameterMap& params);
  virtual ~RemoveVL() {};

  Transformation analyze(const DataSet* data) const;
};

} // namespace gaia2

#endif // GAIA_REMOVEVL_H
