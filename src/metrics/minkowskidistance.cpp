#include "minkowskidistance.h"
using namespace gaia2;

MinkowskiDistance::MinkowskiDistance(const PointLayout& layout, const ParameterMap& params)
  : DistanceFunction(layout, params) {
  // get exponent from params
  // generate lookup table
}

Real MinkowskiDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  throw GaiaException("Minkowski distance not implemented at the moment... Please file a feature request in the Trac system.");
  // foreach segment
  // foreach dimension
  /*
    for();
    return ...;
  */
}
