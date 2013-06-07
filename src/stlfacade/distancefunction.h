#ifndef GAIA_STL_DISTANCEFUNCTION_H
#define GAIA_STL_DISTANCEFUNCTION_H

#include <vector>
#include "origtypes.h"

namespace gaia2std {

class DistanceFunction {
  DECLARE_PROXY(DistanceFunction);
  friend class View;

 public:
  DistanceFunction(const std::string& name,
                   const DataSet* layout,
                   const ParameterMap& params);

  float operator()(const Point& p1, const Point& p2);

};


} // namespace gaia2std

#endif // GAIA_STL_DISTANCEFUNCTION_H
