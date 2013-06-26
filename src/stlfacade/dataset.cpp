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

#include "dataset.h"
#include "../dataset.h"
#include "../convert.h"
using namespace std;

namespace gaia2std {


DataSet::DataSet() : _ownsData(true), _d(new ::gaia2::DataSet)  {}

PROXY_DESTRUCTOR(DataSet);

string DataSet::name() const {
  return _d->name().toUtf8().data();
}

void DataSet::setName(const string& name) {
  _d->setName(QString::fromUtf8(name.c_str(), name.size()));
}

void DataSet::setReferenceDataSet(DataSet* dataset) {
  if (!dataset) {
    throw gaia2std::GException("Reference data set is null");
  }
  _d->setReferenceDataSet(dataset->_d);
}

vector<string> DataSet::pointNames() const {
  return ::gaia2::convert::QStringList_to_VectorString(_d->pointNames());
}

proxy_ptr<const Point> DataSet::point(const string& id) const {
  const ::gaia2::Point* p = _d->point(QString::fromUtf8(id.c_str(), id.size()));
  return proxy_ptr<const Point>(new Point(p));
}

proxy_ptr<Point> DataSet::point(const string& id) {
  ::gaia2::Point* p = _d->point(QString::fromUtf8(id.c_str(), id.size()));
  return proxy_ptr<Point>(new Point(p));
}


void DataSet::addPoint(Point* point) {
  assert(point->_ownsData == true);
  _d->addPoint(point->impl());
  point->_ownsData = false;
}

void DataSet::removePoint(const string& id) {
  _d->removePoint(QString::fromUtf8(id.c_str(), id.size()));
}

bool DataSet::empty() const {
  return _d->empty();
}

int DataSet::size() const {
  return _d->size();
}

void DataSet::load(const string& filename) {
  _d->load(QString::fromUtf8(filename.c_str(), filename.size()));
}

void DataSet::save(const string& filename) {
  _d->save(QString::fromUtf8(filename.c_str(), filename.size()));
}


} // namespace gaia2std
