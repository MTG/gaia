#include "rename.h"
#include "utils.h"

namespace gaia2 {

Rename::Rename(const ParameterMap& params) : Analyzer(params) {
  validParams = QStringList() << "descriptorName" << "newName";
}


Transformation Rename::analyze(const DataSet* dataset) const {
  G_INFO("Doing rename analysis...");
  checkDataSet(dataset);

  QString dname = _params.value("descriptorName").toString();

  // accept wildcards for searching
  QStringList matches = dataset->layout().descriptorNames(QStringList() << dname);

  if (matches.empty()) {
    throw GaiaException("Rename: could not find descriptor '", dname, "'");
  }

  if (matches.size() > 1) {
    throw GaiaException("Rename: it is a really bad idea to use Rename with wildcards: " + dname);
  }

  Transformation result(dataset->layout());
  result.analyzerName = "rename";
  result.analyzerParams = _params;
  result.applierName = "renameapplier";
  result.params = _params;
  result.params.insert("fullName", matches[0]);

  return result;
}


} // namespace gaia2
