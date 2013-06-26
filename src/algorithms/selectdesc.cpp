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

#include "selectdesc.h"
using namespace gaia2;

SelectDesc::~SelectDesc() {}

SelectDesc::SelectDesc(const Transformation& transfo) : Applier(transfo) {
  _select = transfo.params.value("descriptorNames").toStringList();

  _newLayout = mapLayout(transfo.layout);
  Region all = _newLayout.correspondingRegion();
  createIndexMappings(transfo.layout, _newLayout, all, _realMap, _stringMap, _enumMap);
}

PointLayout SelectDesc::mapLayout(const PointLayout& layout) const {
  PointLayout result = layout.copy();

  // it is probably easier to remove those that aren't in the list from a copy,
  // rather than creating a new layout from scratch and filling it with the
  // wanted descriptors
  QStringList allNames = layout.descriptorNames();
  QStringList toremove;
  foreach (QString name, allNames) {
    if (!_select.contains(name)) {
      toremove << name;
    }
  }

  result.remove(toremove);

  return result;
}


Point* SelectDesc::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  return gaia2::mapPoint(p, _newLayout, _realMap, _stringMap, _enumMap);
}
