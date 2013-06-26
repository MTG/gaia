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

#ifndef GAIA_GAUSSIANIZE_H
#define GAIA_GAUSSIANIZE_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The Gaussianize analyzer fits each descriptor into a gaussian distribution.
 * It does it by sorting all the values, and giving them new values as if they
 * were forming a perfect gaussian distribution.
 * In doing so it completely discards the value it had before (only keeps the
 * rank), and as such may be not as correct or precise as a BoxCox
 * transformation, but it is much faster to compute.
 *
 * NB: This algorithm does not yet work with multi-segments points.
 *
 * @param distributionSize when analyzing the distribution of a lot of values,
 *        it is not necessary to keep all points as reference, but only a subset
 *        of them. This value represent the maximum number of reference values
 *        used to model the distribution. In most cases, 10'000 points should
 *        give a precise enough distribution while still being efficient to
 *        compute. (default: 10000).
 */
class Gaussianize : public Analyzer {

 public:
  Gaussianize(const ParameterMap& params);
  virtual ~Gaussianize() {};

  Transformation analyze(const DataSet* data, const Region& region) const;

 protected:
  int _maxDistSize;

};

} // namespace gaia2

#endif // GAIA_GAUSSIANIZE_H
