#ifndef GAIA_FROZENCOSINEANGLEDISTANCE_H
#define GAIA_FROZENCOSINEANGLEDISTANCE_H

#include "frozendistance.h"

namespace gaia2 {


class FrozenCosineAngleDistance : public FrozenDistance {
public:
  static const Real defaultUndefinedDistance = 0.0;

  FrozenCosineAngleDistance(const FrozenDataSet& dataset, int offset, int size)
    : FrozenDistance(dataset), _offset(offset), _size(size),
      _useDefaultValue(true), _defaultValue(defaultUndefinedDistance) {}


  FrozenCosineAngleDistance(const FrozenDataSet& dataset, const QString& descriptorName)
    : FrozenDistance(dataset) {
    init(descriptorName);
  }

  FrozenCosineAngleDistance(const FrozenDataSet& dataset, const std::string& descriptorName)
    : FrozenDistance(dataset), _useDefaultValue(true), _defaultValue(defaultUndefinedDistance) {
    init(QString::fromStdString(descriptorName));
  }

  FrozenCosineAngleDistance(const FrozenDataSet& dataset, const char* descriptorName)
    : FrozenDistance(dataset), _useDefaultValue(true), _defaultValue(defaultUndefinedDistance) {
    init(QString::fromUtf8(descriptorName));
  }


  FrozenCosineAngleDistance(const FrozenDataSet& dataset, const ParameterMap& params)
    : FrozenDistance(dataset) {

    validParams << "defaultValue";

    if (params.contains("defaultValue")) {
      _useDefaultValue = true;
      _defaultValue = params.value("defaultValue").toDouble();
    }
    else {
      _useDefaultValue = false;
    }

    if (params.contains("descriptorName")) {
      init(params.value("descriptorName"));
    }
    else {
      init(0, dataset.dimension());
    }
  }


  void init(const QString& descriptorName) {
    validParams << "descriptorName";

    QPair<int, int> pos = _dataset.descriptorLocation(descriptorName);
    init(pos.first, pos.second - pos.first);
  }

  void init(int offset, int size) {
    _offset = offset;
    _size = size;

    // precompute the norm for all the vectors in the dataset
    _norm.resize(_dataset.rows());
    for (int i=0; i<_dataset.rows(); i++) _norm[i] = _dataset.row(i).segment(_offset, _size).norm();

    // precompute a lookup table for the acos values
    _acosLUT.resize(LUT_SIZE+1); // we want to allow 0 <= x <= 1 (ie: both ends included)
    for (int i=0; i<LUT_SIZE+1; i++) {
      _acosLUT[i] = std::acos(double(i)/LUT_SIZE) / M_PI;
    }
  }

  void prepare(const FrozenPoint& query) {
    _qnorm = query.segment(_offset, _size).norm();
  }

  // return the normalized angle (ie: alpha / M_PI)
  Real acos(Real x) const {
    if (x < 0) return 1.0 - acos(-x);
    x = qMin(x, (Real)1.0);
    return _acosLUT.at((int)(x*LUT_SIZE+0.5));
  }

  Real operator()(int i, const FrozenPoint& query) const {
    Real n1 = _norm[i];
    Real n2 = _qnorm;

    static const Real epsilon = 1e-6; // arbitrary value
    if (n1*n2 < epsilon) {
      if (_useDefaultValue) {
        return _defaultValue;
      }
      QStringList msg;
      msg << "Could not compute cosine distance between '" << _dataset.pointName(i)
          << "' and query point because one of the two points is null, result is undefined";
      throw GaiaException(msg);
    }

    Real result = _dataset.row(i).segment(_offset, _size).dot(query.segment(_offset, _size) / (n1*n2));
    //return acos(clip(result, -1.0, 1.0)) / M_PI;
    return acos(result);
  }

protected:
  int _offset, _size;
  bool _useDefaultValue;
  Real _defaultValue;

  // precomputed norms for the points in the dataset on which this distance operates
  QVector<Real> _norm;
  Real _qnorm;

  // Lookup table for the acos function
  static const int LUT_SIZE = 4096;
  QVector<Real> _acosLUT;
};


} // namespace gaia2

#endif // GAIA_FROZENCOSINEANGLEDISTANCE_H
