#include "mergeregion.h"
#include "../utils.h"

namespace gaia2 {

MergeRegion::MergeRegion(const ParameterMap& params) : Analyzer(params) {
  validParams << "resultName";

  _resultName = params.value("resultName", "merged").toString();
}


Transformation MergeRegion::analyze(const DataSet* dataset) const {
  G_INFO("Doing MergeRegion analysis...");
  checkDataSet(dataset);

  QStringList selected = selectDescriptors(dataset->layout(), RealType, _descriptorNames, _exclude);

  Transformation result(dataset->layout());
  result.analyzerName = "mergeregion";
  result.analyzerParams = _params;
  result.applierName = "mergeregionapplier";

  ParameterMap params;
  params.insert("resultName", _resultName);
  params.insert("descriptorNames", selected);

  result.params = params;

  return result;
}


} // namespace gaia2
