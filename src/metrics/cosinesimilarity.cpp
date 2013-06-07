#include "cosinesimilarity.h"
#include "../utils.h"
using namespace gaia2;

CosineSimilarity::CosineSimilarity(const PointLayout& layout,
                                   const ParameterMap& params) : DistanceFunction(layout, params) {

  validParams << "defaultValue";

  _region = checkFixedLength(_region.select(RealType));

  if (params.contains("defaultValue")) {
    _useDefaultValue = true;
    _defaultValue = params.value("defaultValue").toDouble();
  }
  else {
    _useDefaultValue = false;
  }
}

Real CosineSimilarity::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real dist = 0.0;
  Real norm1 = 0.0, norm2 = 0.0;

  const RealDescriptor& fldescs1 = p1.frealData(seg1);
  const RealDescriptor& fldescs2 = p2.frealData(seg2);

  for (int i=0; i<_region.segments.size(); i++) {
    const Segment& segment = _region.segments[i];
    for (int j=segment.begin; j<segment.end; j++) {
      Real v1 = fldescs1[j];
      Real v2 = fldescs2[j];
      dist += v1*v2;
      norm1 += v1*v1;
      norm2 += v2*v2;
    }
  }

  static const Real epsilon = 1e-12; // arbitrary value
  if (norm1 * norm2 < epsilon) {
    if (_useDefaultValue) {
      return _defaultValue;
    }
    QStringList msg;
    msg << "Could not compute cosine distance between '" << p1.name()
        << "' and '" << p2.name() << "' because: "
        << "one of the two points is null, result is undefined";
    throw GaiaException(msg);
  }

  dist /= sqrt(norm1 * norm2);

  // make sure we don't overflow stupidly because of precision issues...
  dist = clip(dist, -1.0, 1.0);

  return dist;
}
