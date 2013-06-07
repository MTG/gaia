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
