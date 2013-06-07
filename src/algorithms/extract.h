#ifndef GAIA_EXTRACT_H
#define GAIA_EXTRACT_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Extract analyzer allows you to extract only specific dimensions from any
 * given number of descriptors. It works by specifying the list of dimensions to
 * be extracted as standalone descriptors in the form @c descriptorName[dim].
 * @b NB: at the moment, this transformation only works on Real descriptors.
 * @param dimensions the names of the descriptors/dimensions to be extracted.
 */
class Extract : public Analyzer {

 public:

  Extract(const ParameterMap& params);
  virtual ~Extract() {};

  Transformation analyze(const DataSet* data) const;


 protected:
  QMap<QString, QList<int> > _dims;

};

} // namespace gaia2

#endif // GAIA_EXTRACT_H
