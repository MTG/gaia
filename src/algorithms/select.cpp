#include "select.h"
#include "utils.h"

namespace gaia2 {

Select::Select(const ParameterMap& params) : Analyzer(params) {}


Transformation Select::analyze(const DataSet* dataset) const {
  G_INFO("Doing select analysis...");
  checkDataSet(dataset);

  Transformation result(dataset->layout());
  result.analyzerName = "select";
  result.analyzerParams = _params;
  result.applierName = "selectdesc";
  result.params.insert("descriptorNames", selectDescriptors(dataset->layout(),
                                                            UndefinedType,
                                                            _descriptorNames,
                                                            _exclude));

  return result;
}


} // namespace gaia2
