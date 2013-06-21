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
#include "extractapplier.h"
using namespace gaia2;

ExtractApplier::~ExtractApplier() {}

ExtractApplier::ExtractApplier(const Transformation& transfo) : Applier(transfo) {
  _dims = dimensionListToMapping(transfo.params.value("dimensions").toStringList());


  // create a new layout to be merged so we just have
  // to copy the data each time we need it
  G_DEBUG(GAlgorithms, "ExtractApplier: Creating new layout...");
  // get all the dim names:
  QStringList fulldims;
  foreach (QString name, _dims.keys()) {
    foreach (int dim, _dims[name]) {
      fulldims << (name + "[%1]").arg(dim);
    }
  }

  // at the moment, we assume only Real descriptors (imposed by Extract analyzer)
  PointLayout addedLayout;
  foreach (QString name, fulldims) {
    addedLayout.add(name, RealType);
    addedLayout.fixLength(name, 1);
  }

  // precompute mappings
  G_DEBUG(GAlgorithms, "ExtractApplier: merging layouts...");
  _newLayout = mergeLayouts(transfo.layout, addedLayout);

  G_DEBUG(GAlgorithms, "ExtractApplier: getting index mappings...");
  Region region = _newLayout.descriptorLocation(transfo.layout.descriptorNames());
  createIndexMappings(transfo.layout, _newLayout, region, _realMap, _stringMap, _enumMap);

  _mapping = getMappings(_newLayout);
}


QList<QPair<InnerDim, InnerDim> > ExtractApplier::getMappings(const PointLayout& layout) const {
  // TODO: we should assert that the dimension number doesn't exceed the actual
  // dimension of the descriptor
  QList<QPair<InnerDim, InnerDim> > mapping;
  foreach (QString desc, _dims.keys()) {
    for (int i=0; i<_dims[desc].size(); i++) {
      int dim = _dims[desc][i];
      Segment origpos = layout.descriptorLocation(desc).segment();
      Segment destpos = layout.descriptorLocation((desc+"[%1]").arg(dim)).segment();
      mapping.append(qMakePair(InnerDim(origpos.ltype, origpos.begin, dim),
                               InnerDim(destpos.ltype, destpos.begin, 0)));
    }
  }

  return mapping;
}

void ExtractApplier::transferData(Point* p, const Mapping& m) const {
  for (int i=0; i<m.size(); i++) {
    const InnerDim& orig = m[i].first;
    const InnerDim& dest = m[i].second;

    FORSEG(*p) {

      switch (orig.ltype) {

      case VariableLength: switch (dest.ltype) {

        case VariableLength:
          p->vrealData(nseg)[dest.desc][0] = p->vrealData(nseg)[orig.desc][orig.dim];
          continue;

        case FixedLength:
          p->frealData(nseg)[dest.desc] = p->vrealData(nseg)[orig.desc][orig.dim];
          continue;
        }

      case FixedLength: switch (dest.ltype) {

        case VariableLength:
          p->vrealData(nseg)[dest.desc][0] = p->frealData(nseg)[orig.desc + orig.dim];
          continue;

        case FixedLength:
          p->frealData(nseg)[dest.desc] = p->frealData(nseg)[orig.desc + orig.dim];
          continue;
        }
      }
    }
  }
}

Point* ExtractApplier::mapPoint(const Point* p) const {
  checkLayout(p->layout());

  Point* result = gaia2::mapPoint(p, _newLayout, _realMap, _stringMap, _enumMap);
  transferData(result, _mapping);

  return result;
}
