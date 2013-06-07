#ifndef GAIA_STL_PARAMETER_H
#define GAIA_STL_PARAMETER_H

#include <vector>
#include "origtypes.h"

namespace gaia2std {

class ParameterMap {
  DECLARE_PROXY(ParameterMap);
  friend class Analyzer;
  friend class DistanceFunction;

 public:
  ParameterMap();

  void insert(const std::string& key, const std::string& value);
  void insert(const std::string& key, float value);
  void insert(const std::string& key, std::vector<std::string> value);
  void insert(const std::string& key, std::vector<float> value);

};


} // namespace gaia2std

#endif // GAIA_STL_PARAMETER_H
