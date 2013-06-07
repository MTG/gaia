#include "../linalg.h"
#include "../timer.h"
#include "../utils.h"
#include "inverseprojection.h"
using namespace std;

namespace gaia2 {

InverseProjection::InverseProjection(const ParameterMap& params) : Analyzer(params) {
  validParams = QStringList();
}


Transformation InverseProjection::analyze(const DataSet* dataset) const {
  G_INFO("Doing InverseProjection analysis...");
  checkDataSet(dataset);

  const TransfoChain& history = dataset->history();

  if (history.isEmpty() || history[history.size()-1].applierName != "matmult") {
    throw GaiaException("InverseProjection: cannot apply InverseProjection on a dataset which last transformation is not a projection (PCA, RCA)");
  }

  const Transformation& proj = history[history.size()-1];
  Eigen::MatrixXf basis = proj.params.value("matrix").toMatrix().transpose();


  Transformation result(dataset->layout());
  result.analyzerName = "inversepca";
  result.analyzerParams = _params;
  result.applierName = "matmult";
  result.params.insert("descriptorNames", proj.params.value("resultName"));
  result.params.insert("resultName", proj.params.value("resultName"));
  result.params.insert("matrix", basis);

  return result;
}


} // namespace gaia2
