#ifndef GAIA_FIXLENGTHAPPLIER_H
#define GAIA_FIXLENGTHAPPLIER_H

#include "applier.h"
#include "utils.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * FixLengthApplier applier class. Fixes the length of the specified descriptors.
 */
class FixLengthApplier : public Applier {
 protected:
  QStringList _select;

 public:
  FixLengthApplier(const Transformation& transfo);
  virtual ~FixLengthApplier();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  PointLayout mapLayout(const Point* samplePoint) const;
  void createMappings(const Point* p);
  PointLayout _newLayout;
  IndexMap _realMap, _stringMap, _enumMap;

};

} // namespace gaia2

#endif // GAIA_FIXLENGTHAPPLIER_H
