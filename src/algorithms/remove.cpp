#include "remove.h"
#include "utils.h"

namespace gaia2 {

Remove::Remove(const ParameterMap& params) : Analyzer(params) {
  validParams << "failOnUnmatched";

  _failOnUnmatched = _params.value("failOnUnmatched", true).toBool();
}


Transformation Remove::analyze(const DataSet* dataset) const {
  G_INFO("Doing remove analysis...");
  checkDataSet(dataset);

  Transformation result(dataset->layout());
  result.analyzerName = "remove";
  result.analyzerParams = _params;
  result.applierName = "removedesc";
  result.params.insert("descriptorNames", selectDescriptors(dataset->layout(),
                                                            UndefinedType,
                                                            _descriptorNames,
                                                            _exclude,
                                                            _failOnUnmatched));

  return result;
}


} // namespace gaia2
