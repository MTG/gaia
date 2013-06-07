#include "linearcombinationdistance.h"
using namespace gaia2;

LinearCombinationDistance::LinearCombinationDistance(const PointLayout& layout,
                                                     const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "NO_PARAMS_CHECK";

  foreach (const QString& name, params.keys()) {
    ParameterMap dparams = params.value(name).toParameterMap();

    // create auxiliary distance function
    DistanceFunction* dfunc = MetricFactory::create(dparams.value("distance").toString(),
                                                    layout,
                                                    dparams.value("params").toParameterMap());

    // add it with its corresponding weight
    addDistance(dfunc, dparams.value("weight").toDouble());
  }
}

LinearCombinationDistance::~LinearCombinationDistance() {
  for (int i=0; i<_weights.size(); i++) {
    delete _weights[i].first;
  }
}


void LinearCombinationDistance::addDistance(DistanceFunction* f, Real coeff) {
  //_weights.insert(f, coeff);
  _weights.append(qMakePair(f, coeff));
}

Real LinearCombinationDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  Real dist = 0.0;
  for (int i=0; i<_weights.size(); i++) {
    dist += _weights[i].second * (*_weights[i].first)(p1, p2, seg1, seg2);
  }
  return dist;
}
