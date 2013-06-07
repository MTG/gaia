#include "transformation.h"
#include "../transformation.h"
#include "dataset.h"

namespace gaia2std {

PROXY_DESTRUCTOR(Transformation);

proxy_ptr<DataSet> Transformation::applyToDataSet(const DataSet* dataset) const {
  ::gaia2::DataSet* ds = _d->applyToDataSet(dataset->impl());
  proxy_ptr<DataSet> result(new DataSet(ds));
  // pass ownership to caller of function
  result->_ownsData = true;
  return result;
}

proxy_ptr<Point> Transformation::applyToPoint(const Point* point) const {
  ::gaia2::Point* p = _d->applyToPoint(point->impl());
  proxy_ptr<Point> result(new Point(p));
  // pass ownership to caller of function
  result->_ownsData = true;
  return result;
}

} // namespace gaia2std
