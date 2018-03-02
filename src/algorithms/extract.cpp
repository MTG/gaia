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

#include "extract.h"
#include "../utils.h"

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
