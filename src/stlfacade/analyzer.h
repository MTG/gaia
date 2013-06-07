#ifndef GAIA_STL_ANALYZER_H
#define GAIA_STL_ANALYZER_H

#include "transformation.h"
#include "dataset.h"

namespace gaia2std {

//#ifndef PARAMETERMAP_DEFINED
//class ParameterMap;
//#endif // PARAMETERMAP_DEFINED

class Analyzer {
  DECLARE_PROXY(Analyzer);

 public:
  Analyzer(const std::string& name, const ParameterMap& params);

  proxy_ptr<Transformation> analyze(const DataSet* data) const;

};

} // namespace gaia2std


#endif // GAIA_STL_ANALYZER_H
