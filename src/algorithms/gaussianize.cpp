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

#include <QVector>
#include <cmath>
#include "gaussianize.h"
#include "../utils.h"

namespace gaia2 {

Gaussianize::Gaussianize(const ParameterMap& params) : Analyzer(params) {
  validParams << "distributionSize";

  _maxDistSize = _params.value("distributionSize", 10000).toInt();
}


Transformation Gaussianize::analyze(const DataSet* dataset, const Region& descs) const {
  G_INFO("Doing gaussianize analysis...");
  checkDataSet(dataset);
  checkMinPoints(dataset, 2);
  Region region = checkFixedLength(descs.select(RealType), dataset->layout());

  int reduxCoeff = (int)ceil((float)dataset->totalSegments() / _maxDistSize);

  // get the distribution for each descriptor
  ParameterMap distMap;

  Array<Real> dist(_maxDistSize, 0.0);
  foreach (const Segment& seg, region.segments) {
    for (int idx=seg.begin; idx<seg.end; idx++) {

      // get distribution for this descriptor
      int p = 0; // point/segment index in distribution

      for (int i=0; i<dataset->size(); i+=reduxCoeff) {
        const Point* pt = dataset->at(i);
        FORSEG(*pt) {
          dist[p++] = pt->frealData(nseg)[idx];
        }
      }

      Q_ASSERT(p <= _maxDistSize);
      dist.resize(p);
      sort(dist.begin(), dist.end());

      QString descriptorName = dataset->layout().descriptorName(RealType, FixedLength, idx) +
                               '[' + QString::number(idx - seg.begin) + ']';

      distMap.insert(descriptorName, dist);
    }
  }

  Transformation result(dataset->layout());
  result.analyzerName = "gaussianize";
  result.analyzerParams = _params;
  result.applierName = "distribute";
  result.params = ParameterMap();
  result.params.insert("distribution", distMap);

  return result;
}


} // namespace gaia2
