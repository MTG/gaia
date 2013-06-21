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
