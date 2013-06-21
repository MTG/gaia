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
#include "convert.h"

namespace gaia2 {
namespace convert {

std::string toString(const QString& qstr) {
  return qstr.toUtf8().data();
}

std::vector<std::string> QStringList_to_VectorString(const QList<QString>& qlist) {
  std::vector<std::string> result(qlist.size());
  for (int i=0; i<qlist.size(); i++) {
    result[i] = qlist.at(i).toUtf8().data();
  }
  return result;
}

QStringList VectorString_to_QStringList(const std::vector<std::string>& svector) {
  QStringList result;
  for (uint i=0; i<svector.size(); i++) {
    result << QString::fromUtf8(svector.at(i).c_str(),
                                svector.at(i).size());
  }
  return result;
}

std::vector<float> RealDescriptor_to_VectorFloat(const RealDescriptor& rdesc) {
  std::vector<float> result(rdesc.size());
  for (int i=0; i<rdesc.size(); i++) {
    result[i] = rdesc[i];
  }
  return result;
}

RealDescriptor VectorFloat_to_RealDescriptor(const std::vector<float>& fvector) {
  RealDescriptor result((int)fvector.size(), (Real)0);
  for (int i=0; i<result.size(); i++) {
    result[i] = fvector[i];
  }
  return result;
}

std::vector<std::string> StringDescriptor_to_VectorString(const StringDescriptor& sdesc) {
  std::vector<std::string> result(sdesc.size());
  for (int i=0; i<sdesc.size(); i++) {
    result[i] = sdesc[i].toUtf8().data();
  }
  return result;
}

StringDescriptor VectorString_to_StringDescriptor(const std::vector<std::string>& svector) {
  StringDescriptor result((int)svector.size(), QString());
  for (int i=0; i<(int)result.size(); i++) {
    result[i] = QString::fromUtf8(svector[i].c_str(), svector[i].size());
  }
  return result;
}

std::vector<std::pair<std::string, float> > SearchResults_to_StlTypes(const SearchResults& results) {
  std::vector<std::pair<std::string, float> > svector(results.size());

  for (int i=0; i<results.size(); i++) {
    gaia2::Result pair = results[i];
    svector[i] = std::make_pair(std::string(pair.first.toUtf8().data()), pair.second);
  }

  return svector;
}

QList<QPointF> VectorPair_to_QPointList(const std::vector<std::pair<float, float> >& pvector) {
  QList<QPointF> result;

  for (int i=0; i<(int)pvector.size(); i++) {
    result << QPointF(pvector[i].first, pvector[i].second);
  }

  return result;
}

std::vector<std::pair<float, float> > QPointList_to_VectorPair(const QList<QPointF>& plist) {
  std::vector<std::pair<float, float> > result(plist.size());

  for (int i=0; i<plist.size(); i++) {
    result[i] = std::make_pair((float)plist[i].x(), (float)plist[i].y());
  }

  return result;
}

} // namespace convert
} // namespace gaia2
