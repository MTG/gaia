#ifndef GAIA_MERGEREGIONAPPLIER_H
#define GAIA_MERGEREGIONAPPLIER_H

#include "applier.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * MergeRegion applier class. Merges multiple real descriptors into a single one.
 */
class MergeRegionApplier : public Applier {
 protected:
  QString _resultName;
  QStringList _select;
  PointLayout _layout;

 public:
  MergeRegionApplier(const Transformation& transfo);
  virtual ~MergeRegionApplier();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  Point* mergePoint(const Point* p, const Region& region) const;

  // WARNING: this makes it non-thread safe
  mutable PointLayout _layoutCache;
  mutable Region _regionCache;

};

} // namespace gaia2

#endif // GAIA_MERGEREGIONAPPLIER_H
