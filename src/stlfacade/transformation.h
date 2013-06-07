#ifndef GAIA_STL_TRANSFORMATION_H
#define GAIA_STL_TRANSFORMATION_H

#include "origtypes.h"

namespace gaia2std {

class Transformation {
  DECLARE_PROXY(Transformation);
  friend class Analyzer;

 public:
  proxy_ptr<DataSet> applyToDataSet(const DataSet* dataset) const;
  proxy_ptr<Point> applyToPoint(const Point* point) const;  
};

} // namespace gaia2std


#endif // GAIA_STL_TRANSFORMATION_H
