#ifndef GAIA_STL_VIEW_H
#define GAIA_STL_VIEW_H

#include <vector>
#include "origtypes.h"

namespace gaia2std {

typedef std::pair<std::string, float> Result;
typedef std::vector<Result> SearchResults;


class View {
  DECLARE_PROXY(View);

 public:
  View(const DataSet* dataset);

  SearchResults nnSearch(const Point& p, const DistanceFunction& dfunc,
                         int numNeighbors, const std::string& filter = "");

  SearchResults nnSearch(const std::string& id, const DistanceFunction& dfunc,
                         int numNeighbors, const std::string& filter = "");

};


} // namespace gaia2std

#endif // GAIA_STL_VIEW_H
