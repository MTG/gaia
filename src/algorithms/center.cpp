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

#include <cmath>
#include "center.h"
#include "../utils.h"
#include "algoutils.h"

namespace gaia2 {

Center::Center(const ParameterMap& params) : Analyzer(params) {}


Transformation Center::analyze(const DataSet* dataset, const Region& descs) const {
  G_INFO("Doing center analysis...");
  checkDataSet(dataset);
  checkMinPoints(dataset, 2);
  Region region = checkFixedLength(descs.select(RealType), dataset->layout());

  QPair<Point, Point> meanVar = getRegionMeanVar(*dataset, region);
  const Point& mean = meanVar.first;
  const Point& var  = meanVar.second;

  Transformation result(dataset->layout());
  result.analyzerName = "center";
  result.analyzerParams = _params;
  result.applierName = "dotproduct";

  ParameterMap stats;

  foreach (const Segment& seg, region.segments) {
    // FIXME: not necessarily correct if some adjacent descriptors have been
    //        merged into a single segment
    QString descriptorName = mean.layout().descriptorName(RealType, FixedLength, seg.begin);
    ParameterMap meanvar;
    meanvar.insert("mean", mean.value(descriptorName));
    meanvar.insert("var", var.value(descriptorName));
    stats.insert(descriptorName, meanvar);
  }

  result.info = stats;

  ParameterMap& applierParams = result.params;
  applierParams.insert("applyClipping", _params.value("applyClipping", false));
  ParameterMap coeffMap;

  foreach (const Segment& seg, region.segments) {
    QString descriptorName = mean.layout().descriptorName(RealType, FixedLength, seg.begin);

    ParameterMap coeffs;
    coeffs.insert("a", RealDescriptor(seg.end - seg.begin, 1.0));
    coeffs.insert("b", -mean.value(descriptorName));

    coeffMap.insert(descriptorName, coeffs);
  }

  applierParams.insert("coeffs", coeffMap);

  return result;
}


} // namespace gaia2
