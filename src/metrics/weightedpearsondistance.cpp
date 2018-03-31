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


#include "weightedpearsondistance.h"
#include "../utils.h"
using namespace gaia2;


WeightedPearsonDistance::WeightedPearsonDistance(const PointLayout& layout,
                                                 const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "weights";

  ParameterMap weightspm = params.value("weights").toParameterMap();

  // get the weights map, but with the full names for the descriptors
  QMap<QString, Real> weights;
  foreach (QString d, weightspm.keys()) {
    QStringList candidates = layout.descriptorNames(RealType, d);
    if (candidates.empty()) throw GaiaException("WeightedPearson: descriptor '", d, "' doesn't exist");

    // The following condition has been removed because it actually makes sense
    // to use wildcards when talking about a group of descriptors, which in some
    // cases will sum up to 1, so that assigning a weight to all of them isn't
    // proportional to the number of instances in that group.
    //if (candidates.size() > 1) throw GaiaException("WeightedPearson: descriptor '", d, "' is ambiguous");
    foreach (QString cand, candidates) {
      weights[cand] = weightspm[d].toDouble();
    }
  }


  QStringList descs = selectDescriptors(layout, RealType, weights.keys());
  Region region = checkFixedLength(layout.descriptorLocation(descs));

  QVector<int> fixedIndices = region.listIndices(RealType, FixedLength);
  _fixedl.resize(fixedIndices.size());
  for (int i=0; i<_fixedl.size(); i++) {
    QString desc = layout.descriptorName(RealType, FixedLength, fixedIndices[i]);
    _fixedl[i] = qMakePair(fixedIndices[i], weights[desc]);
  }


  // compute weightSum here cause it's constant over all points
  _weightSum = 0;
  for (int i=0; i<_fixedl.size(); i++) {
    _weightSum += _fixedl[i].second;
  }
}


Real WeightedPearsonDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  const RealDescriptor& fldescs1 = p1.frealData(seg1);
  const RealDescriptor& fldescs2 = p2.frealData(seg2);

  // compute the weighted means first
  Real x_m = 0.0, y_m = 0.0;

  for (int i=0; i<_fixedl.size(); i++) {
    int idx = _fixedl[i].first;
    Real w = _fixedl[i].second;
    x_m += w * fldescs1[idx];
    y_m += w * fldescs2[idx];
  }

  x_m /= _weightSum;
  y_m /= _weightSum;

  // compute the Pearson correlation now
  Real s1 = 0.0, s2_1 = 0.0, s2_2 = 0.0;

  for (int i=0; i<_fixedl.size(); i++) {
    int idx = _fixedl[i].first;
    Real w = _fixedl[i].second;
    Real x = fldescs1[idx] - x_m, y = fldescs2[idx] - y_m;
    s1   += w * x * y;
    s2_1 += w * x * x;
    s2_2 += w * y * y;
  }

  // need to double check: if s2_1 = 0 or s2_2 = 0, it means one point has
  // all values constant, which means it's not (?) correlated so we return 1
  if (s2_1 == 0.0 || s2_2 == 0.0) return 1.0;

  return 1.0 - s1/sqrt(s2_1 * s2_2);
}
