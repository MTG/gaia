#include "mergeregionapplier.h"
#include "../utils.h"
using namespace gaia2;

MergeRegionApplier::~MergeRegionApplier() {}

MergeRegionApplier::MergeRegionApplier(const Transformation& transfo) : Applier(transfo) {
  _select = transfo.params.value("descriptorNames").toStringList();
  _resultName = transfo.params.value("resultName").toString();
  _layout.add(_resultName, RealType);

  _regionCache = transfo.layout.descriptorLocation(_select);
}


Point* MergeRegionApplier::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  return mergePoint(p, _regionCache);
}

/**
 * @todo assert final merged point has the same size as the specified dimension.
 */
Point* MergeRegionApplier::mergePoint(const Point* p, const Region& region) const {
  Point* result = new Point();
  result->setName(p->name());
  result->setLayout(_layout, p->numberSegments());

  FORSEG(*result) {
    result->setValue(nseg, _resultName, mergeDescriptors(p, nseg, region));
  }

  return result;
}

