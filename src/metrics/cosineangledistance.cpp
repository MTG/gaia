#include "cosineangledistance.h"
#include "cosinesimilarity.h"
#include "../utils.h"
#include <cmath>
using namespace gaia2;

CosineAngleDistance::CosineAngleDistance(const PointLayout& layout,
                                         const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams << "defaultValue";

  ParameterMap cosDistParams(params);
  if (cosDistParams.contains("defaultValue")) {
    cosDistParams.insert("defaultValue", cos(params.value("defaultValue").toDouble() * M_PI));
  }

  _cosDist = new CosineSimilarity(layout, cosDistParams);
}


CosineAngleDistance::~CosineAngleDistance() {
  delete _cosDist;
}


Real CosineAngleDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  return acos((*_cosDist)(p1, p2, seg1, seg2)) / M_PI;
}
