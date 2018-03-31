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

/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/

#include "enumerate.h"
#include "../utils.h"

namespace gaia2 {

Enumerate::Enumerate(const ParameterMap& params) : Analyzer(params) {}


Transformation Enumerate::analyze(const DataSet* dataset) const {
  G_INFO("Doing enumerate analysis...");
  checkDataSet(dataset);

  Transformation result(dataset->layout());
  result.analyzerName = "enumerate";
  result.analyzerParams = _params;
  result.applierName = "enumerateapplier";
  result.params.insert("descriptorNames", selectDescriptors(dataset->layout(),
                                                            UndefinedType,
                                                            _descriptorNames,
                                                            _exclude));

  return result;
}


} // namespace gaia2
