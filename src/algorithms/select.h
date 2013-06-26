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

#ifndef GAIA_SELECT_H
#define GAIA_SELECT_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Select analyzer allows you to keep only specific descriptors.
 * @param descriptorNames the names of the descriptors to be kept. Can use wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class Select : public Analyzer {

 public:

  Select(const ParameterMap& params);
  virtual ~Select() {};

  Transformation analyze(const DataSet* data) const;

};

} // namespace gaia2

#endif // GAIA_SELECT_H
