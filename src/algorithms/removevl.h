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

#ifndef GAIA_REMOVEVL_H
#define GAIA_REMOVEVL_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The RemoveVL analyzer checks for descriptors which are variable-length
 * descriptors, such as the list of beats, ticks, chords, etc.\ and removes
 * them.
 */
class RemoveVL : public Analyzer {

 protected:
  QStringList _collections;

 public:

  RemoveVL(const ParameterMap& params);
  virtual ~RemoveVL() {};

  Transformation analyze(const DataSet* data) const;
};

} // namespace gaia2

#endif // GAIA_REMOVEVL_H
