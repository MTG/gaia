#ifndef GAIA_STL_POINT_H
#define GAIA_STL_POINT_H

#include <vector>
#include "origtypes.h"

namespace gaia2std {

class Point {
  DECLARE_PROXY(Point);
  friend class DataSet;
  friend class Collection;
  friend class Transformation;
  friend class DistanceFunction;
  friend class View;

 public:
  Point();

  std::string name() const;
  void setName(const std::string& name);

  void load(const std::string& name);

  std::vector<float> value(const std::string& name) const;
  std::vector<std::string> label(const std::string& name) const;

  std::vector<float> value(int nsegment, const std::string& name) const;
  std::vector<std::string> label(int nsegment, const std::string& name) const;

  void setValue(const std::string& name, float value);
  void setValue(const std::string& name, const std::vector<float>& value);
  void setLabel(const std::string& name, const std::string& label);
  void setLabel(const std::string& name, const std::vector<std::string>& label);

  void setValue(int nsegment, const std::string& name, float value);
  void setValue(int nsegment, const std::string& name, const std::vector<float>& value);
  void setLabel(int nsegment, const std::string& name, const std::string& label);
  void setLabel(int nsegment, const std::string& name, const std::vector<std::string>& label);

 private:
  // @todo should these go into DECLARE_PROXY?
  Point(const Point&);
  Point& operator=(const Point&);

};

} // namespace gaia2std


#endif // GAIA_STL_POINT_H
