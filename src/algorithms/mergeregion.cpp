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
