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
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
#ifndef GAIA_SVMTRAIN_H
#define GAIA_SVMTRAIN_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The SVMTrain analyzer creates a SVM model for the given dataset using libsvm.
 *
 * Chih-Chung Chang and Chih-Jen Lin, LIBSVM : a library for support vector machines, 2001.
 * Software available at http://www.csie.ntu.edu.tw/~cjlin/libsvm.
 *
 * @param className the name of the descriptor which is to be used as the class
 * @param type the SVM type. Available types are:
 *        <ul><li>C-SVC</li>
 *        <li>nu-SVC</li>
 *        <li>one-class</li>
 *        <li>epsilon-SVR</li>
 *        <li>nu-SVR</li></ul>
 *        (default: C-SVC)
 * @param kernel the kernel type. Available types are:
 *        <ul><li>linear</li>
 *        <li>polynomial (poly)</li>
 *        <li>radial (basis function) (also called RBF)</li>
 *        <li>sigmoid</li></ul>
 *        (default: RBF)
 * @param probability whether to train the model for probability estimates (default: false)
 * @param degree the degree in the kernel function (default 3)
 * @param c the cost parameter @c C used in C-SVC, epsilon-SVR, and nu-SVR (default 1)
 * @param nu the @c nu parameter of nu-SVC, one-class SVM, and nu-SVR (default 0.5)
 * @param gamma the @c gamma coeff in the kernel function (default 1/dim)
 */
class SVMTrain : public Analyzer {
 protected:
  QString _className;

 public:

  SVMTrain(const ParameterMap& params);
  virtual ~SVMTrain() {};

  Transformation analyze(const DataSet* data) const;

 protected:

  /**
   * Fills params translation maps, etc...
   */
  static void initParams();
  static bool _paramsInitialized;

  static GaiaMap<QString, int, GaiaException> _svmTypeMap;
  static GaiaMap<QString, int, GaiaException> _kernelTypeMap;
};

} // namespace gaia2

#endif // GAIA_SVMTRAIN_H
