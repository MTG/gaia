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
#ifndef GAIA_STL_ANALYZER_H
#define GAIA_STL_ANALYZER_H

#include "transformation.h"
#include "dataset.h"

namespace gaia2std {

//#ifndef PARAMETERMAP_DEFINED
//class ParameterMap;
//#endif // PARAMETERMAP_DEFINED

class Analyzer {
  DECLARE_PROXY(Analyzer);

 public:
  Analyzer(const std::string& name, const ParameterMap& params);

  proxy_ptr<Transformation> analyze(const DataSet* data) const;

};

} // namespace gaia2std


#endif // GAIA_STL_ANALYZER_H
