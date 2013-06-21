/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 *
 * Gaia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/
 */
#include <QSysInfo>
#include "parameter.h"
#include "transformation.h"
#include "pointlayout.h"

namespace gaia2 {

const ParameterMap noParams;

Parameter::Parameter(const Array<Real>& val) {
  if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
    throw GaiaException("Cannot use Parameter(const Array<Real>&) on a big-endian platform...");
  }

  *this = QByteArray((const char*)val.data(), val.size()*sizeof(Real));
}

Array<Real> Parameter::toArrayReal() const {
  if (QSysInfo::ByteOrder == QSysInfo::BigEndian) {
    throw GaiaException("Cannot use Parameter::toArrayReal() on a big-endian platform...");
  }

  if (!canConvert(QVariant::ByteArray)) {
    if (canConvert(QVariant::List)) {
      return toRealDescriptor();
    }
    throw GaiaException("Cannot convert this parameter to an Array<Real>.");
  }

  QByteArray data = this->toByteArray();
  Q_ASSERT(data.size() % sizeof(Real) == 0);

  Array<Real> result(data.size() / sizeof(Real));
  memcpy(result.data(), data.constData(), data.size());

  return result;
}

Parameter::Parameter(const RealDescriptor& val) {
  QList<QVariant> values;
  for (int i=0; i<val.size(); i++) {
    values << (double)val[i];
  }
  *this = values;
}

RealDescriptor Parameter::toRealDescriptor() const {
  // if it is a single real value
  if (canConvert(QVariant::Double)) {
    return RealDescriptor(1, toDouble());
  }

  // otherwise, it should be a list of real values
  QList<QVariant> valueList = this->toList();
  RealDescriptor result(valueList.size(), (Real)0.0);

  for (int i=0; i<valueList.size(); i++) {
    result[i] = valueList[i].toDouble();
  }

  return result;
}

Parameter::Parameter(const StringDescriptor& val) {
  QList<QVariant> values;
  for (int i=0; i<val.size(); i++) {
    values << val[i];
  }
  *this = values;
}

StringDescriptor Parameter::toStringDescriptor() const {
  // if it is a single string value
  if (canConvert(QVariant::String)) {
    return StringDescriptor(1, toString());
  }

  // otherwise, it should be a list of string values
  QList<QVariant> valueList = this->toList();
  StringDescriptor result(valueList.size(), "");

  for (int i=0; i<valueList.size(); i++) {
    result[i] = valueList[i].toString();
  }

  return result;
}

Parameter::Parameter(const ParameterMap& parameterMap) {
  QMap<QString, QVariant> variantMap;
  foreach (QString param, parameterMap.keys()) {
    variantMap.insert(param, parameterMap.value(param));
  }
  QVariant::operator=(variantMap);
}

ParameterMap Parameter::toParameterMap() const {
  QMap<QString, QVariant> variantMap = this->toMap();
  ParameterMap parameterMap;

  foreach (QString param, variantMap.keys()) {
    parameterMap.insert(param, variantMap.value(param));
  }

  return parameterMap;
}


Parameter::Parameter(const Eigen::MatrixXf& matrix) {
  const int nrows = matrix.rows(), ncols = matrix.cols();
  QList<QVariant> matrixList;
  matrixList << nrows << ncols;
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<ncols; j++) {
      matrixList << matrix(i, j);
    }
  }

  *this = matrixList;
}

Eigen::MatrixXf Parameter::toMatrix() const {
  QList<QVariant> matrixList = this->toList();
  const int nrows = matrixList.takeFirst().toInt();
  const int ncols = matrixList.takeFirst().toInt();

  Eigen::MatrixXf result(nrows, ncols);
  for (int i=0; i<nrows; i++) {
    for (int j=0; j<ncols; j++) {
      result(i, j) = matrixList.takeFirst().toDouble();
    }
  }

  Q_ASSERT(matrixList.empty());

  return result;
}


Parameter::Parameter(const Transformation& transfo) {
  QList<QVariant> fieldList;
  fieldList << Parameter(transfo.layout);
  fieldList << transfo.name << transfo.analyzerName << transfo.applierName;
  fieldList << Parameter(transfo.analyzerParams) << Parameter(transfo.params)
            << Parameter(transfo.info);

  *this = fieldList;
}

Transformation Parameter::toTransformation() const {
  QList<QVariant> fieldList = this->toList();
  PointLayout layout = Parameter(fieldList[0]).toPointLayout();

  Transformation result(layout);
  result.name           = fieldList[1].toString();
  result.analyzerName   = fieldList[2].toString();
  result.applierName    = fieldList[3].toString();
  result.analyzerParams = Parameter(fieldList[4]).toParameterMap();
  result.params         = Parameter(fieldList[5]).toParameterMap();
  result.info           = Parameter(fieldList[6]).toParameterMap();

  return result;
}


Parameter::Parameter(const TransfoChain& history) {
  QList<QVariant> transfos;

  foreach (Transformation transfo, history) {
    transfos << Parameter(transfo);
  }

  *this = transfos;
}

TransfoChain Parameter::toHistory() const {
  QList<QVariant> transfos = this->toList();
  TransfoChain result;

  foreach (const QVariant& transfo, transfos) {
    result << Parameter(transfo).toTransformation();
  }

  return result;
}

Parameter::Parameter(const QList<QPointF>& coords) {
  QList<QVariant> coordList;
  foreach (QPointF c, coords) coordList << QVariant(c);
  *this = coordList;
}

QList<QPointF> Parameter::toCoords() const {
  QList<QVariant> coords = this->toList();
  QList<QPointF> result;

  foreach (QVariant c, coords) result << c.toPointF();

  return result;
}

Parameter::Parameter(const PointLayout& layout) {
  QByteArray buf;
  QDataStream buffer(&buf, QIODevice::WriteOnly);
  buffer.setVersion(QDataStream::Qt_4_4);

  buffer << layout;

  *this = buf;
}

PointLayout Parameter::toPointLayout() const {
  QByteArray buf = this->toByteArray();
  QDataStream buffer(buf);
  buffer.setVersion(QDataStream::Qt_4_4);

  PointLayout layout;
  buffer >> layout;

  return layout;
}


ParameterMap toParameterMap(const yaml::Node& node) {
  ParameterMap result;
  yaml::Mapping nmap = node.mapping();

  yaml::Mapping::const_iterator it = nmap.constBegin();
  for (; it != nmap.constEnd(); ++it) {
    QString key = it.key().scalar();
    yaml::Node yvalue = it.value();

    switch (yvalue.type()) {

    case yaml::ScalarType: {
      // is it a scalar (string or real value)?
      yaml::Scalar yscalar = yvalue.scalar();

      if (yscalar.at(0).isNull()) {
        // this scalar has been quoted, do not attempt to cast into a float
        result.insert(key, yscalar);
      }
      else {
        bool ok;
        float value = yscalar.toFloat(&ok);
        if (ok) {
          result.insert(key, value);
        }
        else { // not a real problem, just that the scalar is actually a string
          result.insert(key, yscalar);
        }
      }
      break;
    }


    case yaml::SequenceType: {
      // is it a sequence?
      // NB: we only detect sequences of strings at the moment
      yaml::Sequence yseq = yvalue.sequence();

      QStringList slist;
      for (int i=0; i<yseq.size(); i++) {
        slist << yseq[i].scalar();
      }
      result.insert(key, slist);
      break;
    }

    case yaml::MappingType:
      result.insert(key, toParameterMap(yvalue));
      break;

    default:
      throw GaiaException("Unknown Yaml type...");
    }
  }

  return result;
}


} // namespace gaia2
