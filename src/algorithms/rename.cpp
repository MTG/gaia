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
