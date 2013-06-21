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
#ifndef GAIA_RENAME_H
#define GAIA_RENAME_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Rename analyzer allows you to rename an existing field.
 * @param descriptorName the name of the field you want to rename.
 * @param newName the new names to give to the field.
 */
class Rename : public Analyzer {

 public:

  Rename(const ParameterMap& params);
  virtual ~Rename() {};

  Transformation analyze(const DataSet* data) const;

};

} // namespace gaia2

#endif // GAIA_RENAME_H
