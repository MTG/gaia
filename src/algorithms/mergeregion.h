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

#ifndef GAIA_MERGEREGION_H
#define GAIA_MERGEREGION_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 * The MergeRegion analyzer allows you to merge multiple descriptors into a single
 * one. You lose the ability to discern between them, but you gain a lot of memory
 * space by doing that.
 * @param descriptorNames the names of the descriptors to be merged. Can use wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param resultName the name of the resulting descriptor (default: "merged").
 */
class MergeRegion : public Analyzer {

 public:

  MergeRegion(const ParameterMap& params);
  virtual ~MergeRegion() {};

  Transformation analyze(const DataSet* data) const;


 protected:
  QString _resultName;

};

} // namespace gaia2

#endif // GAIA_MERGEREGION_H
