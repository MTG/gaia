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

#ifndef GAIA_FIXLENGTH_H
#define GAIA_FIXLENGTH_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The FixLength analyzer allows you to specify a list of descriptors for which
 * you want to fix the length definitely. This is very useful for saving memory.
 * @param descriptorNames the names of the descriptors to be fixed. Can use wildcards.
 *        If you don't specify a value here, it will fix the length of all the descriptors
 *        which are of fixed size inside this dataset. In that case, the @c exclude
 *        parameter will be ignored.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class FixLength : public Analyzer {

 public:

  FixLength(const ParameterMap& params);
  virtual ~FixLength() {};

  Transformation analyze(const DataSet* data) const;


 protected:
  bool _default;

};

} // namespace gaia2

#endif // GAIA_FIXLENGTH_H
