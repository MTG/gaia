#ifndef GAIA_PARAMETER_H
#define GAIA_PARAMETER_H

#include <QVariant>
#include "types.h"
#include "gaia.h"
#include "descriptor.h"
#include "yamlcpp.h"
#include "Eigen/Core"

namespace gaia2 {

class Parameter;

class ParameterMap : public GaiaMap<QString, Parameter, GaiaException> {
public:
  ParameterMap() : GaiaMap<QString, Parameter, GaiaException>() {}
};

// forward declarations (for faster compilation)
class Transformation;
class TransfoChain;
class PointLayout;

/**
 * @todo WARNING: this class doesn't implement canConvert & value<T> for the
 *       custom types that have been added.
 */
class Parameter : public QVariant {
 public:
  Parameter() : QVariant() {}

  template <typename T>
  Parameter(const T& val) : QVariant(val) {}

  // automatic coercion to QString
  operator QString() const {
    return this->toString();
  }

  // starting with Qt 4.6, a new QVariant type got introduced: float
  // This is a very stupid idea as it breaks BC, and we want to avoid it
  Parameter(float x) : QVariant((double)x) {}

  // type: Array<Real>
  Parameter(const Array<Real>& val);
  Array<Real> toArrayReal() const;

  // type: RealDescriptor
  Parameter(const RealDescriptor& val);
  RealDescriptor toRealDescriptor() const;

  // type: StringDescriptor
  Parameter(const StringDescriptor& val);
  StringDescriptor toStringDescriptor() const;

  // type: ParameterMap
  Parameter(const ParameterMap& parameterMap);
  ParameterMap toParameterMap() const;

  // type: Eigen::MatrixXf
  Parameter(const Eigen::MatrixXf& matrix);
  Eigen::MatrixXf toMatrix() const;

  // type: Transformation
  Parameter(const Transformation& transfo);
  Transformation toTransformation() const;

  // type: TransfoChain (history)
  Parameter(const TransfoChain& history);
  TransfoChain toHistory() const;

  // type: QList<QPointF> (coordinates, for BPF)
  Parameter(const QList<QPointF>& coords);
  QList<QPointF> toCoords() const;

  // type: PointLayout
  Parameter(const PointLayout& layout);
  PointLayout toPointLayout() const;

};

// convenience empty ParameterMap
extern const ParameterMap noParams;


ParameterMap toParameterMap(const yaml::Node& node);

} // namespace gaia2

#endif // GAIA_PARAMETER_H
