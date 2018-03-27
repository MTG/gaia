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
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

/* <copyright entity="UPF">
# UPF. All Right Reserved, http://www.upf.edu/
#
# This source is subject to the Contributor License Agreement of the Essentia project.
# Please see the CLA.txt file available at http://essentia.upf.edu/contribute/
# for more
# information.
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
# PARTICULAR PURPOSE.
#
# </copyright>
*/


#ifndef GAIA_CONVERT_H
#define GAIA_CONVERT_H

#include <vector>
#include <string>
#include <QStringList>
#include "descriptor.h"
#include "view.h"

namespace gaia2 {

/**
 * Utility namespace used for various types of conversion.
 */
namespace convert {

/**
 * Converts a QString into a std::string.
 */
std::string toString(const QString& qstr);

/**
 * Converts a QStringList into a std::vector<std::string>.
 */
std::vector<std::string> QStringList_to_VectorString(const QList<QString>& qlist);

/**
 * Converts a std::vector<std::string> into a QStringList.
 */
QStringList VectorString_to_QStringList(const std::vector<std::string>& svector);

/**
 * Converts a RealDescriptor into a std::vector<float>.
 */
std::vector<float> RealDescriptor_to_VectorFloat(const RealDescriptor& rdesc);

/**
 * Converts a std::vector<float> into a RealDescriptor.
 */
RealDescriptor VectorFloat_to_RealDescriptor(const std::vector<float>& fvector);

/**
 * Converts a StringDescriptor into a std::vector<std::string>.
 */
std::vector<std::string> StringDescriptor_to_VectorString(const StringDescriptor& sdesc);

/**
 * Converts a std::vector<std::string> into a StringDescriptor.
 */
StringDescriptor VectorString_to_StringDescriptor(const std::vector<std::string>& svector);

/**
 * Converts a gaia2::SearchResults to its STL equivalent (std::vector<std::pair<std::string, float> > >).
 */
std::vector<std::pair<std::string, float> > SearchResults_to_StlTypes(const SearchResults& results);

/**
 * Converts a std::vector<std::pair<float, float> > to a QList<PointF>.
 */
QList<QPointF> VectorPair_to_QPointList(const std::vector<std::pair<float, float> >& pvector);

/**
 * Converts a QList<PointF> to a std::vector<std::pair<float, float> >.
 */
std::vector<std::pair<float, float> > QPointList_to_VectorPair(const QList<QPointF>& plist);


/**
 * Converts a std::vector of something into a QList of the same objects
 */
template <typename T>
QList<T> Vector_to_QList(const std::vector<T>& v) {
  QList<T> result;
  for (int i=0; i<(int)v.size(); i++) result << v[i];
  return result;
}


} // namespace convert
} // namespace gaia2

#endif // GAIA_CONVERT_H
