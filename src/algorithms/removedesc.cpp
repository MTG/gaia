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
