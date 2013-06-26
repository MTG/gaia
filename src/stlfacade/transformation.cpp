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

#include "transformation.h"
#include "../transformation.h"
#include "dataset.h"

namespace gaia2std {

PROXY_DESTRUCTOR(Transformation);

proxy_ptr<DataSet> Transformation::applyToDataSet(const DataSet* dataset) const {
  ::gaia2::DataSet* ds = _d->applyToDataSet(dataset->impl());
  proxy_ptr<DataSet> result(new DataSet(ds));
  // pass ownership to caller of function
  result->_ownsData = true;
  return result;
}

proxy_ptr<Point> Transformation::applyToPoint(const Point* point) const {
  ::gaia2::Point* p = _d->applyToPoint(point->impl());
  proxy_ptr<Point> result(new Point(p));
  // pass ownership to caller of function
  result->_ownsData = true;
  return result;
}

} // namespace gaia2std
