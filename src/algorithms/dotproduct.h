#ifndef GAIA_DOTPRODUCT_H
#define GAIA_DOTPRODUCT_H

#include "applier.h"

namespace gaia2 {

typedef QMap<Segment, QPair<RealDescriptor, RealDescriptor> > CoeffMap;

/**
 * @ingroup appliers
 * DotProduct applier class. Multiplies each coordinate of the given point
 * by the ones given in the constructor and adds (optional) initial values.
 */
class DotProduct : public Applier {

 public:
  DotProduct(const Transformation& transfo);
  virtual ~DotProduct();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;


 protected:
  Point* mapPoint(const Point* p, const CoeffMap& coeffs) const;

  CoeffMap prepareCoeffMap(const PointLayout& layout) const;

  bool _applyClipping;
  CoeffMap _coeffMapCache;

};

} // namespace gaia2

#endif // GAIA_DOTPRODUCT_H
