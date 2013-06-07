#ifndef GAIA_ADDFIELDAPPLIER_H
#define GAIA_ADDFIELDAPPLIER_H

#include "applier.h"
#include "utils.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * AddFieldApplier applier class. Adds the specified fields to the current
 * layout.
 */
class AddFieldApplier : public Applier {
 protected:
  QStringList _real, _string;
  PointLayout _newLayout;
  Point _addedPoint;
  QList<Point> _addedPoints;
  IndexMap _realMap1, _stringMap1, _enumMap1, _realMap2, _stringMap2, _enumMap2;

 public:
  AddFieldApplier(const Transformation& transfo);
  virtual ~AddFieldApplier();

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

};

} // namespace gaia2

#endif // GAIA_ADDFIELDAPPLIER_H
