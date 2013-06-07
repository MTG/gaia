#ifndef GAIA_STL_DATASET_H
#define GAIA_STL_DATASET_H

#include <vector>
#include <string>
#include "origtypes.h"
#include "point.h"
#include "transformation.h"

namespace gaia2std {

class DataSet {
  DECLARE_PROXY(DataSet);
  friend class Analyzer;
  friend class Transformation;
  friend class DistanceFunction;
  friend class View;

 public:
  DataSet();

  std::string name() const;
  void setName(const std::string& name);
  void setReferenceDataSet(DataSet* dataset);

  std::vector<std::string> pointNames() const;

  proxy_ptr<const Point> point(const std::string& id) const;
  proxy_ptr<Point> point(const std::string& id);

  void addPoint(Point* point);
  void removePoint(const std::string& id);

  bool empty() const;
  int size() const;

  void load(const std::string& filename);
  void save(const std::string& filename);
};

} // namespace gaia2std

#endif // GAIA_STL_DATASET_H
