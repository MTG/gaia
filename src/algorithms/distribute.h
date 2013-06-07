#ifndef GAIA_DISTRIBUTE_H
#define GAIA_DISTRIBUTE_H

#include "applier.h"

namespace gaia2 {

/**
 * @ingroup appliers
 * Distribute applier class. Rerank descriptor and gives it a new value so that
 * it fits in a specific distribution (gaussian for now, but could be extended
 * to any type of distribution).
 */
class Distribute : public Applier {

 public:
  Distribute(const Transformation& transfo);
  virtual ~Distribute();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

  //virtual DataSet* mapDataSet(const DataSet* dataset) const;

 protected:
  QMap<QString, Array<Real> > _distMap;

};

} // namespace gaia2

#endif // GAIA_DISTRIBUTE_H
