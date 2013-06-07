#include "forceidentitydistance.h"
using namespace gaia2;

ForceIdentityDistance::ForceIdentityDistance(const PointLayout& layout,
                                             const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "distance" << "params";

  _dist = MetricFactory::create(params.value("distance"),
                                layout,
                                params.value("params", ParameterMap()).toParameterMap());
}


ForceIdentityDistance::~ForceIdentityDistance() {
  delete _dist;
}


Real ForceIdentityDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  if (p1.name() == p2.name()) return 0;

  return (*_dist)(p1, p2, seg1, seg2);
}
