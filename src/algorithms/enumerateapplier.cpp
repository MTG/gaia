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
