#include "semanticclassifierdistance.h"
#include "utils.h"
using namespace gaia2;


SemanticClassifierDistance::SemanticClassifierDistance(const PointLayout& layout,
                                                       const ParameterMap& params)
  : DistanceFunction(layout, params) {

  validParams = QStringList() << "alpha" << "beta" << "weights";

  _alpha = params.value("alpha").toDouble();
  _beta = params.value("beta").toDouble();

  ParameterMap weightspm = params.value("weights").toParameterMap();

  // get the weights map, but with the full names for the descriptors
  QMap<QString, Real> weights;
  foreach (QString d, weightspm.keys()) {
    QStringList candidates = layout.descriptorNames(RealType, d);
    if (candidates.empty()) throw GaiaException("SemanticClassifier: descriptor '", d, "' doesn't exist");
    if (candidates.size() > 1) throw GaiaException("SemanticClassifier: descriptor '", d, "' is ambiguous");
    weights[candidates[0]] = weightspm[d].toDouble();
  }


  QStringList descs = selectDescriptors(layout, RealType, weights.keys());
  Region region = checkFixedLength(layout.descriptorLocation(descs));

  QVector<int> fixedIndices = region.listIndices(RealType, FixedLength);
  _fixedl.resize(fixedIndices.size());
  for (int i=0; i<_fixedl.size(); i++) {
    QString desc = layout.descriptorName(RealType, FixedLength, fixedIndices[i]);
    _fixedl[i] = qMakePair(fixedIndices[i], weights[desc]);
  }

}


Real SemanticClassifierDistance::operator()(const Point& p1, const Point& p2, int seg1, int seg2) const {
  const RealDescriptor& fldescs1 = p1.frealData(seg1);
  const RealDescriptor& fldescs2 = p2.frealData(seg2);

  Real dist = 0.0;

  for (int i=0; i<_fixedl.size(); i++) {
    int idx = _fixedl[i].first;
    Real w = _fixedl[i].second;

    Real num = exp(pow(fabs(fldescs1[idx]-fldescs2[idx]), _alpha));
    Real den = _beta + exp(fldescs1[idx]+fldescs2[idx]);
    dist += w * num / den;

  }

  return dist;
}
