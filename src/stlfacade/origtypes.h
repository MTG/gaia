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
#ifndef GAIA_STL_ORIGTYPES_H
#define GAIA_STL_ORIGTYPES_H

#include <string>
#include <assert.h>
#include "baseexception.h"

namespace gaia2 {

class DataSet;
class Point;
class PointLayout;
class Transformation;
class Analyzer;
class ParameterMap;
class DistanceFunction;
class SearchPoint;

template <typename DataSetType, typename PointType, typename SearchPointType, typename DistanceType>
class BaseView;
typedef BaseView<DataSet, Point, SearchPoint, DistanceFunction> View;


} // namespace gaia2


namespace gaia2std {

// include inside the namespace
#include "counted_ptr.h"

#define proxy_ptr counted_ptr

// forward-declare some classes so they can all be friends!
class DataSet;
class Collection;
class Point;
class ParameterMap;
class DistanceFunction;

} // namespace gaia2std


// useful define to make sure we define everything necessary for proxy-classes
#define DECLARE_PROXY(classname)              \
 public:                                      \
  ~classname();                               \
 private:                                     \
  bool _ownsData;                             \
  ::gaia2::classname* _d;                      \
  classname(const ::gaia2::classname* c) :     \
    _ownsData(false),                         \
    _d(const_cast< ::gaia2::classname*>(c)) {} \
  ::gaia2::classname* impl() const {           \
    return _d;                                \
  }

#define PROXY_DESTRUCTOR(classname) \
classname::~classname() {           \
  if (_ownsData) delete _d;         \
}

#endif // GAIA_STL_ORIGTYPES_H
