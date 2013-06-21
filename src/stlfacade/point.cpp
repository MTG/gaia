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
#include "point.h"
#include "../point.h"
#include "../convert.h"
using namespace std;

namespace gaia2std {

Point::Point() : _ownsData(true), _d(new ::gaia2::Point) {}

Point::~Point() {
  if (_ownsData) delete _d;
}

string Point::name() const {
  return _d->name().toUtf8().data();
}

void Point::setName(const string& name) {
  _d->setName(QString::fromUtf8(name.c_str(), name.size()));
}

void Point::load(const string& name) {
  _d->load(QString::fromUtf8(name.c_str(), name.size()));
}

vector<float> Point::value(const string& name) const {
  ::gaia2::RealDescriptor desc = _d->value(QString::fromUtf8(name.c_str(), name.size()));
  return ::gaia2::convert::RealDescriptor_to_VectorFloat(desc);
}

vector<float> Point::value(int nsegment, const string& name) const {
  ::gaia2::RealDescriptor desc = _d->value(nsegment, QString::fromUtf8(name.c_str(), name.size()));
  return ::gaia2::convert::RealDescriptor_to_VectorFloat(desc);
}

vector<string> Point::label(const string& name) const {
  ::gaia2::StringDescriptor desc = _d->label(QString::fromUtf8(name.c_str(), name.size()));
  return ::gaia2::convert::StringDescriptor_to_VectorString(desc);
}

vector<string> Point::label(int nsegment, const string& name) const {
  ::gaia2::StringDescriptor desc = _d->label(nsegment, QString::fromUtf8(name.c_str(), name.size()));
  return ::gaia2::convert::StringDescriptor_to_VectorString(desc);
}

void Point::setValue(const string& name, float value) {
  _d->setValue(QString::fromUtf8(name.c_str(), name.size()), value);
}

void Point::setValue(int nsegment, const string& name, float value) {
  _d->setValue(nsegment, QString::fromUtf8(name.c_str(), name.size()), value);
}

void Point::setValue(const string& name, const vector<float>& value) {
  _d->setValue(QString::fromUtf8(name.c_str(), name.size()),
               ::gaia2::convert::VectorFloat_to_RealDescriptor(value));
}

void Point::setValue(int nsegment, const string& name, const vector<float>& value) {
  _d->setValue(nsegment, QString::fromUtf8(name.c_str(), name.size()),
               ::gaia2::convert::VectorFloat_to_RealDescriptor(value));
}

void Point::setLabel(const string& name, const string& label) {
  _d->setLabel(QString::fromUtf8(name.c_str(), name.size()),
               QString::fromUtf8(label.c_str(), label.size()));
}

void Point::setLabel(int nsegment, const string& name, const string& label) {
  _d->setLabel(nsegment,
               QString::fromUtf8(name.c_str(), name.size()),
               QString::fromUtf8(label.c_str(), label.size()));
}

void Point::setLabel(const string& name, const vector<string>& label) {
  _d->setLabel(QString::fromUtf8(name.c_str(), name.size()),
               ::gaia2::convert::VectorString_to_StringDescriptor(label));
}

void Point::setLabel(int nsegment, const string& name, const vector<string>& label) {
  _d->setLabel(nsegment, QString::fromUtf8(name.c_str(), name.size()),
               ::gaia2::convert::VectorString_to_StringDescriptor(label));
}


Point::Point(const Point&) {
  assert(false);
}

Point& Point::operator=(const Point&) {
  assert(false);
  return *this;
}


} // namespace gaia2std
