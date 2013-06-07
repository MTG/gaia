#include "euclideandistance.h"
#include "../utils.h"
using namespace gaia2;

EuclideanDistance::EuclideanDistance(const PointLayout& layout,
                                     const ParameterMap& params)
  : DistanceFunction(layout, params) {

  _region = checkFixedLength(_region.select(RealType));
}

Real EuclideanDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real dist = 0.0;

  const RealDescriptor& fldescs1 = p1.frealData(seg1);
  const RealDescriptor& fldescs2 = p2.frealData(seg2);

  for (int i=0; i<_region.segments.size(); i++) {
    const Segment& segment = _region.segments[i];
    for (int j=segment.begin; j<segment.end; j++) {
      Real d = fldescs1[j] - fldescs2[j];
      dist += d*d;
    }
  }

  dist = sqrt(dist);

  return dist;
}
