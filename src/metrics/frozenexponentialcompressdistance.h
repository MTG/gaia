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

#ifndef GAIA_FROZENEXPONENTIALCOMPRESSDISTANCE_H
#define GAIA_FROZENEXPONENTIALCOMPRESSDISTANCE_H

#include "frozendistance.h"

namespace gaia2 {


class FrozenExponentialCompressDistance : public FrozenDistance {
public:
  FrozenExponentialCompressDistance(const FrozenDataSet& dataset, const ParameterMap& params)
    : FrozenDistance(dataset) {

    validParams << "distance" << "params" << "alpha";

    if (params.empty()) throw GaiaException("Cannot create an exponential compress distance with no params...");

    QString dname = params["distance"].toString().toLower();
    ParameterMap dparams = params["params"].toParameterMap();

    _dist = FrozenMetricFactory::create(dname, dataset, dparams);
    _alpha = params.value("alpha", 1.0).toDouble();

  }

  ~FrozenExponentialCompressDistance() {
    delete _dist;
  }

  void prepare(const FrozenPoint& query) {
    _dist->prepare(query);
  }

  Real operator()(int i, const FrozenPoint& query) const {
    return 1 - exp(-_alpha * (*_dist)(i, query));
  }

protected:
  FrozenDistance* _dist;
  Real _alpha;
};



} // namespace gaia2

#endif // GAIA_FROZENEXPONENTIALCOMPRESSDISTANCE_H
