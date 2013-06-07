#ifndef GAIA_RENAMEAPPLIER_H
#define GAIA_RENAMEAPPLIER_H

#include "applier.h"
#include "utils.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * Rename applier class. Renames a descriptor.
 */
class RenameApplier : public Applier {

 public:
  RenameApplier(const Transformation& transfo);
  virtual ~RenameApplier();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  QString _oldName, _newName;
  PointLayout _newLayout;
  IndexMap _realMap, _stringMap, _enumMap;

};

} // namespace gaia2

#endif // GAIA_RENAMEAPPLIER_H
