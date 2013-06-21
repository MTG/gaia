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
#include "rhythmdistance.h"
#include "gaiamath.h"
using namespace gaia2;


RhythmDistance::RhythmDistance(const PointLayout& layout,
                               const ParameterMap& params) : DistanceFunction(layout, params) {

  validParams = QStringList() << "descriptorName" << "indexRange" << "alpha";

  try {
    Region region = layout.descriptorLocation(params.value("descriptorName")).select(RealType);
    _fixedLength = (region.lengthType() == FixedLength);
    _index = region.index();
  }
  catch (GaiaException&) {
    throw GaiaException("RhythmDistance: you need to specify exactly one Real descriptor of size 1");
  }

  Array<Real> idxRange = params.value("indexRange").toArrayReal();
  const int size = idxRange.size();
  _indexRange.resize(size);
  for (int i=0; i<size; i++) {
    _indexRange[i] = (int)idxRange[i];
  }

  _alpha = params.value("alpha").toDouble();
  _alphaWeights.resize(size);
  for (int i=0; i<size; i++) {
    // NB: use double for additional precision (needed here)
    _alphaWeights[i] = intpow((double)_alpha, (int)_indexRange[i]-1);
  }

}

Real RhythmDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real x, y;

  if (_fixedLength) {
    x = p1.frealData(seg1)[_index];
    y = p2.frealData(seg2)[_index];
  }
  else {
    x = p1.vrealData(seg1)[_index][0];
    y = p2.vrealData(seg2)[_index][0];
  }

  if (x > y) {
    return distance(x, y, p1.name(), p2.name());
  }
  else {
    return distance(y, x, p2.name(), p1.name());
  }
}

Real RhythmDistance::distance(const Real& x, const Real& y, const QString& t1, const QString& t2) const {
  if (y == 0) {
    G_INFO("WARNING: Computing rhythm distance when one descriptor is 0... p1:" << t1 << "- p2:" << t2);
    return 0;
  }

  Real ratio = x/y;
  Real result = _alphaWeights[0] * qAbs(ratio - _indexRange[0]);

  for (int i=1; i<_indexRange.size(); i++) {
    result = qMin(result, _alphaWeights[i] * qAbs(ratio - _indexRange[i]));
  }

  return result;
}
