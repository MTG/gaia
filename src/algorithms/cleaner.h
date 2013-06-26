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

#ifndef GAIA_CLEANER_H
#define GAIA_CLEANER_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datasetmanagementalgos
 *
 * The Cleaner analyzer checks for values that may cause problems when doing
 * mathematical analysis (such as normalize, pca, ...), and removes them.
 * This analyzer checks for Real descriptors which have either Infinite or
 * NaN values, and the ones that are constant (< 1e-7, actually, hence they can
 * not be normalized, and do not add any information anyway), and returns a
 * transformation that removes them.
 * @param descriptorNames the names of the descriptors to be included in the
 *        checking. Only accepts real descriptors, can use wildcards.
 *        (default: "*").
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 */
class Cleaner : public Analyzer {
 public:

  /**
   * Minimum range under which a dimension will be thrown away.
   */
  static const Real MIN_RANGE;

  Cleaner(const ParameterMap& params);
  virtual ~Cleaner() {};

  Transformation analyze(const DataSet* data, const Region& region) const;

};

} // namespace gaia2

#endif // GAIA_CLEANER_H
