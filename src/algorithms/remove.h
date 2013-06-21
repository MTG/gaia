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
#ifndef GAIA_REMOVE_H
#define GAIA_REMOVE_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Remove analyzer allows you to discard specific descriptors.
 * @param descriptorNames the names of the descriptors to be discarded.
 *        Can use wildcards.
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param failOnUnmatched whether to fail if one of the given patterns didn't
 *        match any descriptor in the layout.
 */
class Remove : public Analyzer {
 protected:
  bool _failOnUnmatched;

 public:

  Remove(const ParameterMap& params);
  virtual ~Remove() {};

  Transformation analyze(const DataSet* data) const;

};

} // namespace gaia2

#endif // GAIA_REMOVE_H
