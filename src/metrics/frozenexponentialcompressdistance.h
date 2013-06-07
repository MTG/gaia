#ifndef GAIA_FROZENEXPONENTIALCOMPRESSDISTANCE_H
#define GAIA_FROZENEXPONENTIALCOMPRESSDISTANCE_H

#include "frozendistance.h"

namespace gaia2 {


class FrozenExponentialCompressDistance : public FrozenDistance {
public:
  FrozenExponentialCompressDistance(const FrozenDataSet& dataset, const ParameterMap& params)
    : FrozenDistance(dataset) {

    validParams << "distance" << "params" << "alpha";

    if (params.empty()) throw GaiaException("Cannot create an exponential compress distance with no params...");

    QString dname = params["distance"].toString().toLower();
    ParameterMap dparams = params["params"].toParameterMap();

    _dist = FrozenMetricFactory::create(dname, dataset, dparams);
    _alpha = params.value("alpha", 1.0).toDouble();

  }

  ~FrozenExponentialCompressDistance() {
    delete _dist;
  }

  void prepare(const FrozenPoint& query) {
    _dist->prepare(query);
  }

  Real operator()(int i, const FrozenPoint& query) const {
    return 1 - exp(-_alpha * (*_dist)(i, query));
  }

protected:
  FrozenDistance* _dist;
  Real _alpha;
};



} // namespace gaia2

#endif // GAIA_FROZENEXPONENTIALCOMPRESSDISTANCE_H
