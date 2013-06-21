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
#include "kullbackleiblerdistance.h"
using namespace gaia2;

KullbackLeiblerDistance::KullbackLeiblerDistance(const PointLayout& layout,
                                                 const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "descriptorName";

  QString name = params.value("descriptorName").toString();

  Segment meanSegment = layout.descriptorLocation(name + ".mean").segment();
  Segment covSegment = layout.descriptorLocation(name + ".cov").segment();
  Segment icovSegment = layout.descriptorLocation(name + ".icov").segment();

  _meanIdx = meanSegment.begin; _meanIdxEnd = meanSegment.end;
  _covIdx  = covSegment.begin;  _covIdxEnd  = covSegment.end;
  _icovIdx = icovSegment.begin; _icovIdxEnd = icovSegment.end;

  if (!(meanSegment.ltype == FixedLength &&
        covSegment.ltype == FixedLength &&
        icovSegment.ltype == FixedLength)) {
    throw GaiaException("KullbackLeibler: mean, cov and icov should all be fixed-length.");
  }

  int size = _meanIdxEnd - _meanIdx;
  if ((_covIdxEnd - _covIdx != size*size+2) ||
      (_icovIdxEnd - _icovIdx != size*size+2)) {
    throw GaiaException("KullbackLeibler: mean and cov/icov dimensions do not match");
  }

}


/**
 * this function could be used to adjust the distance, using for instance an
 * exponential scaling (eg: dist = e^(a*dist+b))
 */
inline Real adjustDistance(Real dist) {
  // just make sure we don't get negative due to rounding errors
  if ((dist - 1e-5) < 0.0) return 0.0;
  return dist;
}


Real KullbackLeiblerDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real dist = 0.0;

  // distance = trace(cov1*icov2) + trace(cov2*icov1) - 2*S + trace((icov1+icov2)*(m1-m2)*(m1-m2)')

  const RealDescriptor& descs1 = p1.frealData(seg1);
  const RealDescriptor& descs2 = p2.frealData(seg2);

  int size = _meanIdxEnd - _meanIdx;
  Q_ASSERT(descs1[_covIdx] == size);
  Q_ASSERT(descs1[_covIdx+1] == size);
  Q_ASSERT(descs1[_icovIdx] == size);
  Q_ASSERT(descs1[_icovIdx+1] == size);
  Q_ASSERT(descs2[_covIdx] == size);
  Q_ASSERT(descs2[_covIdx+1] == size);
  Q_ASSERT(descs2[_icovIdx] == size);
  Q_ASSERT(descs2[_icovIdx+1] == size);

  dist += traceMatrixProduct(&descs1[_covIdx + 2], &descs2[_icovIdx + 2], size);
  dist += traceMatrixProduct(&descs2[_covIdx + 2], &descs1[_icovIdx + 2], size);
  dist += traceMatrixProductMean(&descs1[_icovIdx + 2], &descs2[_icovIdx + 2],
                                 &descs1[_meanIdx], &descs2[_meanIdx], size);
  dist -= 2*size;

  return adjustDistance(dist);
}
