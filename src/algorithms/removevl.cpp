#include "removevl.h"
#include "algoutils.h"

namespace gaia2 {

RemoveVL::RemoveVL(const ParameterMap& params) : Analyzer(params) {}


Transformation RemoveVL::analyze(const DataSet* dataset) const {
  G_INFO("Doing removevl analysis...");
  checkDataSet(dataset);

  QStringList toRemove = findVariableLengthDescriptors(dataset);
  G_DEBUG(GAlgorithms, "removing:" << toRemove);

  // Tranfsormation parameters
  Transformation result(dataset->layout());
  result.analyzerName = "removevl";
  result.analyzerParams = _params;
  result.applierName = "removedesc";
  result.params.insert("descriptorNames", toRemove);

  return result;
}

} // namespace gaia2
