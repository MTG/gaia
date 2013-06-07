#ifndef GAIA_FROZENDISTANCE_H
#define GAIA_FROZENDISTANCE_H

#include <QVector>
#include <QPointer>
#include <Eigen/Dense>
#include "layoutawarefactory.h"
#include "frozendataset.h"
#include "utils.h"

namespace gaia2 {


class FrozenDistance {
public:
  FrozenDistance(const FrozenDataSet& dataset) : _dataset(dataset) {}
  virtual ~FrozenDistance() {}

  virtual void prepare(const FrozenPoint& query) { Q_UNUSED(query); }
  virtual Real operator()(int i, const FrozenPoint& query) const = 0;

  const FrozenDataSet& dataset() const { return _dataset; }

  // needed for the factory
  QString name;
  QStringList validParams;

protected:
  const FrozenDataSet& _dataset;
  //int _offset, _size;
};




typedef LayoutAwareFactory<QString, FrozenDistance, FrozenDataSet, ParameterMap> FrozenMetricFactory;
typedef FrozenMetricFactory FrozenDistanceFunctionFactory;

void registerFrozenMetrics();


} // namespace gaia2

#endif // GAIA_FROZENDISTANCE_H
