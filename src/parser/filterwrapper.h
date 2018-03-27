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


#ifndef GAIA_PARSER_FILTERWRAPPER_H
#define GAIA_PARSER_FILTERWRAPPER_H

#include "filter.h"
#include "convert.h"

namespace gaia2 {
namespace filter {

/**
 * The FilterWrapper class serves as an easy wrapper for a subset of all
 * possible filters, but with a much more intuitive API that allows filters
 * to be constructed programatically on the fly instead of being parsed from
 * a string representation.
 */
class FilterWrapper : public Filter {
  void initValueList(const QString& varname, const QString& op, const QList<Real>& values);
  void initLabelList(const QString& varname, const QString& op, const QList<QString>& labels);

 public:
  FilterWrapper(const QString& varname, const QString& op, Real value);
  FilterWrapper(const QString& varname, const QString& op, const QList<Real>& values);

  FilterWrapper(const QString& varname, const QString& op, const QString& label);
  FilterWrapper(const QString& varname, const QString& op, const QList<QString>& labels);

  FilterWrapper(const std::string& varname, const std::string& op, Real value);
  FilterWrapper(const std::string& varname, const std::string& op, const std::vector<Real>& values);

  FilterWrapper(const std::string& varname, const std::string& op, const std::string& label);
  FilterWrapper(const std::string& varname, const std::string& op, const std::vector<std::string>& labels);

};


class AndFilter : public Filter {
  void init(const QList<Filter*>& preds);

 public:
  AndFilter(const QList<Filter*>& preds)  { init(preds); }
  AndFilter(const std::vector<Filter*>& preds) { init(convert::Vector_to_QList(preds)); }

};

class OrFilter : public Filter {
  void init(const QList<Filter*>& preds);

 public:
  OrFilter(const QList<Filter*>& preds)  { init(preds); }
  OrFilter(const std::vector<Filter*>& preds) { init(convert::Vector_to_QList(preds)); }
};

class NotFilter : public Filter {
 public:
  NotFilter(const Filter* pred);
};


} // namespace filter
} // namespace gaia2


#endif // GAIA_PARSER_FILTERWRAPPER_H
