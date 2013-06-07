#ifndef GAIA_DISTANCEFUNCTION_H
#define GAIA_DISTANCEFUNCTION_H

#include "layoutawarefactory.h"
#include "parameter.h"
#include "point.h"

namespace gaia2 {


/**
 * @ingroup metrics
 * @todo should the function be virtual or not? (calling overhead) (template?)
 * This class is the base distance function class which all specific distance
 * functions must inherit from.
 *
 */
class DistanceFunction {
 public:

  /**
   * Name for the metric, usually the key that was used to instantiate it
   * from the factory.
   */
  QString name;

  DistanceFunction(const PointLayout& layout, const ParameterMap& params);
  virtual ~DistanceFunction() {}

  /**
   * This function computes the distance between the two given points. It needs
   * to be implemented in each subclass. It takes as optional arguments the number
   * of the segments on which to operate.
   */
  virtual Real operator()(const Point& p1, const Point& p2,
                          int seg1 = 0, int seg2 = 0) const = 0;

  const PointLayout& layout() const { return _layout; }

  /**
   * Checks that the given Region only contains fixed-length descriptors and throws
   * an exception if not.
   * @throw GaiaException if the region contained variable-length descriptors.
   */
  const Region& checkFixedLength(const Region& region) const;

  /**
   * List of valid parameters this analyzer accepts. It needs to be filled in
   * the constructor of the derived analyzer, and will be checked by the factory
   * just after instantiation.
   */
  QStringList validParams;

 protected:
  PointLayout _layout;
  ParameterMap _params;
  Region _region;


};


typedef LayoutAwareFactory<QString, DistanceFunction, PointLayout, ParameterMap> MetricFactory;
typedef MetricFactory DistanceFunctionFactory;

typedef QPair<int, Real> DimWeight;

void registerMetrics();


} // namespace gaia2

#endif // GAIA_DISTANCEFUNCTION_H
