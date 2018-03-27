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

/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/


#include "linalg.h"
#include "gaia.h"
#include "gaiamath.h"

namespace gaia2 {


template<typename T>
Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> covarianceMatrix_tmpl(const PointArray& v,
                                                                       const Region& region) {
  int nsegments = v.totalSegments();

  if (nsegments == 0) {
    throw GaiaException("Cannot compute the covariance matrix of 0 points");
  }

  try {
    const PointLayout* layout = &v.samplePoint()->layout();
    region.checkTypeOnlyFrom(RealType, layout);
    region.checkLengthTypeOnlyFrom(FixedLength, layout);
  }
  catch (GaiaException& e) {
    throw GaiaException("Gaia can only compute the covariance matrix of fixed-length real descriptors, however: ", e.msg());
  }

  typedef Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> Matrix;
  typedef Eigen::Matrix<T, Eigen::Dynamic, 1> Vector;

  const PointLayout& layout = v.samplePoint()->layout();
  int dimension = region.dimension(RealType);
  G_DEBUG(GMath, "Covariance matrix dimension:" << dimension);

  // put everything in a single matrix for faster operations on it
  Matrix trans(nsegments, dimension);

  int pidx, dim = 0;
  foreach (const Segment& seg, region.segments) {
    G_DEBUG(GMath, "desc:" << layout.descriptorName(RealType, FixedLength, seg.begin)
                << "- size:" << (seg.end - seg.begin));
    // for each dimension in descriptor
    for (int i=seg.begin; i<seg.end; i++) {
      pidx = 0;
      for (int j=0; j<v.size(); j++) {
        const Point* p = v.at(j);
        FORSEG(*p) {
          trans(pidx, dim) = p->frealData(nseg)[i];
          pidx++;
        }
      }
      dim++; // next dimension in output matrix
    }
  }

  Q_ASSERT(dim = dimension);

  // compute and substract means first
  Vector means = trans.colwise().sum() / nsegments;

  for (int i=0; i<dimension; i++) {
    trans.col(i).array() -= means[i];
  }

  // compute covariance matrix
  Matrix cov(dimension, dimension);
  for (int i=0; i<dimension; i++) {
    for (int j=0; j<=i; j++) {
      Real covij = trans.col(i).dot(trans.col(j));
      cov(i, j) = cov(j, i) = covij / (nsegments - 1); // unbiased estimator
    }
  }

  return cov;
}

Eigen::MatrixXf covarianceMatrixSinglePrecision(const PointArray& v,
                                                const Region& region) {
  return covarianceMatrix_tmpl<float>(v, region);
}

Eigen::MatrixXd covarianceMatrix(const PointArray& v,
                                 const Region& region) {
  return covarianceMatrix_tmpl<double>(v, region);
}



} // namespace gaia2
