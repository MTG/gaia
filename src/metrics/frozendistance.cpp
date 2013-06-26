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

#include "frozendistance.h"

#include "frozeneuclideandistance.h"
#include "frozencosineangledistance.h"
#include "frozenlinearcombinationdistance.h"
#include "frozenexponentialcompressdistance.h"

#define REGISTER_FROZEN_METRIC(name) FrozenMetricFactory::Registrar<Frozen##name##Distance> register##name(#name);

namespace gaia2 {

void registerFrozenMetrics() {
  REGISTER_FROZEN_METRIC(Euclidean);
  REGISTER_FROZEN_METRIC(CosineAngle);
  REGISTER_FROZEN_METRIC(LinearCombination);
  REGISTER_FROZEN_METRIC(ExponentialCompress);
}

} // namespace gaia2
