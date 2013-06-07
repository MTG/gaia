#include "randomdistance.h"
#include "../utils.h"
using namespace gaia2;

// The implementation uses the MersenneTwister.h file downloaded from:
// http://www-personal.engin.umich.edu/~wagnerr/MersenneTwister.html

RandomDistance::RandomDistance(const PointLayout& layout,
                               const ParameterMap& params) : DistanceFunction(layout, params) {
  validParams << "range";

  _range = params.value("range", 1.0).toDouble();
  _mtrand.seed();
}

Real RandomDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Q_UNUSED(p1); Q_UNUSED(p2);
  Q_UNUSED(seg1); Q_UNUSED(seg2);
  return _range * _mtrand();
}
