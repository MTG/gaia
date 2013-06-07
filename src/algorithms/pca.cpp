#include "pca.h"
#include "linalg.h"
#include "timer.h"
#include "utils.h"
#include "Eigen/Eigenvalues"
using namespace std;
using namespace Eigen;

namespace gaia2 {

PCA::PCA(const ParameterMap& params) : Analyzer(params) {
  validParams << "dimension" << "coveredVariance" << "resultName";

  _targetDimension = _params.value("dimension", -1).toInt();
  _coveredVariance = _params.value("coveredVariance", -1).toDouble();

  if ((_targetDimension == -1 && _coveredVariance == -1) ||
      (_targetDimension != -1 && _coveredVariance != -1)) {
    throw GaiaException("PCA: the dimension and coveredVariance parameters are mutually exclusive: you need to specify one or the other");
  }

  _resultName = _params.value("resultName", "").toString();
  if (_resultName.isEmpty()) {
    throw GaiaException("You need to specify a 'resultName' which is not the empty string...");
  }

  G_DEBUG(GAlgorithms, "PCA: output descriptor name:" << _resultName);
}


Real totalVariancePercentage(const VectorXd& eigenValues,
                             int targetDimension) {
  const int dim = eigenValues.size();
  double total=0.0, target=0.0;

  for (int i=0; i<(int)dim; i++) {
    total += abs(eigenValues[i]);
    if (i < targetDimension)
      target += abs(eigenValues[i]);
  }

  return (target*100) / total;
}


Transformation PCA::analyze(const DataSet* dataset, const Region& descs) const {
  G_INFO("Doing PCA analysis...");
  checkDataSet(dataset);
  checkMinPoints(dataset, 2);
  Region region = checkFixedLength(descs.select(RealType), dataset->layout());

  // sanity check
  int dim = region.dimension(RealType);
  if (_targetDimension > dim) {
    QStringList msg;
    msg << "PCA: Specified target dimension (" << QString::number(_targetDimension)
        << ") is greater than given dataset dimension (" << QString::number(dim) << ")";
    throw GaiaException(msg);
  }


  // compute the covariance matrix
  G_DEBUG(GAlgorithms, "PCA: computing covariance matrix");
  Timer chrono;
  chrono.start();
  MatrixXd covariance = covarianceMatrix(*dataset, region);
  chrono.stop();
  G_DEBUG(GAlgorithms, "PCA: cov matrix computed in " << chrono.elapsed() << "s.");

  // FIXME: need to catch the case where the matrix is not invertible (or do we?)
  SelfAdjointEigenSolver<MatrixXd> solver(covariance);
  VectorXd eigenValues = solver.eigenvalues();
  MatrixXd eigenVectors = solver.eigenvectors();

  Q_ASSERT(dim == eigenValues.size());

  // sort eigenvalues/vectors
  sortEigenVectors(eigenValues, eigenVectors, std::greater<double>());

  // if we specified the coveredVariance param, now is a good time to estimate
  // the target dimension of our PCA
  int targetDimension = _targetDimension;
  if (targetDimension == -1) {
    targetDimension = 1;
    while (totalVariancePercentage(eigenValues, targetDimension) < _coveredVariance) {
      targetDimension++;
    }
    Q_ASSERT(targetDimension <= eigenValues.size());
  }

  // transpose and copy
  MatrixXf featureVector = eigenVectors.transpose().block(0, 0, targetDimension, dim).cast<Real>();

  // Some info and statistics about the PCA analysis
  Real totalVariance = totalVariancePercentage(eigenValues, targetDimension);

  QString odimpart = formatDimensionParticipation(dimensionParticipation(covariance.diagonal(),
                                                                         mapRegionIndicesToNames(region, dataset)));

  QMap<int, QString> tdimMap;
  QString resultName = _params.value("resultName");
  for (int i=0; i<targetDimension; i++) {
    tdimMap.insert(i, resultName + "[" + QString::number(i) + "]");
  }

  QString tdimpart = formatDimensionParticipation(dimensionParticipation(eigenValues,
                                                                         tdimMap));

  G_DEBUG(GAlgorithms, "PCA: Chosen target dimension"
          << targetDimension << "accounts for"
          << totalVariance << "% of the total variance.");

  G_DEBUG(GAlgorithms, "PCA variance: original dimension participation:");
  G_DEBUG(GAlgorithms, odimpart);

  G_DEBUG(GAlgorithms, "PCA variance: target dimension participation:");
  G_DEBUG(GAlgorithms, tdimpart);

  // TODO: put this info in the info field of the transformation

  Transformation result(dataset->layout());
  result.analyzerName = "pca";
  result.analyzerParams = _params;
  result.applierName = "matmult";
  result.params.insert("resultName", _resultName);
  result.params.insert("descriptorNames", dataset->layout().descriptorNames(region));
  result.params.insert("matrix", featureVector);

  result.info.insert("Target dimension", targetDimension);
  result.info.insert("Total variance", totalVariance);
  result.info.insert("Original variance", odimpart);
  result.info.insert("Target variance", tdimpart);

  return result;
}


} // namespace gaia2
