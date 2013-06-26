/* 
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 * 
 * Gaia is free software: you can redistribute it and/or modify it under 
 * the terms of the GNU Affero General Public License as published by the Free 
 * Software Foundation (FSF), either version 3 of the License, or (at your 
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include "rca.h"
#include "../gaia.h"
#include "../linalg.h"
#include "../yamlcpp.h"
#include "../utils.h"
using namespace std;
using namespace Eigen;

namespace gaia2 {

typedef QMap<QString, QVariant> VMap;

RCA::RCA(const ParameterMap& params) : Analyzer(params) /*, _lowmem(false)*/ {
  validParams << "dimension" << "resultName" << "classFile" << "className" << "classMap";

  _targetDimension = _params.value("dimension").toInt();
  _classFile = _params.value("classFile", "").toString();
  _className = _params.value("className", "").toString();

  VMap vmap = _params.value("classMap", VMap()).toMap();
  for (VMap::const_iterator it = vmap.constBegin(); it != vmap.constEnd(); ++it) {
    _classMap.insert(it.key(), it.value().toString());
  }

  if (!((!_classFile.isEmpty() && _classMap.isEmpty() && _className.isEmpty()) ||
        (_classFile.isEmpty() && !_classMap.isEmpty() && _className.isEmpty()) ||
        (_classFile.isEmpty() && _classMap.isEmpty() && !_className.isEmpty()))) {
    throw GaiaException("RCA: You need to specify either classFile, classMap or className, and only one of them.");
  }

  _resultName = _params.value("resultName", "").toString();
  if (_resultName.isEmpty()) {
    throw GaiaException("You need to specify a 'resultName' which is not the empty string...");
  }

  G_DEBUG(GAlgorithms, "RCA: output descriptor name:" << _resultName);
}


/**
 * @todo figure out what the fuck is going on here. Works exactly like the
 * matlab algo, so it should be ok. However there's something that smells
 * fishy to me so better check it out sometime...
 */
Transformation RCA::analyze(const DataSet* dataset, const Region& descs) const {
  G_INFO("Doing RCA analysis...");
  checkDataSet(dataset);
  checkMinPoints(dataset, 2);
  Region region = checkFixedLength(descs.select(RealType), dataset->layout());

  // get the chunklets, now that we have the dataset
  QList<const PointArray*> chunklets;
  if (!_classFile.isEmpty()) {
    chunklets = getChunkletsFromFile(_classFile, dataset);
  }
  else if (!_className.isEmpty()) {
    chunklets = getChunkletsFromLabel(_className, dataset);
  }
  else {
    chunklets = getChunkletsFromMap(_classMap, dataset);
  }

  if (chunklets.size() == 0) {
    throw GaiaException("No points in classes... Cannot perform RCA analysis...");
  }

  // step 1 : compute the within chunklet covariance matrix
  MatrixXd cov = computeCovarianceMatrix(chunklets, region);
  MatrixXd A;

  const int dim = cov.rows();
  // step 2 : apply dimensionality reduction
  if (_targetDimension < dim) {
    // compute total covariance using only chunkleted points
    // (original matlab version also has the following line commented:
    //   % TotalCov= cov(data); %compute total covariance using all the data.
    //   % More accurate, but may lead to PSD problems.
    // ). I'll stick to the uncommented version for now.

    PointArray allchunklets(false); // this array should not take ownership of the points
    foreach (const PointArray* c, chunklets) allchunklets += *c;

    MatrixXd totalCov = covarianceMatrix(allchunklets, region);
    MatrixXd fld = totalCov.lu().inverse() * cov;

    // Cannot use self-adjoint solver here as the FLD is not necessarily symmetric
    EigenSolver<MatrixXd> solver(fld);
    VectorXd eigenValues = solver.eigenvalues().real();
    MatrixXd eigenVectors = solver.eigenvectors().real();

    sortEigenVectors(eigenValues, eigenVectors);

    A = eigenVectors.block(0, 0, dim, _targetDimension);

    // we have to normalize the values in A, because maybe the eigenvectors
    // weren't. They would be if fld were symmetric, but that is not always the
    // case...
    for (int i=0; i<_targetDimension; i++) {
      double norm = A.col(i).norm();
      A.col(i) /= norm;
    }

    cov = A.transpose() * cov * A;
  }
  else {
    A = MatrixXd::Identity(dim, dim);
  }


  // step 3a : compute the whitening transformation
  MatrixXf featureVector = (A * whitenMatrix(cov)).transpose().cast<float>();

  // Some info and statistics about the RCA analysis
  QString dimBoost = formatDimensionParticipation(dimensionBoost(featureVector, region, dataset), true);
  G_DEBUG(GAlgorithms, "RCA dimension boost:");
  G_DEBUG(GAlgorithms, dimBoost);


  Transformation result(dataset->layout());
  result.analyzerName = "rca";
  result.analyzerParams = _params;
  result.applierName = "matmult";
  result.params.insert("resultName", _resultName);
  result.params.insert("descriptorNames", dataset->layout().descriptorNames(region));
  result.params.insert("matrix", featureVector);
  result.info.insert("boost", dimBoost);

  // free up memory used by chunklets
  foreach (const PointArray* c, chunklets) delete c;

  return result;
}


