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

#include "distribute.h"
#include "../utils.h"
using namespace gaia2;

Distribute::~Distribute() {}

Distribute::Distribute(const Transformation& transfo) : Applier(transfo) {
  ParameterMap distMap = transfo.params.value("distribution").toParameterMap();

  foreach (const QString& name, distMap.keys()) {
    _distMap.insert(name, distMap.value(name).toArrayReal());
  }
}


Point* Distribute::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  Point* result = new Point(*p);

  foreach (const QString& name, _distMap.keys()) {

    // TODO: we can optimize this expensive lookup
    QPair<QString, int> dpos = parseDimension(name);
    int idx = p->layout().descriptorLocation(dpos.first).segment().begin + dpos.second;

    const Array<Real>& dist = _distMap[name];
    int nPoints = dist.size();

    // we need outliers to not have extremely high values when taking
    // erfinv of the smallest or the highest point
    int outliers = (int)(0.00001 * nPoints);

    FORSEG(*result) {
      RealDescriptor& fdata = result->frealData(nseg);
      int rank = binarySearch(dist, fdata[idx]);

      if (rank < outliers) rank = outliers;
      if (rank > nPoints-outliers) rank = nPoints - outliers;
      // fix for outliers = 0 (few points in input vector)
      if (rank == 0) rank++;
      if (rank == nPoints) rank--;

      Real normIdx = (Real)rank / nPoints;

      fdata[idx] = erfinv(2.0*normIdx - 1.0);
    }
  }

  return result;
}
