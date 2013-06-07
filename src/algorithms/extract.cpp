#include "extract.h"
#include "utils.h"

namespace gaia2 {

Extract::Extract(const ParameterMap& params) : Analyzer(params) {
  validParams = QStringList() << "dimensions";

  _dims = dimensionListToMapping(params.value("dimensions").toStringList());
}


Transformation Extract::analyze(const DataSet* dataset) const {
  G_INFO("Doing extract analysis...");
  checkDataSet(dataset);

  // get the full names for the descriptors
  QStringList dimMapping;
  foreach (QString name, _dims.keys())  {
    QStringList matches = dataset->layout().descriptorNames(RealType, name);
    if (matches.empty()) {
      throw GaiaException("Extract: could not find descriptor '", name, "'");
    }

    if (matches.size() > 1) {
      throw GaiaException("Extract: it is a really bad idea to use Extract with wildcards: " + name);
    }

    foreach (int d, _dims[name]) {
      dimMapping << (matches[0] + '[' + QString::number(d) + ']');
    }
  }

  Transformation result(dataset->layout());
  result.analyzerName = "extract";
  result.analyzerParams = _params;
  result.applierName = "extractapplier";
  result.params = _params;
  result.params["dimensions"] = dimMapping;

  return result;
}


} // namespace gaia2
