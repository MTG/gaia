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

#include "addfieldapplier.h"
using namespace gaia2;

AddFieldApplier::~AddFieldApplier() {}

AddFieldApplier::AddFieldApplier(const Transformation& transfo) : Applier(transfo) {
  _real = transfo.params.value("real", QStringList()).toStringList();
  _string = transfo.params.value("string", QStringList()).toStringList();

  // create a default point using the new layout to be merged so we just have
  // to copy its data each time we need it

  // 1- create the layout
  G_DEBUG(GAlgorithms, "AddFieldApplier: Creating new layout...");
  PointLayout addedLayout;
  ParameterMap size = transfo.params.value("size", ParameterMap()).toParameterMap();

  foreach (QString name, _real) {
    if (size.contains(name)) addedLayout.add(name, RealType, FixedLength, size.value(name).toInt());
    else                     addedLayout.add(name, RealType);
  }
  foreach (QString name, _string) {
    if (size.contains(name)) addedLayout.add(name, StringType, FixedLength, size.value(name).toInt());
    else                     addedLayout.add(name, StringType);
  }

  // 2- create the point and set its values to default
  G_DEBUG(GAlgorithms, "AddFieldApplier: Creating new point with default values...");
  _addedPoint.setLayout(addedLayout);
  foreach (QString name, _real)   FORSEG(_addedPoint) {
    if (size.contains(name)) _addedPoint.setValue(nseg, name, RealDescriptor(size.value(name).toInt(), 0));
    else                     _addedPoint.setValue(nseg, name, RealDescriptor());
  }
  foreach (QString name, _string) FORSEG(_addedPoint) {
    if (size.contains(name)) _addedPoint.setLabel(nseg, name, StringDescriptor(size.value(name).toInt(), ""));
    else                     _addedPoint.setLabel(nseg, name, StringDescriptor());
  }

  ParameterMap defaultValues = transfo.params.value("default", ParameterMap()).toParameterMap();
  foreach (QString name, defaultValues.keys()) {
    if (_real.contains(name)) {
      FORSEG(_addedPoint) _addedPoint.setValue(nseg, name, defaultValues[name].toRealDescriptor());
    }
    else if (_string.contains(name)) {
      FORSEG(_addedPoint) _addedPoint.setLabel(nseg, name, defaultValues[name].toStringDescriptor());
    }
    else {
      throw GaiaException("AddFieldApplier: default value provided for unknown new desccriptor: ", name);
    }
  }

  // NB: this is a hack for multi-segment points, it should probably be done in a
  //     cleaner way, but in the meantime, this'll do.
  //     We create 10 points, each of which has from 1 to 10 segments. This is "needed"
  //     because the transferPointData function we use later needs both source and
  //     destination points to have the same number of segments. The clean solution
  //     would be to rewrite the transfer function to work on segments.
  for (int i=1; i<=10; i++) {
    Point p;
    p.setLayout(addedLayout, i);
    for (int j=0; j<i; j++) p.setSegment(j, &_addedPoint, 0);
    _addedPoints << p;
  }

  // precompute the regions and index mappings
  G_DEBUG(GAlgorithms, "AddFieldApplier: merging layouts...");
  _newLayout = mergeLayouts(transfo.layout, addedLayout);

  G_DEBUG(GAlgorithms, "AddFieldApplier: getting index mappings...");
  IndexMap realMap1, stringMap1, enumMap1, realMap2, stringMap2, enumMap2;
  Region region1 = _newLayout.descriptorLocation(transfo.layout.descriptorNames());
  Region region2 = _newLayout.descriptorLocation(addedLayout.descriptorNames());
  createIndexMappings(transfo.layout, _newLayout, region1, _realMap1, _stringMap1, _enumMap1);
  createIndexMappings(addedLayout, _newLayout, region2, _realMap2, _stringMap2, _enumMap2);
}


inline void checkNSegments(const Point* p) {
  if (p->numberSegments() > 10) {
    throw GaiaException("The AddField transformation is currently limited to points with 10 or fewer segments.\n"
                        "Please contact nicolas.wack@upf.edu to remove this inane limitation.");
  }
}

Point* AddFieldApplier::mapPoint(const Point* p) const {
  checkNSegments(p);
  // copy original data
  Point* result = gaia2::mapPoint(p, _newLayout, _realMap1, _stringMap1, _enumMap1);
  // also add the new fields with their default values
  transferPointData(&_addedPoints[result->numberSegments()-1], result, _realMap2, _stringMap2, _enumMap2);

  return result;
}


DataSet* AddFieldApplier::mapDataSet(const DataSet* dataset) const {
  MAPDATASET_PREPARE;

  MAPDATASET_LOOP_POINT;
  checkNSegments(p);
  Point* newp = gaia2::mapPoint(p, _newLayout, _realMap1, _stringMap1, _enumMap1);
  transferPointData(&_addedPoints[newp->numberSegments()-1], newp, _realMap2, _stringMap2, _enumMap2);
  MAPDATASET_END_LOOP;
}
