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
#ifndef GAIA_INVERSEPROJECTION_H
#define GAIA_INVERSEPROJECTION_H

#include "analyzer.h"

namespace gaia2 {

/**
 * @ingroup analyzers datatransformationalgos
 *
 * The inverse projection Analyzer computes the inverse the previous
 * transformation, given it was a linear projection (such as PCA or RCA).
 *
 * Actually, as not all projections are invertible, in effect it does a
 * projection using the transposed matrix of the previous one.
 *
 * In the case of PCA and RCA, it does correspond to the inverse projection,
 * because the projection matrix is the unitary basis for the covariance
 * matrix, which is positive-(semi)definite.
 *
 * The resulting layout will be the same as if you had used the MergeRegion
 * transformation on the dataset before calling the projection transformation.
 *
 * @b NB: this transformation requires that the last transformation applied to
 *        the dataset is a projection.
 */
class InverseProjection : public Analyzer {

 public:
  InverseProjection(const ParameterMap& params);
  virtual ~InverseProjection() {};

  Transformation analyze(const DataSet* dataset) const;


 protected:
  int _targetDimension;
  Real _coveredVariance;
};

} // namespace gaia2

#endif // GAIA_INVERSEPROJECTION_H
