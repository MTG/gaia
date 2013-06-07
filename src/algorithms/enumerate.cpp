#include "enumerate.h"
#include "utils.h"

namespace gaia2 {

Enumerate::Enumerate(const ParameterMap& params) : Analyzer(params) {}


Transformation Enumerate::analyze(const DataSet* dataset) const {
  G_INFO("Doing enumerate analysis...");
  checkDataSet(dataset);

  Transformation result(dataset->layout());
  result.analyzerName = "enumerate";
  result.analyzerParams = _params;
  result.applierName = "enumerateapplier";
  result.params.insert("descriptorNames", selectDescriptors(dataset->layout(),
                                                            UndefinedType,
                                                            _descriptorNames,
                                                            _exclude));

  return result;
}


} // namespace gaia2
