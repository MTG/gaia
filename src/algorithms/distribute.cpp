#include "distribute.h"
#include "utils.h"
using namespace gaia2;

Distribute::~Distribute() {}

Distribute::Distribute(const Transformation& transfo) : Applier(transfo) {
  ParameterMap distMap = transfo.params.value("distribution").toParameterMap();

  foreach (const QString& name, distMap.keys()) {
    _distMap.insert(name, distMap.value(name).toArrayReal());
  }
}


Point* Distribute::mapPoint(const Point* p) const {
  checkLayout(p->layout());
  Point* result = new Point(*p);

  foreach (const QString& name, _distMap.keys()) {

    // TODO: we can optimize this expensive lookup
    QPair<QString, int> dpos = parseDimension(name);
    int idx = p->layout().descriptorLocation(dpos.first).segment().begin + dpos.second;

    const Array<Real>& dist = _distMap[name];
    int nPoints = dist.size();

    // we need outliers to not have extremely high values when taking
    // erfinv of the smallest or the highest point
    int outliers = (int)(0.00001 * nPoints);

    FORSEG(*result) {
      RealDescriptor& fdata = result->frealData(nseg);
      int rank = binarySearch(dist, fdata[idx]);

      if (rank < outliers) rank = outliers;
      if (rank > nPoints-outliers) rank = nPoints - outliers;
      // fix for outliers = 0 (few points in input vector)
      if (rank == 0) rank++;
      if (rank == nPoints) rank--;

      Real normIdx = (Real)rank / nPoints;

      fdata[idx] = erfinv(2.0*normIdx - 1.0);
    }
  }

  return result;
}
