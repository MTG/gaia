#include "distancefunction.h"
#include "../metrics/distancefunction.h"
#include "dataset.h"
#include "parameter.h"
#include "../dataset.h"

namespace gaia2std {

PROXY_DESTRUCTOR(DistanceFunction);

DistanceFunction::DistanceFunction(const std::string& name,
                                   const DataSet* dataset,
                                   const ParameterMap& params) :
  _ownsData(true),
  _d(::gaia2::MetricFactory::create(QString::fromUtf8(name.c_str(), name.size()),
                                   dataset->impl()->layout(),
                                   *params.impl())) {}


float DistanceFunction::operator()(const Point& p1, const Point& p2) {
  return _d->operator()(*p1.impl(), *p2.impl());
}



} // namespace gaia2std
