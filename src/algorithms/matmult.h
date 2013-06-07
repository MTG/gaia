#ifndef GAIA_MATMULT_H
#define GAIA_MATMULT_H

#include "applier.h"
#include "Eigen/Core"

namespace gaia2 {

/**
 * @ingroup appliers
 * MatMult class.
 */
class MatMult : public Applier {

 public:
  MatMult(const Transformation& transfo);
  virtual ~MatMult();

  /**
   * Ownership of resulting point is handed to caller of this function.
   */
  virtual Point* mapPoint(const Point* p) const;

 protected:
  QStringList _descriptorNames;
  Eigen::MatrixXf _matrix;
  PointLayout _layout;

  Point* mapPoint(const Point* p, const Region& region) const;

  Region _regionCache;
};

} // namespace gaia2

#endif // GAIA_MATMULT_H
