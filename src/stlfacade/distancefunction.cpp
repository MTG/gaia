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
#include "distancefunction.h"
#include "../metrics/distancefunction.h"
#include "dataset.h"
#include "parameter.h"
#include "../dataset.h"

namespace gaia2std {

PROXY_DESTRUCTOR(DistanceFunction);

DistanceFunction::DistanceFunction(const std::string& name,
                                   const DataSet* dataset,
                                   const ParameterMap& params) :
  _ownsData(true),
  _d(::gaia2::MetricFactory::create(QString::fromUtf8(name.c_str(), name.size()),
                                   dataset->impl()->layout(),
                                   *params.impl())) {}


float DistanceFunction::operator()(const Point& p1, const Point& p2) {
  return _d->operator()(*p1.impl(), *p2.impl());
}



} // namespace gaia2std
