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

#include <QSet>
#include <QFile>
#include <QTextStream>
#include <QTemporaryFile>
#include "svmtrain.h"
#include "3rdparty/libsvm/svm.h"
#include "3rdparty/libsvm/gaiasvmutils.h"
#include "utils.h"
#include "algoutils.h"

namespace gaia2 {

// static variables
bool SVMTrain::_paramsInitialized = false;
GaiaMap<QString, int, GaiaException> SVMTrain::_svmTypeMap;
GaiaMap<QString, int, GaiaException> SVMTrain::_kernelTypeMap;


void SVMTrain::initParams() {
  if (_paramsInitialized) return;

  _svmTypeMap.insert("c-svc", C_SVC);
  _svmTypeMap.insert("nu-svc", NU_SVC);
  _svmTypeMap.insert("one-class", ONE_CLASS);
  _svmTypeMap.insert("epsilon-svr", EPSILON_SVR);
  _svmTypeMap.insert("nu-svr", NU_SVR);

  _kernelTypeMap.insert("linear", LINEAR);
  _kernelTypeMap.insert("poly", POLY);
  _kernelTypeMap.insert("polynomial", POLY);
  _kernelTypeMap.insert("radial", RBF);
  _kernelTypeMap.insert("rbf", RBF);
  _kernelTypeMap.insert("sigmoid", SIGMOID);

  _paramsInitialized = true;
}



SVMTrain::SVMTrain(const ParameterMap& params) : Analyzer(params) {
  G_DEBUG(GAlgorithms, "Initializing SVMTrain analyzer");
  validParams << "type" << "kernel" << "probability" << "degree" << "c"
              << "nu" << "gamma" << "className" << "balanceClasses"
              << "maxIterations" << "maxToleranceUpdates";
  initParams();

  _className = _params.value("className").toString();

  // we never want to use the class descriptor to train an SVM (nonsense), so instead of
  // complaining if it gets selected because the user used wildcards, just remove it silently
  _exclude << _className;

}


Transformation SVMTrain::analyze(const DataSet* dataset) const {
  G_INFO("Doing SVMTrain analysis...");

  QStringList sdescs = selectDescriptors(dataset->layout(), StringType, _descriptorNames, _exclude, false);

  if (!sdescs.isEmpty()) {
    throw GaiaException("SVMTrain: if you want to use string descriptors for training your SVM, "
                        "you first need to enumerate them using the 'enumerate' transform on them. "
                        "String descriptors: ", sdescs.join(", "));
  }

  QStringList descs = selectDescriptors(dataset->layout(), UndefinedType, _descriptorNames, _exclude);
  // sort descriptors in the order in which they are taken inside the libsvm dataset
  sort(descs.begin(), descs.end(), DescCompare(dataset->layout()));
  Region region = dataset->layout().descriptorLocation(descs);

  QStringList classMapping = svm::createClassMapping(dataset, _className);

  // first, convert the training data into an SVM problem structure
  // NB: all checks about fixed-length and type of descriptors are done in this function
  struct svm_problem prob = svm::dataSetToLibsvmProblem(dataset, _className, region, classMapping);

  // also get dimension (this trick works because a vector in there is the number
  // of dimensions + 1 for the sentinel, and we're not in a sparse representation)
  int dimension = prob.x[1] - prob.x[0] - 1;

  int nr_weight = 0;
  int *labels  = NULL;
  double *weights = NULL;

  // if we want to balance the classes, compute the weights as done in Scikit-learn:
  // https://scikit-learn.org/stable/modules/generated/sklearn.utils.class_weight.compute_class_weight.html
  // Otherwise leave labels and weights as NULL pointers
  if (_params.value("balanceClasses", false).toBool()) {
    QMap<int, int> counts;

    for (int i = 0; i < prob.l; i++ ) {
      if (!counts.contains(prob.y[i])) {
        counts[prob.y[i]] = 1;
      } else {
        counts[prob.y[i]] += 1;
      }
    }

    nr_weight = counts.size();

    labels = new int[nr_weight];
    weights = new double[nr_weight];

    QMapIterator<int, int> iCounts(counts);
    int count = 0;

    // the heuristics to compute the weights are inspired by
    // Logistic Regression in Rare Events Data, King, Zen, 2001.
    while (iCounts.hasNext()) {
      iCounts.next();
    
      labels[count] = iCounts.key();
      weights[count] = (double)prob.l / (iCounts.value() * nr_weight);

      count++;
    }
  }

  // default values
  struct svm_parameter param;
  //param.svm_type = C_SVC;
  //param.kernel_type = RBF;
  //param.degree = 3;
  //param.gamma = 0;	// 1/k
  param.coef0 = 0;
  param.nu = 0.5;
  param.cache_size = 100;
  //param.C = 1;
  param.eps = 1e-3;
  param.p = 0.1;
  param.shrinking = 1;
  //param.probability = 0;
  param.nr_weight = nr_weight;
  param.weight_label = labels;
  param.weight = weights;

  // get parameters
  QString svmType = _params.value("type", "C-SVC").toString().toLower();
  param.svm_type = _svmTypeMap.value(svmType);
  QString kernelType = _params.value("kernel", "RBF").toString().toLower();
  param.kernel_type = _kernelTypeMap.value(kernelType);
  param.degree = _params.value("degree", 3).toInt();
  param.C = _params.value("c", 1).toDouble();
  param.gamma = _params.value("gamma", 1.0/dimension).toDouble();
  param.probability = _params.value("probability", false).toBool() ? 1 : 0;
  param.max_iterations = _params.value("maxIterations", 1e7).toInt();
  param.max_tolerance_updates = _params.value("maxToleranceUpdates", 4).toInt();

  const char* error_msg = svm_check_parameter(&prob, &param);

  if (error_msg) {
    throw GaiaException(error_msg);
  }


  // do it!
  struct svm_model* model;

  const bool crossValidation = false;
  if (crossValidation) {
    int nr_fold = 10;
    int total_correct = 0;
    double total_error = 0;
    double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
    double* target = new double[prob.l];

    svm_cross_validation(&prob, &param, nr_fold, target);

    if (param.svm_type == EPSILON_SVR ||
        param.svm_type == NU_SVR) {
      for (int i=0; i<prob.l; i++) {
        double y = prob.y[i];
        double v = target[i];
        total_error += (v-y)*(v-y);
        sumv += v;
        sumy += y;
        sumvv += v*v;
        sumyy += y*y;
        sumvy += v*y;
      }
      G_INFO("Cross Validation Mean squared error =" << total_error/prob.l);
      G_INFO("Cross Validation Squared correlation coefficient =" <<
             ((prob.l*sumvy - sumv*sumy) * (prob.l*sumvy - sumv*sumy)) /
             ((prob.l*sumvv - sumv*sumv) * (prob.l*sumyy - sumy*sumy))
             );
    }
    else {
      for (int i=0; i<prob.l; i++)
        if (target[i] == prob.y[i])
          ++total_correct;
      G_INFO("Cross Validation Accuracy =" << 100.0*total_correct/prob.l << "%");
    }
  }
  else { // !crossValidation
    model = svm_train(&prob, &param);
  }

  // save model to a temporary file (only method available from libsvm...),
  // reload it and put it into a gaia2::Parameter
  QTemporaryFile modelFile;
  modelFile.open();
  QString modelFilename = modelFile.fileName();
  modelFile.close();

  if (svm_save_model(modelFilename.toLatin1().constData(), model) == -1) {
    throw GaiaException("SVMTrain: error while saving SVM model to temp file");
  }

  modelFile.open();
  QByteArray modelData = modelFile.readAll();
  modelFile.close();


  // if we asked for the model to be output specifically, also do it
  if (_params.value("modelFilename", "").toString() != "") {
    QString filename = _params.value("modelFilename").toString();
    svm_save_model(filename.toLatin1().constData(), model);
  }

  // destroy the model allocated by libsvm
  svm_destroy_model(model);

  Transformation result(dataset->layout());
  result.analyzerName = "svmtrain";
  result.analyzerParams = _params;
  result.applierName = "svmpredict";
  result.params.insert("modelData", modelData);
  result.params.insert("className", _params.value("className"));
  result.params.insert("descriptorNames", descs);
  result.params.insert("classMapping", classMapping);
  result.params.insert("probability", (param.probability == 1 && (param.svm_type == C_SVC ||
                                                                  param.svm_type == NU_SVC)));

  delete[] labels;
  delete[] weights;

  return result;
}

} // namespace gaia2
