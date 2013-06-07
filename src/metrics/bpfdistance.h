#ifndef GAIA_BPFCOMPOSE_H
#define GAIA_BPFCOMPOSE_H

#include "distancefunction.h"

namespace gaia2 {

/**
 * @ingroup metrics compositemetrics
 *
 * This class composes the breakpoint function specified by its pairs of (split point, value)
 * with the given distance.
 *
 * @param distance the name of the distance to apply the BPF to.
 * @param params the parameters for creating the distance we compose with.
 * @param breakPoints the pairs (split point, value) that define the BPF.
 */
class BPFDistance : public DistanceFunction {
 public:
  BPFDistance(const PointLayout& layout, const ParameterMap& params);
  ~BPFDistance();

  Real operator()(const Point& p1, const Point& p2, int seg1, int seg2) const;

 protected:

  class BPF {
   protected:
    std::vector<Real> _xpoints, _ypoints;

   public:
    void setBreakPoints(QList<QPointF> bpoints);
    Real operator()(Real x) const;
  };

  BPF _bpf;
  DistanceFunction* _dist;
};

} // namespace gaia2

#endif // GAIA_BPFCOMPOSE_H