MatrixXd RCA::whitenMatrix(const MatrixXd& cov) const {
  // this step consists 'only' in computing W = C^(-1/2), however this
  // translates in heavier computing than what is written. We have to
  // diagonalize the matrix (however as it is symmetric definite positive,
  // this consists in getting the eigenvectors and that's it), elevate
  // the diagonal part to the desired power and re-multiply the different
  // parts to get what we're looking for.

  // Thanks Eigen2!!
  SelfAdjointEigenSolver<MatrixXd> solver(cov);
  return solver.operatorInverseSqrt();
}


MatrixXd RCA::computeCovarianceMatrix(const QList<const PointArray*>& chunklets,
                                      const Region& region) const {
  int totalSegments = 0;
  const int dim = region.dimension(RealType);
  MatrixXd cov = MatrixXd::Zero(dim, dim);

  for (int i=0; i<chunklets.size(); i++) {
    // compute covariance for chunklet i and de-normalize it
    const int nsegments = chunklets[i]->totalSegments();

    cov += covarianceMatrix(*chunklets[i], region) * (nsegments - 1);
    totalSegments += nsegments;
  }

  // re-normalize the matrix here, with the total number of segments
  cov /= totalSegments;

  return cov;
}


QList<const PointArray*> RCA::getChunkletsFromFile(const QString& filename,
                                                   const DataSet* inputDataSet) {

  yaml::Mapping yclasses = yaml::loadFromFile(filename).mapping();
  QMap<QString, QString> classes;

  for (yaml::Mapping::const_iterator it = yclasses.constBegin(); it != yclasses.constEnd(); ++it) {
    classes.insert(it.key(), it.value());
  }

  return getChunkletsFromMap(classes, inputDataSet);
}

QList<const PointArray*> RCA::getChunkletsFromMap(const QMap<QString, QString>& groundTruth,
                                                  const DataSet* inputDataSet) {
  QList<QString> classNames;
  QList<PointArray*> chunklets;

  //foreach (QString name, classes.keys()) {
  for (QMap<QString, QString>::const_iterator it = groundTruth.constBegin();
       it != groundTruth.constEnd(); ++it) {

    try {
      Point* p = const_cast<Point*>(inputDataSet->point(it.key()));

      int idx = classNames.indexOf(it.value());
      if (idx >= 0) {
        chunklets[idx]->append(p);
        // is not a problem because we return a const Collection*
      }
      else {
        // create the class bin
        classNames.append(it.value());
        chunklets.append(new PointArray(false)); // this PointArray does not own the points
        chunklets.back()->append(p);
      }
    }
    catch (GaiaException&) {
      G_WARNING("WARNING: point named '" << it.key() << "' has not been found "
                "in dataset... Ignoring...");
    }
  }

  return cleanChunklets(chunklets);
}

QList<const PointArray*> RCA::cleanChunklets(QList<PointArray*>& chunklets) {
  // remove useless classes (less than 2 elements)
  for (int i=0; i<chunklets.size(); i++) {
    if (chunklets[i]->size() < 2) {
      chunklets.removeAt(i);
      i--;
    }
  }

  QList<const PointArray*> constChunklets;
  foreach (PointArray* c, chunklets) constChunklets.append(c);

  return constChunklets;
}

QList<const PointArray*> RCA::getChunkletsFromLabel(const QString& label,
                                                    const DataSet* inputDataSet) {

  QMap<QString, QString> classes;

  for (int i=0; i<inputDataSet->size(); i++) {
    Point* p = const_cast<Point*>(inputDataSet->at(i));
    QString className = p->label(label).toSingleValue();

    classes.insert(p->name(), className);
  }

  return getChunkletsFromMap(classes, inputDataSet);
}

} // namespace gaia2
