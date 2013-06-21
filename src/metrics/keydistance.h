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
#ifndef GAIA_KEYDISTANCE_H
#define GAIA_KEYDISTANCE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics refmetrics
 *
 * This class computes the harmonic distance between keys. Range is from 0 to 1.
 * This distance has been written by Nico and is more likely to serve as an
 * example than a useful distance, but still...
 *
 * @param name the name of the descriptor containing the key value. Default is
 *        "key_key".
 *
 */
class KeyDistance : public DistanceFunction {
 public:
  // @todo use keyIndex or Region?
  KeyDistance(const PointLayout& layout, const ParameterMap& params);
  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:
  int _keyIndex;

};

} // namespace gaia2

#endif // GAIA_KEYDISTANCE_H
