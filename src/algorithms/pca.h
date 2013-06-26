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

#ifndef GAIA_PCA_H
#define GAIA_PCA_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The PCA Analyzer computes the <a href="http://en.wikipedia.org/wiki/Principal_components_analysis">Principal Components Analysis</a>
 * of the given DataSet using the specified descriptors.
 * @b NB: the dimension and coveredVariance parameters are exclusive, so you must
 *        specify only one of the two.
 * @param descriptorNames the names of the descriptors to be included in the
 *        PCA analysis. Only accepts real descriptors, can use wildcards.
 *        (default: "*").
 * @param except the names of the descriptors to be removed from the selected
 *        list specified before. Can use wildcards. (default: "").
 * @param resultName the name of the resulting descriptor. (default: some
 *        name constructed from 'pca', the number of dimensions, and the original
 *        descriptor names)
 * @param dimension the target (final) dimension.
 * @param coveredVariance the percentage of the total variance to be covered by
 *        the PCA. The target dimension will then be chosen as the smallest one
 *        that accounts for at least that much variance.
 */
class PCA : public Analyzer {
 protected:
  QString _resultName;
  int _targetDimension;
  Real _coveredVariance;

 public:
  PCA(const ParameterMap& params);
  virtual ~PCA() {};

  Transformation analyze(const DataSet* dataset, const Region& region) const;

};

} // namespace gaia2

#endif // GAIA_PCA_H
