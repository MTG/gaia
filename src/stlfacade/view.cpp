#include "../view.h"
#include "view.h"
#include "dataset.h"
#include "distancefunction.h"
#include "point.h"
using namespace std;

namespace gaia2std {

View::View(const DataSet* dataset) :
  _ownsData(true),
  _d(new ::gaia2::View(dataset->impl())) {}

PROXY_DESTRUCTOR(View);


SearchResults View::nnSearch(const Point& p, const DistanceFunction& dfunc,
                             int numNeighbors, const string& filter) {
  ::gaia2::SearchResults r = _d->nnSearch(*p.impl(),
                                          dfunc.impl(),
                                          QString::fromUtf8(filter.c_str(), filter.size())
                                          ).get(numNeighbors);
  SearchResults result(r.size());
  for (int i=0; i<r.size(); i++) {
    result[i] = make_pair(string(r[i].first.toUtf8().data()), r[i].second);
  }
  return result;
}

SearchResults View::nnSearch(const string& id, const DistanceFunction& dfunc,
                             int numNeighbors, const string& filter) {
  ::gaia2::SearchResults r = _d->nnSearch(QString::fromUtf8(id.c_str(), id.size()),
                                          dfunc.impl(),
                                          QString::fromUtf8(filter.c_str(), filter.size())
                                          ).get(numNeighbors);
  SearchResults result(r.size());
  for (int i=0; i<r.size(); i++) {
    result[i] = make_pair(string(r[i].first.toUtf8().data()), r[i].second);
  }
  return result;
}


} // namespace gaia2std
