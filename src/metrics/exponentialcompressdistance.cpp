#include "exponentialcompressdistance.h"
#include "../utils.h"
#include <cmath>
using namespace gaia2;

ExponentialCompressDistance::ExponentialCompressDistance(const PointLayout& layout,
                                                         const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "distance" << "params" << "alpha";

  _dist = MetricFactory::create(params.value("distance"),
                                layout,
                                params.value("params", ParameterMap()).toParameterMap());

  _alpha = params.value("alpha", 1.0).toDouble();
}


ExponentialCompressDistance::~ExponentialCompressDistance() {
  delete _dist;
}


Real ExponentialCompressDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  return 1 - exp(-_alpha * (*_dist)(p1, p2, seg1, seg2));
}
