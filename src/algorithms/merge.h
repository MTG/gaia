#ifndef GAIA_MERGE_H
#define GAIA_MERGE_H

#include "applier.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * Merge applier class. Merges together 2 points which have had a different
 * history of transformations, but which both stem from the same original
 * point.
 */
class Merge : public Applier {

 protected:
  TransfoChain _history1, _history2;

 public:
  Merge(const Transformation& transfo);
  virtual ~Merge();

  virtual Point* mapPoint(const Point* p) const;

  /**
   * Optimized version so that we don't have to recompute the index mappings
   * for each point.
   */
  DataSet* mapDataSet(const DataSet* dataset) const;

};

} // namespace gaia2

#endif // GAIA_MERGE_H
