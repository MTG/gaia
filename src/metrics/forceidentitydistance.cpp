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
#include "forceidentitydistance.h"
using namespace gaia2;

ForceIdentityDistance::ForceIdentityDistance(const PointLayout& layout,
                                             const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "distance" << "params";

  _dist = MetricFactory::create(params.value("distance"),
                                layout,
                                params.value("params", ParameterMap()).toParameterMap());
}


ForceIdentityDistance::~ForceIdentityDistance() {
  delete _dist;
}


Real ForceIdentityDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  if (p1.name() == p2.name()) return 0;

  return (*_dist)(p1, p2, seg1, seg2);
}
