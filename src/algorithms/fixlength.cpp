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
#include "fixlength.h"
#include "removevl.h"
#include "algoutils.h"

namespace gaia2 {

FixLength::FixLength(const ParameterMap& params) : Analyzer(params) {
  _default = false;
  try {
    _descriptorNames = params.value("descriptorNames").toStringList();
  }
  catch (GaiaException&) {
    _default = true;
  }
  _exclude = params.value("except", QStringList()).toStringList();
}


Transformation FixLength::analyze(const DataSet* dataset) const {
  G_INFO("Doing fixlength analysis...");
  checkDataSet(dataset);

  QStringList selected, excluded;
  if (_default) {
    // if we didn't specify descriptorNames, we want the default behavior which
    // is to fix the length of all the descriptors which have the same size in all
    // points in this dataset
    selected = dataset->layout().descriptorNames();
    excluded = findVariableLengthDescriptors(dataset);
  }
  else { // !_default
    // only fix the length of the specified descriptors
    selected = dataset->layout().descriptorNames(UndefinedType, _descriptorNames);
    excluded = dataset->layout().descriptorNames(UndefinedType, _exclude);
  }

  // substract one from the other to get final list of desired descriptors
  foreach (QString name, excluded) {
    selected.removeAll(name);
  }
  selected.sort();


  Transformation result(dataset->layout());
  result.analyzerName = "fixlength";
  result.analyzerParams = _params;
  result.applierName = "fixlengthapplier";

  ParameterMap params;
  params.insert("descriptorNames", selected);

  result.params = params;

  return result;
}


} // namespace gaia2
