#ifndef GAIA_REMOVEDESC_H
#define GAIA_REMOVEDESC_H

#include "applier.h"
#include "utils.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * RemoveDesc applier class. Removes specified dimensions from given point.
 */
class RemoveDesc : public Applier {
 protected:
  QStringList _remove;

 public:
  RemoveDesc(const Transformation& transfo);
  virtual ~RemoveDesc();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  PointLayout mapLayout(const PointLayout& layout) const;
  PointLayout _newLayout;
  IndexMap _realMap, _stringMap, _enumMap;

};

} // namespace gaia2

#endif // GAIA_REMOVEDESC_H
