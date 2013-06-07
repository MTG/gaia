#ifndef GAIA_ENUMERATEAPPLIER_H
#define GAIA_ENUMERATEAPPLIER_H

#include "applier.h"
#include "utils.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * EnumerateApplier applier class. Fixes the length of the specified descriptors.
 */
class EnumerateApplier : public Applier {
 protected:
  QStringList _select;

 public:
  EnumerateApplier(const Transformation& transfo);
  virtual ~EnumerateApplier();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

  /**
   * Optimized version so that we don't have to recompute the index mappings
   * for each point.
   */
  DataSet* mapDataSet(const DataSet* dataset) const;

 protected:
  PointLayout mapLayout(const PointLayout& layout) const;

  PointLayout _newLayout;
  IndexMap _realMap, _stringMap, _enumMap;

};

} // namespace gaia2

#endif // GAIA_ENUMERATEAPPLIER_H
