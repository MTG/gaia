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

#include "removedesc.h"
using namespace gaia2;

RemoveDesc::~RemoveDesc() {}

RemoveDesc::RemoveDesc(const Transformation& transfo) : Applier(transfo) {
  _remove = transfo.params.value("descriptorNames").toStringList();

  _newLayout = mapLayout(transfo.layout);
  Region all = _newLayout.correspondingRegion();
  createIndexMappings(transfo.layout, _newLayout, all, _realMap, _stringMap, _enumMap);
}


PointLayout RemoveDesc::mapLayout(const PointLayout& layout) const {
  PointLayout result = layout.copy();

  result.remove(_remove);

  return result;
}


Point* RemoveDesc::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  return gaia2::mapPoint(p, _newLayout, _realMap, _stringMap, _enumMap);
}
