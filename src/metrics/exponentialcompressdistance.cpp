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
#include "exponentialcompressdistance.h"
#include "../utils.h"
#include <cmath>
using namespace gaia2;

ExponentialCompressDistance::ExponentialCompressDistance(const PointLayout& layout,
                                                         const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "distance" << "params" << "alpha";

  _dist = MetricFactory::create(params.value("distance"),
                                layout,
                                params.value("params", ParameterMap()).toParameterMap());

  _alpha = params.value("alpha", 1.0).toDouble();
}


ExponentialCompressDistance::~ExponentialCompressDistance() {
  delete _dist;
}


Real ExponentialCompressDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  return 1 - exp(-_alpha * (*_dist)(p1, p2, seg1, seg2));
}
