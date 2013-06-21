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
#ifndef GAIA_EXTRACT_H
#define GAIA_EXTRACT_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Extract analyzer allows you to extract only specific dimensions from any
 * given number of descriptors. It works by specifying the list of dimensions to
 * be extracted as standalone descriptors in the form @c descriptorName[dim].
 * @b NB: at the moment, this transformation only works on Real descriptors.
 * @param dimensions the names of the descriptors/dimensions to be extracted.
 */
class Extract : public Analyzer {

 public:

  Extract(const ParameterMap& params);
  virtual ~Extract() {};

  Transformation analyze(const DataSet* data) const;


 protected:
  QMap<QString, QList<int> > _dims;

};

} // namespace gaia2

#endif // GAIA_EXTRACT_H
