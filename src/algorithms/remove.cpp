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

#include "remove.h"
#include "../utils.h"

namespace gaia2 {

Remove::Remove(const ParameterMap& params) : Analyzer(params) {
  validParams << "failOnUnmatched";

  _failOnUnmatched = _params.value("failOnUnmatched", true).toBool();
}


Transformation Remove::analyze(const DataSet* dataset) const {
  G_INFO("Doing remove analysis...");
  checkDataSet(dataset);

  Transformation result(dataset->layout());
  result.analyzerName = "remove";
  result.analyzerParams = _params;
  result.applierName = "removedesc";
  result.params.insert("descriptorNames", selectDescriptors(dataset->layout(),
                                                            UndefinedType,
                                                            _descriptorNames,
                                                            _exclude,
                                                            _failOnUnmatched));

  return result;
}


} // namespace gaia2
