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
#include "semanticclassifierdistance.h"
#include "utils.h"
using namespace gaia2;


SemanticClassifierDistance::SemanticClassifierDistance(const PointLayout& layout,
                                                       const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "alpha" << "beta" << "weights";

  _alpha = params.value("alpha").toDouble();
  _beta = params.value("beta").toDouble();

  ParameterMap weightspm = params.value("weights").toParameterMap();

  // get the weights map, but with the full names for the descriptors
  QMap<QString, Real> weights;
  foreach (QString d, weightspm.keys()) {
    QStringList candidates = layout.descriptorNames(RealType, d);
    if (candidates.empty()) throw GaiaException("SemanticClassifier: descriptor '", d, "' doesn't exist");
    if (candidates.size() > 1) throw GaiaException("SemanticClassifier: descriptor '", d, "' is ambiguous");
    weights[candidates[0]] = weightspm[d].toDouble();
  }


  QStringList descs = selectDescriptors(layout, RealType, weights.keys());
  Region region = checkFixedLength(layout.descriptorLocation(descs));

  QVector<int> fixedIndices = region.listIndices(RealType, FixedLength);
  _fixedl.resize(fixedIndices.size());
  for (int i=0; i<_fixedl.size(); i++) {
    QString desc = layout.descriptorName(RealType, FixedLength, fixedIndices[i]);
    _fixedl[i] = qMakePair(fixedIndices[i], weights[desc]);
  }

}


Real SemanticClassifierDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  const RealDescriptor& fldescs1 = p1.frealData(seg1);
  const RealDescriptor& fldescs2 = p2.frealData(seg2);

  Real dist = 0.0;

  for (int i=0; i<_fixedl.size(); i++) {
    int idx = _fixedl[i].first;
    Real w = _fixedl[i].second;

    Real num = exp(pow(fabs(fldescs1[idx]-fldescs2[idx]), _alpha));
    Real den = _beta + exp(fldescs1[idx]+fldescs2[idx]);
    dist += w * num / den;

  }

  return dist;
}
