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

#include "keydistance.h"

namespace gaia2 {

class Key {
 protected:
  int _note;

 public:
  Key(QString key) {
    switch (key[0].toUpper().toLatin1()) {
    case 'A': _note = 0; break;
    case 'B': _note = 2; break;
    case 'C': _note = 3; break;
    case 'D': _note = 5; break;
    case 'E': _note = 7; break;
    case 'F': _note = 8; break;
    case 'G': _note = 10; break;
    default: throw GaiaException("impossible to parse key: ", key);
    }

    // is there an alteration or not?
    if (key.size() == 2) {
      switch (key[1].toLatin1()) {
      case '#': _note++; break;
      case 'b': _note--; break;
      default: throw GaiaException("impossible to parse key: ", key);
      }
    }

    _note = (_note + 12) % 12;
  }

  Real dist(const Key& k) const {
    int dist = (_note - k._note + 12) % 12;
    switch (dist) {
    case 0: return 0.0f;

    case 1:
    case 11: return 0.8f;

    case 2:
    case 10: return 0.6f;

    case 3:
    case 9: return 0.3f;

    case 4:
    case 8: return 0.4f;

    case 5:
    case 7: return 0.2f;

    case 6: return 0.5f;

    default:
      throw GaiaException("INTERNAL ERROR"); // unreachable
    }
  }
};


KeyDistance::KeyDistance(const PointLayout& layout, const ParameterMap& params)
  : DistanceFunction(layout, params) {

  QString keyName = "key_key";
  if (params.contains("name")) {
    keyName = params.value("name");
  }

  _keyIndex = layout.descriptorLocation(keyName).index();
}

Real KeyDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Key key1(p1.vstringData(seg1)[_keyIndex].toSingleValue());
  Key key2(p2.vstringData(seg2)[_keyIndex].toSingleValue());

  return key1.dist(key2);
}

} // namespace gaia2
