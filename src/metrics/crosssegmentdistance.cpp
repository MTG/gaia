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


#include "crosssegmentdistance.h"
#include "../utils.h"
using namespace gaia2;



class MinAggr : public DistAggr {
 public:
  Real initValue() const { return 1e30; }

  Real operator()(Real x, Real y) const { return qMin(x, y); }

  Real postProcess(Real x, int nsegs) const { return x; }
  virtual ~MinAggr() {}
};


class MaxAggr : public DistAggr {
 public:
  Real initValue() const { return 0; }

  Real operator()(Real x, Real y) const { return qMax(x, y); }

  Real postProcess(Real x, int nsegs) const { return x; }
  virtual ~MaxAggr() {}
};

class MeanAggr : public DistAggr {
 public:
  Real initValue() const { return 0; }

  Real operator()(Real x, Real y) const { return x + y; }

  Real postProcess(Real x, int nsegs) const { return x / nsegs; }
  virtual ~MeanAggr() {}
};


CrossSegmentDistance::~CrossSegmentDistance() {
  delete _aggr1;
  delete _aggr2;
}

CrossSegmentDistance::CrossSegmentDistance(const PointLayout& layout,
                                           const ParameterMap& params) : DistanceFunction(layout, params) {

  validParams = QStringList() << "distance" << "func1" << "func2";

  ParameterMap distParams(params);
  distParams.remove("distance");
  distParams.remove("func1");
  distParams.remove("func2");

  _dist = MetricFactory::create(params.value("distance"), layout, distParams);
  QString f1name = params.value("func1", "min").toString();
  QString f2name = params.value("func2", "min").toString();

  if      (f1name == "min")  _aggr1 = new MinAggr;
  else if (f1name == "max")  _aggr1 = new MaxAggr;
  else if (f1name == "mean") _aggr1 = new MeanAggr;
  else throw GaiaException("func1 needs to be one of [ min, max, mean ]");

  if      (f2name == "min")  _aggr2 = new MinAggr;
  else if (f2name == "max")  _aggr2 = new MaxAggr;
  else if (f2name == "mean") _aggr2 = new MeanAggr;
  else throw GaiaException("func2 needs to be one of [ min, max, mean ]");
}

Real CrossSegmentDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  if (seg1!=0 || seg2!=0) {
    throw GaiaException("CrossSegmentDistance: cannot be applied to a single segment");
  }


  int p1n = p1.numberSegments() - 1;
  int p2n = p2.numberSegments() - 1;

  Real dist = _aggr2->initValue();

  for (int p1seg=1; p1seg<=p1n; p1seg++) {
    Real segdist = _aggr1->initValue();

    for (int p2seg=1; p2seg<=p2n; p2seg++) {
      segdist = (*_aggr1)(segdist, (*_dist)(p1, p2, p1seg, p2seg));
    }

    segdist = _aggr1->postProcess(segdist, p2n);

    dist = (*_aggr2)(dist, segdist);
  }

  dist = _aggr2->postProcess(dist, p1n);

  return dist;
}



