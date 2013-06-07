#include "summedsegmentdistance.h"
#include "utils.h"
using namespace gaia2;

SummedSegmentDistance::SummedSegmentDistance(const PointLayout& layout,
                                             const ParameterMap& params) : DistanceFunction(layout, params) {

  validParams = QStringList() << "distance" << "params" << "cross";

  _dist = MetricFactory::create(params.value("distance").toString(),
                                layout,
                                params.value("params").toParameterMap());
  _cross = params.value("cross").toBool();
}

Real SummedSegmentDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real dist = 0.0;

  if (seg1!=0 || seg2!=0) {
    throw GaiaException("SummedSegmentDistance: cannot be applied to a single segment");
  }

  int p1n = p1.numberSegments() - 1;
  int p2n = p2.numberSegments() - 1;
  //int minn = std::min(p1n,p2n);
  int maxn = std::max(p1n,p2n);

  if (_cross) {
    for (int p1seg=0; p1seg<maxn; p1seg++) {
      for (int p2seg=0; p2seg<maxn; p2seg++) {
        dist += (*_dist)(p1, p2, 1 + (p1seg % p1n), 1 + (p2seg % p2n));
      }
    }
  }
  else {
    for (int seg=0; seg < maxn; seg++) {
      dist += (*_dist)(p1, p2, 1 + (seg % p1n) , 1 + (seg % p2n));
    }
  }

  return dist;
}
