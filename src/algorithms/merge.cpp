#include "merge.h"
#include "utils.h"
using namespace gaia2;

Merge::~Merge() {}

Merge::Merge(const Transformation& transfo) : Applier(transfo) {
  _history1 = transfo.params.value("history1").toHistory();
  _history2 = transfo.params.value("history2").toHistory();
}


Point* Merge::mapPoint(const Point* p) const {
  Point* p1 = _history1.mapPoint(p);
  Point* p2 = _history2.mapPoint(p);

  Point* result = mergePoints(p1, p2);

  delete p1;
  delete p2;

  return result;
}



/**
 * @todo this could be optimized by copying the code from the mergeDataSets
 * utility function, but it is doubtful this will ever be useful (we shouldn't
 * be calling mapDataSet too often, but rather directly mergeDataSets).
 */
DataSet* Merge::mapDataSet(const DataSet* dataset) const {
  DataSet* ds1 = _history1.mapDataSet(dataset);
  DataSet* ds2 = _history2.mapDataSet(dataset);

  DataSet* result = mergeDataSets(ds1, ds2);

  delete ds1;
  delete ds2;

  return result;
}
