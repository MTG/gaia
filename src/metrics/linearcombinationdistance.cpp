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
#include "linearcombinationdistance.h"
using namespace gaia2;

LinearCombinationDistance::LinearCombinationDistance(const PointLayout& layout,
                                                     const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "NO_PARAMS_CHECK";

  foreach (const QString& name, params.keys()) {
    ParameterMap dparams = params.value(name).toParameterMap();

    // create auxiliary distance function
    DistanceFunction* dfunc = MetricFactory::create(dparams.value("distance").toString(),
                                                    layout,
                                                    dparams.value("params").toParameterMap());

    // add it with its corresponding weight
    addDistance(dfunc, dparams.value("weight").toDouble());
  }
}

LinearCombinationDistance::~LinearCombinationDistance() {
  for (int i=0; i<_weights.size(); i++) {
    delete _weights[i].first;
  }
}


void LinearCombinationDistance::addDistance(DistanceFunction* f, Real coeff) {
  //_weights.insert(f, coeff);
  _weights.append(qMakePair(f, coeff));
}

Real LinearCombinationDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real dist = 0.0;
  for (int i=0; i<_weights.size(); i++) {
    dist += _weights[i].second * (*_weights[i].first)(p1, p2, seg1, seg2);
  }
  return dist;
}
