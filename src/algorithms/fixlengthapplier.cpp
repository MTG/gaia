#include "fixlengthapplier.h"
using namespace gaia2;

FixLengthApplier::~FixLengthApplier() {}

FixLengthApplier::FixLengthApplier(const Transformation& transfo) : Applier(transfo) {
  _select = transfo.params.value("descriptorNames").toStringList();
}


PointLayout FixLengthApplier::mapLayout(const Point* samplePoint) const {
  const PointLayout& layout = samplePoint->layout();
  PointLayout newLayout = layout.copy();

  QList<QPair<QString, int> > descs;

  foreach (const QString& name, _select) {
    switch (layout.descriptorLocation(name).type()) {
    case RealType:
      descs << qMakePair(name, samplePoint->value(name).size());
      break;
    case StringType:
    case EnumType:
      descs << qMakePair(name, samplePoint->label(name).size());
      break;
    default:
      throw GaiaException("INTERNAL ERROR: Unknown type for FixLength::mapLayout...");
    }
  }

  newLayout.fixLength(descs);

  return newLayout;
}

void FixLengthApplier::createMappings(const Point* p) {
  _newLayout = mapLayout(p);

  Region all = _newLayout.correspondingRegion();
  createIndexMappings(p->layout(), _newLayout, all, _realMap, _stringMap, _enumMap);
}


Point* FixLengthApplier::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  if (_newLayout.isEmpty()) const_cast<FixLengthApplier*>(this)->createMappings(p);

  return gaia2::mapPoint(p, _newLayout, _realMap, _stringMap, _enumMap);
}
