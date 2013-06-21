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
#include "enumerateapplier.h"
using namespace gaia2;

EnumerateApplier::~EnumerateApplier() {}

EnumerateApplier::EnumerateApplier(const Transformation& transfo) : Applier(transfo) {
  _select = transfo.params.value("descriptorNames").toStringList();

  // precompute regions
  _newLayout = mapLayout(transfo.layout);

  Region all = _newLayout.correspondingRegion();
  createIndexMappings(transfo.layout, _newLayout, all, _realMap, _stringMap, _enumMap);
}


PointLayout EnumerateApplier::mapLayout(const PointLayout& layout) const {
  PointLayout newLayout = layout.copy();

  foreach (const QString& name, _select) {
    newLayout.enumerate(name);
  }

  return newLayout;
}

Point* EnumerateApplier::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  return gaia2::mapPoint(p, _newLayout, _realMap, _stringMap, _enumMap);
}


DataSet* EnumerateApplier::mapDataSet(const DataSet* dataset) const {
  MAPDATASET_PREPARE;
  PointLayout layout;

  MAPDATASET_LOOP_POINT;
  Point* newp = gaia2::mapPoint(p, _newLayout, _realMap, _stringMap, _enumMap);
  MAPDATASET_END_LOOP;
}
