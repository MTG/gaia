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
